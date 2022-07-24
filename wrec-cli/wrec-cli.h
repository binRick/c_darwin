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
#define DEFAULT_VERBOSE      false
#define DEFAULT_MODE         "test"
#define DEFAULT_WINDOW_ID    0
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
};
struct recorded_frame_t {
  int64_t timestamp;
  char    *file;
};
struct capture_config_t {
  bool          active;
  int           max_duration_seconds;
  int           max_frames_qty;
  int64_t       started_timestamp, ended_timestamp;
  int           frames_per_second;
  int           window_id;
  struct Vector *recorded_frames_v;
};
///////////////////////////////////
static int parse_args(int argc, char *argv[]);
int debug_args();

///////////////////////////////////

