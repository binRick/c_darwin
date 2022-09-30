#pragma once
#ifndef WINDOW_INFO_H
#define WINDOW_INFO_H
//////////////////////////////////////
#include "c_vector/vector/vector.h"
#include "display/utils/utils.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <unistd.h>
//////////////////////////////////////
enum window_info_flag_t {
  WINDOW_FLAG_SHADOW     = 1 << 0,
  WINDOW_FLAG_FULLSCREEN = 1 << 1,
  WINDOW_FLAG_MINIMIZE   = 1 << 2,
  WINDOW_FLAG_FLOAT      = 1 << 3,
  WINDOW_FLAG_STICKY     = 1 << 4,
  WINDOW_FLAGS_QTY,
};
enum window_info_dur_type_t {
  WINDOW_INFO_DUR_TYPE_SPACE_ID,
  WINDOW_INFO_DUR_TYPE_IS_MINIMIZED,
  WINDOW_INFO_DUR_TYPE_TOTAL,
  WINDOW_INFO_DUR_TYPES_QTY,
};
struct window_info_dur_t {
  unsigned long started;
  unsigned long dur;
};
static const char *window_info_dur_type_names[] = {
  [WINDOW_INFO_DUR_TYPE_SPACE_ID]     = "space_id",
  [WINDOW_INFO_DUR_TYPE_IS_MINIMIZED] = "minimized",
  [WINDOW_INFO_DUR_TYPE_TOTAL]        = "total",
};
struct window_info_t {
  const char               *name, *title;
  size_t                   window_id, memory_usage, display_id, space_id;
  unsigned long            started, dur;
  int                      layer, sharing_state, store_type, connection_id;
  bool                     is_onscreen, is_focused, is_minimized, is_visible, can_minimize, is_fullscreen;
  pid_t                    pid;
  CGRect                   rect;
  AXUIElementRef           *app;
  CFDictionaryRef          window;
  ProcessSerialNumber      *psn;
  CFArrayRef               pid_app_list;
  size_t                   pid_app_list_qty;
  struct kinfo_proc        pid_info;
  AXUIElementRef app_window;
  struct Vector            *space_ids_v, *child_pids_v, *window_ids_above, *window_ids_below, *app_window_ids_v;
  struct window_info_dur_t durs[32];
};
#endif
