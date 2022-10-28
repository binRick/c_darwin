#pragma once
#ifndef TESSERACT_UTILS_TYPES_H
#define TESSERACT_UTILS_TYPES_H
#include "c_vector/vector/vector.h"
#include "capture/utils/utils.h"
#include "tesseract/capi.h"
#include "tesseract/utils/utils.h"
#include "window/utils/utils.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
struct TesseractArgs {
  size_t                 id;
  char                   *file;
  unsigned char          *img;
  CGImageRef             img_ref;
  size_t                 index, img_len, img_width, img_height;
  enum image_type_id_t   format;
  enum capture_type_id_t type;
  struct Vector          *results_v;
};
struct component_image_t {
  int        mode;
  int        x, y;
  int        width, height;
  int        confidence;
  const char *text;
  char       *file;
};
struct tesseract_extract_result_source_file_t {
  char *file;
  int  width, height;
  int  stbi_format;
};
struct tesseract_determined_area_t {
  int   max_x, min_y, max_y, min_x;
  float x_max_offset_perc;
  float x_min_offset_perc;
  float y_min_offset_perc;
  float y_max_offset_perc;
  int   x_max_offset_pixels;
  int   x_min_offset_pixels;
  int   y_min_offset_pixels;
  int   y_max_offset_pixels;
};
struct tesseract_extract_result_t {
  unsigned long                                 mode, started;
  int                                           box;
  int                                           x, y;
  int                                           width, height;
  int                                           confidence;
  char                                          *text;
  char                                          *file;
  struct tesseract_extract_result_source_file_t source_file;
  struct window_info_t                          window;
  struct tesseract_determined_area_t            determined_area;
  enum capture_type_id_t                        type;
  enum image_type_id_t                          format;
  unsigned char                                 *img;
  size_t                                        img_len;
};
enum extract_mode_type_t {
  EXTRACT_MODE_TYPE_TEXT = 1,
  EXTRACT_MODE_TYPE_SYMBOLS,
};

#endif
