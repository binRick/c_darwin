#pragma once
#ifndef DISPLAY_C
#define DISPLAY_C
////////////////////////////////////////////
#include "display/display.h"
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
static bool DISPLAY_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__display(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_display") != NULL) {
    log_debug("Enabling display Debug Mode");
    DISPLAY_DEBUG_MODE = true;
  }
}
static void display_internal_fxn1(void);

////////////////////////////////////////////
static void display_internal_fxn1(void){
  log_info("Called function display_internal_fxn1");
  if (DISPLAY_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}

////////////////////////////////////////////
void display_fxn1(void){
  display_internal_fxn1();
  log_info("Called function display_fxn1");
  if (DISPLAY_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}
////////////////////////////////////////////
#endif
