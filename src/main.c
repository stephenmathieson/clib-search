
//
// main.c
//
// Copyright (c) 2013 Stephen Mathieson
// MIT licensed
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wiki-registry.h"
#include "case.h"
#include "substr.h"
#include "commander.h"
#include "clib-search.h"

/**
 * Check if the given `pkg` matches any `args`.
 */

static int matches(int count, char *args[], package_t *pkg) {
  char *description = strdup(pkg->description);
  description = case_lower(description);

  char *name = strstr(pkg->repo, "/");
  if (name) {
    name++; // remove "/"
    case_lower(name);
    for (int i = 0; i < count; i++) {
      char *idx = strstr(name, args[i]);
      if (idx) return 1;
    }
  }


  for (int i = 0; i < count; i++) {
    char *idx = strstr(description, args[i]);
    if (idx) return 1;
  }

  return 0;
}

/**
 * Entry point.
 */

int main(int argc, const char **argv) {
  command_t program;
  command_init(&program, "clib-search", CLIB_SEARCH_VERSION);
  program.usage = "[options] [query ...]";
  command_parse(&program, argc, argv);

  // TODO local 5h cache like current node impl
  list_t *pkgs = wiki_registry(CLIB_WIKI_URL);

  list_node_t *node;
  list_iterator_t *it = list_iterator_new(pkgs, LIST_HEAD);
  while ((node = list_iterator_next(it))) {
    package_t *pkg = (package_t *) node->val;
    if (0 == program.argc || matches(program.argc, program.argv, pkg)) {
      printf("  \033[36m%s\033[m\n", pkg->repo);
      printf("  url: \033[90m%s\033[m\n", pkg->href);
      printf("  desc: \033[90m%s\033[m\n", pkg->description);
      printf("\n");
    }
  }
  list_iterator_destroy(it);
  list_destroy(pkgs);

  return 0;
}
