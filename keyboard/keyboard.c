#pragma once
#ifndef KEYBOARD_C
#define KEYBOARD_C
////////////////////////////////////////////
#include "keyboard/keyboard.h"
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

////////////////////////////////////////////
static bool KEYBOARD_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__keyboard(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_keyboard") != NULL) {
    log_debug("Enabling keyboard Debug Mode");
    KEYBOARD_DEBUG_MODE = true;
  }
}
static void keyboard_internal_fxn1(void);

////////////////////////////////////////////
static void keyboard_internal_fxn1(void){
  log_info("Called function keyboard_internal_fxn1");
  if (KEYBOARD_DEBUG_MODE == true)
    log_info("Debug Mode Enabled");
}

////////////////////////////////////////////
void keyboard_fxn1(void){
  keyboard_internal_fxn1();
  log_info("Called function keyboard_fxn1");
  if (KEYBOARD_DEBUG_MODE == true)
    log_info("Debug Mode Enabled");
}
////////////////////////////////////////////
#endif
