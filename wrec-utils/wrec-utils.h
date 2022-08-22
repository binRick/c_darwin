#pragma once
#include <stdio.h>
#define DEBUG_PID_ENV    false
#include <ctype.h>
#include <libproc.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/sysctl.h>
#include <sys/time.h>
////////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_vector/include/vector.h"
#include "djbhash/src/djbhash.h"
#include "fsio.h"
#include "parson.h"
#include "str-replace.h"
#include "string-utils/string-utils.h"
#include "stringbuffer.h"
#include "stringfn.h"
#include "submodules/libfort/src/fort.h"
#include "tiny-regex-c/re.h"
#include "window-utils/window-utils.h"
//////////////////////////////////////////
enum resize_type_t {
  RESIZE_BY_WIDTH,
  RESIZE_BY_HEIGHT,
  RESIZE_BY_FACTOR,
  RESIZE_BY_NONE,
  RESIZE_TYPES_QTY
} resize_type_t;
enum capture_mode_type_t {
  CAPTURE_REQUEST_MODE_WRITE_FILE_RETURN_RESULT,
  CAPTURE_REQUEST_MODE_WRITE_FILE_CALLBACK_RESULT,
  CAPTURE_REQUEST_MODE_RETURN_RESULT,
  CAPTURE_REQUEST_MODE_CALLBACK_RESULT,
  CAPTURE_REQUEST_MODES_QTY,
};

struct capture_result_data_t {
  enum capture_mode_type_t capture_mode;
  size_t                   captured_width;
  size_t                   captured_height;
  size_t                   resized_width;
  size_t                   resized_height;
  int                      colorspace, bytes_per_row, bits_per_pixel;
  uint8_t                  *pixels;
  char                     *file_path;
  size_t                   file_size;
  CGImageRef               captured_image_ref;
  CGImageRef               resized_image_ref;
};

struct capture_request_t {
  enum capture_mode_type_t mode;
  enum resize_type_t       resize_type;
  int                      window_id, resize_factor;
  char                     *file_path;
  bool                     debug_mode;
  void                     *(*callback_function)(struct capture_result_data_t *);
  size_t                   width, height;
};

struct capture_result_t {
  int                          captured_width, captured_height, resized_width, resized_height;
  unsigned long                request_received, capture_started, resize_started, write_file_started;
  unsigned long                capture_dur, write_file_dur, resize_dur, total_dur;
  bool                         save_file_success, success;
  CGSize                       window_size;
  CGRect                       window_bounds;
  CGRect                       captured_rect;
  struct capture_request_t     *request;
  struct capture_result_data_t *data;
};
//////////////////////////////////////////
int list_windows();
int capture_window();
//////////////////////////////////////////
