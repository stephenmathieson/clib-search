
//
// clib-search.h
//
// Copyright (c) 2013 Stephen Mathieson
// MIT licensed
//

#ifndef __CLIB_SEARCH_H__
#define __CLIB_SEARCH_H__ 1

#define CLIB_SEARCH_VERSION "0.0.0"

#define CLIB_WIKI_URL "https://github.com/clibs/clib/wiki/Packages"

typedef int bool;
#define true 1;
#define false 0;

struct filter;

/*
 * Filters.
 */

typedef struct filter {
  char *author;
  bool executables;
  bool utilities;
} filter_t;

#endif

