#pragma once
//////////////////////////////////////
#include "c_eventemitter/include/eventemitter.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "osx-keys/osx-keys.h"
#include <assert.h>
#include <ctype.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CoreFoundation.h>
#include <err.h>
#include <errno.h>
#include <inttypes.h>
#include <libgen.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <poll.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
//////////////////////////////////////
enum focused_events_t {
  EVENT_START  = 100,
  EVENT_MIDDLE = 200,
  EVENT_END    = 300,
  EVENT_SPACE_ID_CHANGED = 400,
};
struct focused_config_t {
  unsigned long       started;
  bool                enabled, focus_all_space_ids;
  size_t              focused_window_width;
  size_t              cur_space_id;
  struct Vector       *focused_space_ids, *focused_window_ids;
  struct EventEmitter *ee;
  CFMachPortRef       event_tap;
  pthread_t           loop_thread;
};
bool add_focused_window_id(struct focused_config_t *cfg, size_t WINDOW_ID);
struct focused_config_t *init_focused_config(void);
bool start_focused(struct focused_config_t *cfg);
bool stop_focused(struct focused_config_t *cfg);