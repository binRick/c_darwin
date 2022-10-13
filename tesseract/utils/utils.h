#pragma once
#ifndef TESSERACT_UTILS_H
#define TESSERACT_UTILS_H
//////////////////////////////////////
#include "c_vector/vector/vector.h"
#include "capture/utils/utils.h"
#include "tesseract/capi.h"
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
//////////////////////////////////////
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
  enum image_type_id_t                        format;
  unsigned char                                 *img;
  size_t                                        img_len;
};
enum extract_mode_type_t {
  EXTRACT_MODE_TYPE_TEXT = 1,
  EXTRACT_MODE_TYPE_SYMBOLS,
};
struct Vector *tesseract_extract_items(struct Vector *ids_v, size_t concurrency);
struct tesseract_extract_result_t *tesseract_find_item_matching_word_locations(size_t id, struct Vector *words);
struct Vector *tesseract_extract_text(size_t id);
void tesseract_extract_symbols(size_t id);
struct Vector *tesseract_extract_memory(unsigned char *img_data, size_t img_data_len, unsigned long MODE);
struct Vector *get_security_words_v();
bool parse_tesseract_extraction_results(struct tesseract_extract_result_t *r);
void report_tesseract_extraction_results(struct tesseract_extract_result_t *r);
bool tesseract_security_preferences_logic(int space_id);
#endif
