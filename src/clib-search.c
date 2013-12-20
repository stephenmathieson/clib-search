
//
// main.c
//
// Copyright (c) 2013 Stephen Mathieson
// MIT licensed
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "case.h"
#include "commander.h"
#include "fs.h"
#include "http-get.h"
#include "str-copy.h"
#include "strsplit.h"
#include "wiki-registry.h"
#include "clib-search.h"

filter_t filter;

/**
 * Check if the given `pkg` matches any `args`.
 */

static int
matches(int count, char *args[], package_t *pkg) {
  char **parts = calloc(1, strlen(pkg->repo) - 1);
  strsplit(pkg->repo, parts, "/");

  if (!filter.executables && 0 == strcmp(pkg->category, "executables")) return 0;
  if (!filter.utilities && 0 == strcmp(pkg->category, "utilities")) return 0;

  char *author = parts[0];
  case_lower(author);
  if (filter.author != NULL && 0 != strcmp(author, filter.author)) {
    count = count - 2;
    return 0;
  }

  // Display all packages if there's no query
  if (count == 0) return 1;

  char *name = parts[1];
  case_lower(name);
  for (int i = 0; i < count; i++) {
    char *idx = strstr(name, args[i]);
    if (idx) return 1;
  }

  char *description = strdup(pkg->description);
  description = case_lower(description);
  for (int i = 0; i < count; i++) {
    char *idx = strstr(description, args[i]);
    if (idx) return 1;
  }

  return 0;
}

static void
author(command_t *self) {
  filter.author = (char *)self->arg;
}

static void
executables(command_t *self) {
  filter.utilities = false;
}

static void
utilities(command_t *self) {
  filter.executables = false;
}

static char *
wiki_html_cache() {
  fs_stats *stats = fs_stat(CLIB_SEARCH_CACHE);
  if (NULL == stats) {
    goto set_cache;
  }

  long now = (long) time(NULL);
  long modified = stats->st_mtime;
  long delta = now - modified;

  free(stats);

  if (delta < CLIB_SEARCH_CACHE_TIME) {
    return fs_read(CLIB_SEARCH_CACHE);
  }

set_cache:;

  http_get_response_t *res = http_get(CLIB_WIKI_URL);
  if (!res->ok) return NULL;

  char *html = str_copy(res->data);
  http_get_free(res);

  if (NULL == html) return html;
  fs_write(CLIB_SEARCH_CACHE, html);
  return html;
}


/**
 * Entry point.
 */

int
main(int argc, char **argv) {
  filter.executables = true;
  filter.utilities = true;

  command_t program;
  command_init(&program, "clib-search", CLIB_SEARCH_VERSION);
  program.usage = "[options] [query ...]";
  command_option(&program, "-a", "--author <author>", "filter by author", author);
  command_option(&program, "-e", "--executables", "filter by executables", executables);
  command_option(&program, "-u", "--utilities", "filter by utilities", utilities);
  command_parse(&program, argc, argv);

  char *html = wiki_html_cache();
  if (NULL == html) {
    fprintf(stderr, "Failed to fetch wiki HTML\n");
    return 1;
  }

  list_t *pkgs = wiki_registry_parse(html);

  list_node_t *node;
  list_iterator_t *it = list_iterator_new(pkgs, LIST_HEAD);
  while ((node = list_iterator_next(it))) {
    package_t *pkg = (package_t *) node->val;
    if (matches(program.argc, program.argv, pkg)) {
      printf("  \033[36m%s\033[m\n", pkg->repo);
      printf("  url: \033[90m%s\033[m\n", pkg->href);
      printf("  desc: \033[90m%s\033[m\n", pkg->description);
      printf("\n");
    }
  }
  list_iterator_destroy(it);
  list_destroy(pkgs);
  command_free(&program);

  return 0;
}

