#pragma once
#ifndef TERMINAL_UTILS_C
#define TERMINAL_UTILS_C
////////////////////////////////////////////

////////////////////////////////////////////
#include "terminal/utils/utils.h"
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
static bool terminal_utils_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__terminal_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_terminal_utils") != NULL) {
    log_debug("Enabling terminal-utils Debug Mode");
    terminal_utils_DEBUG_MODE = true;
  }
}
static void terminal_utils_internal_fxn1(void);

////////////////////////////////////////////
static void terminal_utils_internal_fxn1(void){
  log_info("Called function terminal_utils_internal_fxn1");
  if (terminal_utils_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}

////////////////////////////////////////////
void terminal_utils_fxn1(void){
  terminal_utils_internal_fxn1();
  log_info("Called function terminal_utils_fxn1");
  if (terminal_utils_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}
////////////////////////////////////////////
#endif
