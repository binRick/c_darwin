#pragma once
#ifndef DEFAULT_LOGLEVEL
#define DEFAULT_LOGLEVEL    1
#endif
#include <Carbon/Carbon.h>
#include <fnmatch.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
///////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app-utils/app-utils.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
///////////////////////////////////////////////////
#define MAX_DISPLAYS            8
#define BITS_PER_COMPONENT      8
#define BITS_PER_PIXEL          32
#define RGB_BYTES_PER_ROW       4
#define SHOULD_INTERPOLATE      false
#define DECODE_ARRAY            NULL
#define SAVE_IMAGE_TYPE         kUTTypePNG
#define SAVE_IMAGE_EXTENSION    "png"
///////////////////////////////////////////////////
enum screen_capture_file_type_t {
  SCREEN_CAPTURE_FILE_TYPE_PNG = 1,
  SCREEN_CAPTURE_FILE_TYPE_BMP,
  SCREEN_CAPTURE_FILE_TYPE_JPG,
  SCREEN_CAPTURE_FILES_QTY,
};
enum screen_capture_log_level_t {
  LOG_LEVEL_DEBUG = 1,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVELS_QTY,
};

struct display_t {
  size_t                id;
  unsigned long         capture_started_ms, capture_dur_ms;
  size_t                buffer_size;
  int8_t                *buffer;
  size_t                bits_per_pixel;
  const char            *capture_file_name;
  bool                  captured_success;
  CGRect                rect;
  CGImageRef            image_ref;
  CGDataProviderRef     provider_ref;
  CFDataRef             data_ref;
  CGColorSpaceRef       color_space_ref;
  CFURLRef              url_ref;
  CGImageDestinationRef destination_ref;
};

struct screen_capture_t {
  bool                            success;
  unsigned long                   started_ms, dur_ms;
  const char                      *file;
  CGDirectDisplayID               *display_ids;
  struct Vector                   *displays_v;
  size_t                          displays_qty;
  enum screen_capture_log_level_t log_level;
  enum screen_capture_file_type_t file_type;
};

struct display_t *init_display(size_t DISPLAY_ID);
struct screen_capture_t *init_screen_capture();
/////////////////////////////////////////////////////////
struct screen_capture_t *screen_capture();
bool save_captures(struct screen_capture_t *S);
