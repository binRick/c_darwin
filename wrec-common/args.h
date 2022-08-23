#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
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
  char   *application_name_glob;
};
