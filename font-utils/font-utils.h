#pragma once
#ifndef FONT_UTILS_H
#define FONT_UTILS_H
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
#include "parson/parson.h"
struct font_t {
  char        *file_name, *name, *type, *path, *family, *style, *fullname, *unique, *version;
  bool        enabled, valid, duplicate, copy_protected, embeddable, outline;
  JSON_Object *typefaces_o;
  size_t      size, typefaces_qty;
};
enum font_type_t {
  FONT_TYPE_TRUETYPE,
  FONT_TYPE_OPENTYPE,
  FONT_TYPE_POSTSCRIPT,
  FONT_TYPE_BITMAP,
  FONT_TYPES_QTY,
};
char *font_type_extensions[FONT_TYPES_QTY + 1] = {
  [FONT_TYPE_TRUETYPE]   = "ttf",
  [FONT_TYPE_OPENTYPE]   = "otf",
  [FONT_TYPE_POSTSCRIPT] = "otf",
  [FONT_TYPE_BITMAP]     = "ttf",
};
char *font_type_names[FONT_TYPES_QTY + 1] = {
  [FONT_TYPE_TRUETYPE]   = "truetype",
  [FONT_TYPE_OPENTYPE]   = "opentype",
  [FONT_TYPE_POSTSCRIPT] = "postscript",
  [FONT_TYPE_BITMAP]     = "bitmap",
};
//////////////////////////////////////
struct Vector *get_installed_fonts_v();
///////////////////////////////////////////////////////////////////////
#endif
