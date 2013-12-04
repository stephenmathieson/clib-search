
//
// main.c
//
// Copyright (c) 2013 Stephen Mathieson
// MIT licensed
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clib-search.h"
#include "wiki-registry.h"
#include "case.h"

#define display_package(pkg) ({                           \
  printf("  \033[36m%s\033[m\n", pkg->repo);              \
  printf("  url: \033[90m%s\033[m\n", pkg->href);         \
  printf("  desc: \033[90m%s\033[m\n", pkg->description); \
  printf("\n");                                           \
})

static void usage() {
  printf("\n");
  printf("  clib-search [options] [keyword ...]\n");
  printf("\n");
  printf("  Options:\n");
  printf("\n");
  printf("    -h, --help     Output usage information\n");
  printf("    -V, --version  Output the current version\n");
  printf("\n");
  printf("  Examples:\n");
  printf("\n");
  printf("    # search for foo, bar and baz\n");
  printf("    clib-search foo bar baz\n");
  printf("\n");
  printf("    # show all registered packages\n");
  printf("    clib-search\n");
  printf("\n");
}

/**
 * Check if the given `pkg` matches any `args`.
 */

static int matches(int count, char *args[], package_t *pkg) {
  char *description = strdup(pkg->description);
  description = case_lower(description);

  // TODO check repo name

  for (int i = 1; i < count; i++) {
    char *idx = strstr(description, args[i]);
    if (idx) return 1;
  }

  return 0;
}

/**
 * Entry point.
 */

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];
    // print version and exit
    if (0 == strcmp(arg, "-V") || 0 == strcmp(arg, "--version")) {
      printf("%s\n", CLIB_SEARCH_VERSION);
      return 0;
    } else if (0 == strcmp(arg, "-h") || 0 == strcmp(arg, "--help")) {
      usage();
      return 0;
    }
    // lowercase all args here
    case_lower(argv[i]);
  }

  // TODO local 5h cache like current node impl
  list_t *pkgs = wiki_registry(CLIB_WIKI_URL);

  list_node_t *node;
  list_iterator_t *it = list_iterator_new(pkgs, LIST_HEAD);
  while ((node = list_iterator_next(it))) {
    package_t *pkg = (package_t *) node->val;
    if (1 == argc || matches(argc, argv, pkg)) {
      display_package(pkg);
    }
  }
  list_iterator_destroy(it);
  list_destroy(pkgs);

  return 0;
}
