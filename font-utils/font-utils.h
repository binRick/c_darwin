#pragma once
#ifndef FONT_UTILS_H
#define FONT_UTILS_H
//////////////////////////////////////
#include "c_vector/vector/vector.h"
#include "parson/parson.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
struct font_t {
  char        *file_name, *name, *type, *path, *family, *style, *fullname, *unique, *version;
  bool        enabled, valid, duplicate, copy_protected, embeddable, outline;
  JSON_Object *typefaces_o;
  size_t      size, typefaces_qty;
};
enum font_file_type_t {
  FONT_FILE_TYPE_TRUETYPE,
  FONT_FILE_TYPE_OPENTYPE,
  FONT_FILE_TYPE_POSTSCRIPT,
  FONT_FILE_TYPE_BITMAP,
  FONT_FILE_TYPES_QTY,
};
enum font_type_t {
  FONT_TYPE_COPY_PROTECTED,
  FONT_TYPE_OUTLINE,
  FONT_TYPE_EMBEDDABLE,
  FONT_TYPE_VALID,
  FONT_TYPE_INVALID,
  FONT_TYPE_DUPLICATE,
  FONT_TYPES_QTY,
};
const char *font_file_type_extensions[FONT_FILE_TYPES_QTY + 1];
const char *font_file_type_names[FONT_FILE_TYPES_QTY + 1];
//////////////////////////////////////
struct Vector *get_installed_fonts_v();
///////////////////////////////////////////////////////////////////////
#endif
