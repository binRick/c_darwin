#pragma once
#ifndef KEYBOARD_UTILS_C
#define KEYBOARD_UTILS_C
////////////////////////////////////////////

////////////////////////////////////////////
#include "keyboard/utils/utils.h"
#include "osx-keys/osx-keys.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#define TYPOMATIC_RATE    100000
////////////////////////////////////////////
static bool keyboard_utils_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////

void keyHit(CGKeyCode kc, CGEventFlags flags) {
  keyPress(kc, flags);
  usleep(TYPOMATIC_RATE);
  keyRelease(kc, flags);
}

void keyPress(CGKeyCode kc, CGEventFlags flags) {
  toKey(kc, flags, true);
}

void keyRelease(CGKeyCode kc, CGEventFlags flags) {
  toKey(kc, flags, false);
}

static void __attribute__((constructor)) __constructor__keyboard_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_keyboard_utils") != NULL) {
    log_debug("Enabling keyboard-utils Debug Mode");
    keyboard_utils_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
