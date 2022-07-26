#pragma once
#include "cargs/include/cargs.h"
#include "wrec/wrec.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
///////////////////////////////////
enum resize_type_t {
  RESIZE_BY_WIDTH,
  RESIZE_BY_HEIGHT,
  RESIZE_BY_FACTOR,
  RESIZE_NONE,
} resize_type_t;


static char *resize_type_name(const int RESIZE_TYPE){
  char *RESIZE_TYPE_NAME = "UNKNOWN";

  switch (RESIZE_TYPE) {
  case RESIZE_BY_WIDTH:  RESIZE_TYPE_NAME  = "WIDTH"; break;
  case RESIZE_BY_HEIGHT:  RESIZE_TYPE_NAME = "HEIGHT"; break;
  case RESIZE_BY_FACTOR:  RESIZE_TYPE_NAME = "FACTOR"; break;
  }
  return(RESIZE_TYPE_NAME);
}
///////////////////////////////////
#define DEFAULT_VERBOSE                        false
#define DEFAULT_MODE                           "test"
#define DEFAULT_WINDOW_ID                      0
#define DEFAULT_MAX_RECORD_FRAMES              100
#define DEFAULT_MAX_RECORD_DURATION_SECONDS    30
#define DEFAULT_FRAMES_PER_SECOND              3
#define DEFAULT_MODE_LIST                      false
#define DEFAULT_MODE_CAPTURE                   false
#define DEFAULT_MODE_DEBUG_ARGS                false
#define DEFAULT_RESIZE_TYPE                    RESIZE_NONE
#define DEFAULT_RESIZE_VALUE                   1
///////////////////////////////////
struct modes_t {
  char *name;
  char *description;
  int  (*handler)(void *);
} modes_t;
struct args_t {
  char   *mode;
  bool   verbose;
  int    window_id;
  size_t max_recorded_frames;
  size_t max_record_duration_seconds;
  int    frames_per_second;
  bool   mode_capture;
  bool   mode_list;
  bool   mode_debug_args;
  int    resize_type;
  int    resize_value;
};
struct recorded_frame_t {
  int64_t timestamp;
  char    *file, *hash_enc;
  size_t  file_size;
};
struct capture_config_t {
  bool          active;
  int           max_record_frames_qty, max_record_duration_seconds, frames_per_second, window_id;
  int64_t       started_timestamp, ended_timestamp;
  int           resize_type;
  int           resize_value;
  struct Vector *recorded_frames_v;
};
///////////////////////////////////
static int parse_args(int argc, char *argv[]);
int debug_args();

///////////////////////////////////

