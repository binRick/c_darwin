#pragma once
#ifndef CAPTURE_UTILS_H
#define CAPTURE_UTILS_H
//////////////////////////////////////
#include "capture/window/window.h"
#include "screen/utils/utils.h"
#include "space/utils/utils.h"
#include "window/utils/utils.h"
#include <ctype.h>
#include <Carbon/Carbon.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
//////////////////////////////////////
struct capture_type_t {
  const char    *name;
  size_t        (^get_default_id)(void);
  bool          (^validate_id)(size_t id);
  CGImageRef    (^capture)(size_t id);
  CGImageRef    (^capture_rect)(size_t id, CGRect rect);
  CGImageRef    (^preview)(size_t id);
  CGImageRef    (^width)(size_t id, size_t width);
  CGImageRef    (^height)(size_t id, size_t height);
  struct Vector *(^get_items)(void);
};
struct cached_capture_item_t {
  unsigned char *pixels;
  size_t        size;
};
//////////////////////////////////////
CGImageRef capture_type_width(enum capture_type_id_t capture_type_id, size_t capture_id, size_t width);
CGImageRef capture_type_height(enum capture_type_id_t capture_type_id, size_t capture_id, size_t height);
char *capture_type_capture_png_random_file(enum capture_type_id_t capture_type_id, size_t capture_id);
bool capture_type_capture_png_file(enum capture_type_id_t capture_type_id, size_t capture_id, char *file);
char *capture_type_hash_key(enum capture_type_id_t capture_type_id, size_t capture_id);
size_t capture_type_hash(enum capture_type_id_t capture_type_id, size_t capture_id);
bool capture_type_validate_id(enum capture_type_id_t capture_type_id, size_t capture_id);
size_t capture_type_get_default_id(enum capture_type_id_t capture_type_id);
size_t capture_type_validate_id_or_get_default_id(enum capture_type_id_t capture_type_id, size_t capture_id);
struct Vector *capture_type_get_items(enum capture_type_id_t capture_type_id);
CGImageRef capture_type_id_or_default_capture_type_id(enum capture_type_id_t capture_type_id, size_t capture_id);
CGImageRef capture_type_capture(enum capture_type_id_t capture_type_id, size_t capture_id);
CGImageRef capture_type_preview(enum capture_type_id_t capture_type_id, size_t capture_id);
CGImageRef capture_type_capture_rect(enum capture_type_id_t capture_type_id, size_t capture_id, CGRect rect);
//////////////////////////////////////
#endif
