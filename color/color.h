#pragma once
#ifndef COLOR_H
#define COLOR_H
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
struct parsed_color_t {
  int  red, green, blue, alpha;
  int  brightness, hue;
  bool dark, bright, very_dark, very_bright;
  char *hex, *name, *slug, *ansi_fg, *ansi_fg_escaped, *ansi_bg, *ansi_bg_escaped;
  int  ansicode;
};
enum color_filter_t {
  COLOR_FILTER_BRIGHT      = 11,
  COLOR_FILTER_DARK        = 13,
  COLOR_FILTER_VERY_DARK   = 15,
  COLOR_FILTER_VERY_BRIGHT = 17,
};
enum color_type_t {
  COLOR_TYPE_ALL,
  COLOR_TYPE_BEST,
  __COLOR_TYPES_QTY,
};
struct Vector *color_csv_load(const char *csv_data, enum color_filter_t filter);
const char *color_csv_read(enum color_type_t type);
#endif
