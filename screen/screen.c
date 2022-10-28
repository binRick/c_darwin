#pragma once
#ifndef SCREEN_C
#define SCREEN_C
////////////////////////////////////////////
#include "screen/screen.h"
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
static bool SCREEN_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__screen(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_screen") != NULL) {
    log_debug("Enabling screen Debug Mode");
    SCREEN_DEBUG_MODE = true;
  }
}
static void screen_internal_fxn1(void);

////////////////////////////////////////////
static void screen_internal_fxn1(void){
  log_info("Called function screen_internal_fxn1");
  if (SCREEN_DEBUG_MODE == true)
    log_info("Debug Mode Enabled");
}

////////////////////////////////////////////
void screen_fxn1(void){
  screen_internal_fxn1();
  log_info("Called function screen_fxn1");
  if (SCREEN_DEBUG_MODE == true)
    log_info("Debug Mode Enabled");
}
////////////////////////////////////////////
#endif
