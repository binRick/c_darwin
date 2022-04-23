//////////////////////////////////////
#ifndef LOG_LOADED
#define LOG_LOADED    1
//////////////////////////////////////
#include "../include/includes.h"
//////////////////////////////////////
#ifndef SKIP_LIST_C
//#include "../list/src/list.c"
//#include "../list/src/list_iterator.c"
#endif
//#include "../human/bytes.c"
//#include "../libbool/src/bool.c"
//#include "../time/timestamp.c"
//#include "../time/timequick.h"
//#include "../dict.c/src/dict.c"
//////////////////////////////////////
#define MAX_CALLBACKS           32
//////////////////////////////////////
#ifndef LOG_STDERR
#define LOG_STDERR              stderr
#endif
#ifndef LOG_STDOUT
#define LOG_STDOUT              stdout
#endif
#ifndef DEFAULT_LOG_LOG_PATH
#define DEFAULT_LOG_LOG_PATH    stderr
#endif
#ifndef LOG_PATH_ENABLED
#define LOG_PATH_ENABLED        true
#endif
#ifndef LOG_TIME_ENABLED
#define LOG_TIME_ENABLED        true
#endif
//////////////////////////////////////
char *LOG_LOG_PATH;
char *DEFAULT_STDOUT;
char *LOG_OUT;
//////////////////////////////////////
typedef struct {
  log_LogFn fn;
  void      *udata;
  int       level;
} Callback;
static struct {
  void       *udata;
  log_LockFn lock;
  int        level;
  bool       quiet;
  bool       return_str;
  Callback   callbacks[MAX_CALLBACKS];
} L;


static char * char_callback(log_Event *ev) {
  char buf[16];
  char buf1[1024];

  buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
  fprintf(
    &buf1, "%s %s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
    level_icons[ev->level], GET_LOG_TIME_STRING(buf), level_colors[ev->level], level_strings[ev->level],
    GET_LOG_PATH_STRING(ev->file), ev->line
    );
  return(strdup(&buf1));
}


static void stdout_callback(log_Event *ev) {
  char buf[16];

  buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
  fprintf(
    ev->udata, LOG_FMT,
    level_icons[ev->level], GET_LOG_TIME_STRING(buf), level_colors[ev->level], level_strings[ev->level],
    GET_LOG_PATH_STRING(ev->file), ev->line
    );
  vfprintf(ev->udata, ev->fmt, ev->ap);
  fprintf(ev->udata, "\n");
  fflush(ev->udata);
}


static void file_callback(log_Event *ev) {
  char buf[64];

  buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
  fprintf(
    ev->udata, "%s %-5s %s:%d: ",
    buf, level_strings[ev->level], ev->file, ev->line);
  vfprintf(ev->udata, ev->fmt, ev->ap);
  fprintf(ev->udata, "\n");
  fflush(ev->udata);
}


static void lock(void) {
  if (L.lock) {
    L.lock(true, L.udata);
  }
}


static void unlock(void) {
  if (L.lock) {
    L.lock(false, L.udata);
  }
}


const char * log_level_string(int level) {
  return(level_strings[level]);
}


void log_set_lock(log_LockFn fn, void *udata) {
  L.lock  = fn;
  L.udata = udata;
}


const char * log_get_level_string(){
  return(log_level_string(log_get_level()));
}


int log_get_level(){
  return(L.level);
}


void log_set_level(int level) {
  L.level = level;
}


void log_set_return_str(bool enable) {
  L.return_str = enable;
}


void log_set_quiet(bool enable) {
  L.quiet = enable;
}


int log_add_callback(log_LogFn fn, void *udata, int level) {
  for (int i = 0; i < MAX_CALLBACKS; i++) {
    if (!L.callbacks[i].fn) {
      L.callbacks[i] = (Callback) { fn, udata, level };
      return(0);
    }
  }
  return(-1);
}


int log_add_fp(FILE *fp, int level) {
  return(log_add_callback(file_callback, fp, level));
}


static void init_event(log_Event *ev, void *udata) {
  if (!ev->time) {
    time_t t = time(NULL);
    ev->time = localtime(&t);
  }
  ev->udata = udata;
}


char * str_log(int level, const char *fmt, ...) {
  log_Event ev = {
    .fmt   = fmt,
    .file  = "",
    .line  = 0,
    .level = level,
  };

  lock();
  init_event(&ev, stderr);
  va_start(ev.ap, fmt);
  char *r = char_callback(&ev);

  va_end(ev.ap);
  unlock();
  return(strdup(r));
}


void log_log(int level, const char *file, int line, const char *fmt, ...) {
  log_Event ev = {
    .fmt   = fmt,
    .file  = file,
    .line  = line,
    .level = level,
  };

  lock();

  if (!L.quiet && level >= L.level) {
    init_event(&ev, stderr);
    va_start(ev.ap, fmt);
    stdout_callback(&ev);
    va_end(ev.ap);
  }

  for (int i = 0; i < MAX_CALLBACKS && L.callbacks[i].fn; i++) {
    Callback *cb = &L.callbacks[i];
    if (level >= cb->level) {
      init_event(&ev, cb->udata);
      va_start(ev.ap, fmt);
      cb->fn(&ev);
      va_end(ev.ap);
    }
  }

  unlock();
}
#endif
