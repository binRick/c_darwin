#pragma once
#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/**********************************/
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "core-utils/core-utils.h"
#include "keylogger/keylogger.h"
#include "log/log.h"
#include "timestamp/timestamp.h"
/**********************************/
static int setup_event_tap();
static int setup_event_tap_with_callback(void ( *cb )(char *key));
static CGEventRef event_handler(__attribute__((unused)) CGEventTapProxy proxy, __attribute__((unused)) CGEventType type, CGEventRef event, void *CALLBACK);
const char *keylogger_convertKeyboardCode(int);
static CGEventMask kb_events = (
  CGEventMaskBit(kCGEventKeyDown)
  | CGEventMaskBit(kCGEventFlagsChanged)
  );

static int tap_events(){
  CFRunLoopRun();
  return(EXIT_SUCCESS);
}

static int setup_event_tap_with_callback(void ( *cb )(char *key)){
  CFMachPortRef event_tap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0, kb_events, event_handler, (void *)cb);

  if (!event_tap) {
    log_error("ERROR: Unable to create keyboard event tap.");
    return(EXIT_FAILURE);
  }
  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, event_tap, 0);

  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
  CGEventTapEnable(event_tap, true);
  return(EXIT_SUCCESS);
}

static int setup_event_tap(){
  CFMachPortRef event_tap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0, kb_events, event_handler, NULL);

  if (!event_tap) {
    log_error("ERROR: Unable to create keyboard event tap.");
    return(EXIT_FAILURE);
  }
  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, event_tap, 0);

  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
  CGEventTapEnable(event_tap, true);
  return(EXIT_SUCCESS);
}

static CGEventRef event_handler(__attribute__((unused)) CGEventTapProxy proxy, __attribute__((unused)) CGEventType type, CGEventRef event, void *CALLBACK) {
  unsigned long event_flags = (int)CGEventGetFlags(event);

  if (event_flags > 0) {
    CGKeyCode keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    if (keyCode > 0) {
      char *ckc = keylogger_convertKeyboardCode(keyCode);
      if (strlen(ckc) > 0) {
        struct StringBuffer *event_flag_sb = stringbuffer_new();
        if (event_flags & kCGEventFlagMaskAlternate) {
          stringbuffer_append_string(event_flag_sb, "alt+");
        }
        if (event_flags & kCGEventFlagMaskSecondaryFn) {
          stringbuffer_append_string(event_flag_sb, "secondaryfxn+");
        }
        if (event_flags & kCGEventFlagMaskCommand) {
          stringbuffer_append_string(event_flag_sb, "cmd+");
        }
        if (event_flags & kCGEventFlagMaskControl) {
          stringbuffer_append_string(event_flag_sb, "ctrl+");
        }
        if (event_flags & kCGEventFlagMaskShift) {
          stringbuffer_append_string(event_flag_sb, "shift+");
        }
        stringbuffer_append_string(event_flag_sb, ckc);

        char *event_flag = stringbuffer_to_string(event_flag_sb);
        stringbuffer_release(event_flag_sb);

        if (stringfn_ends_with(event_flag, "+")) {
          stringfn_mut_substring(event_flag, 0, strlen(event_flag) - 1);
        }
        stringfn_mut_trim(event_flag);
        if (CALLBACK != NULL) {
          if (((int (*)(char *)) CALLBACK)(event_flag) == EXIT_SUCCESS) {
            return(NULL);
          }
        }
      }
    }
  }
  return(event);
} /* CGEventCallback */

const char *keylogger_convertKeyboardCode(int keyCode) {
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
  log_error("UNKNOWN KEY: %d", keyCode);
  return("[unknown]");
} /* convertKeyCode */

/**********************************/
int keylogger_exec_with_callback(void ( *cb )(char *)) {
  assert(is_authorized_for_accessibility() == true);
  assert(setup_event_tap_with_callback(cb) == 0);
  log_info("Keylogger Event Tap Setup");
  assert(tap_events() == 0);
  log_info("Keylogger Events Tapped");
  return(EXIT_SUCCESS);
}

int keylogger_exec() {
  assert(is_authorized_for_accessibility() == true);
  assert(setup_event_tap() == 0);
  log_info("Keylogger Event Tap Setup");
  assert(tap_events() == 0);
  log_info("Keylogger Events Tapped");
  return(EXIT_SUCCESS);
}
/**********************************/
