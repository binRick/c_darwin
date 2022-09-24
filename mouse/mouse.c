#pragma once
#ifndef MOUSE_C
#define MOUSE_C
////////////////////////////////////////////
#include "mouse/mouse.h"
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
static bool MOUSE_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__mouse(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_mouse") != NULL) {
    log_debug("Enabling mouse Debug Mode");
    MOUSE_DEBUG_MODE = true;
  }
}
static void mouse_internal_fxn1(void);

////////////////////////////////////////////
static void mouse_internal_fxn1(void){
  log_info("Called function mouse_internal_fxn1");
  if (MOUSE_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}

////////////////////////////////////////////
void mouse_fxn1(void){
  mouse_internal_fxn1();
  log_info("Called function mouse_fxn1");
  if (MOUSE_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}
////////////////////////////////////////////
#endif
