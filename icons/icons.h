#pragma once
#ifndef ICONS_H
#define ICONS_H
//////////////////////////////////////
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
#define MAX_ICON_CATEGORIES    32
enum icon_file_type_t {
  ICON_FILE_TYPE_ICNS,
  ICON_FILE_TYPE_PNG,
  ICON_FILE_TYPES_QTY,
};
struct icon_png_file_t {
  const char                  *path, *hash;
  const size_t                size;
  const int                   width, height;
  const unsigned char         *data;
  const int                   bits;
  const enum icon_file_type_t type;
};
struct icon_file_t {
  const char                  *path, *hash;
  const size_t                size;
  const unsigned char         *data;
  const enum icon_file_type_t type;
};
struct icon_files_t {
  struct icon_file_t     icn;
  struct icon_png_file_t *pngs;
};
struct icon_t {
  const char          *name, *categories[MAX_ICON_CATEGORIES];
  struct icon_files_t files;
};
//static const struct icon_t icons[];
#endif
