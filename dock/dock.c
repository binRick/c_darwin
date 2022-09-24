#pragma once
#ifndef DOCK_C
#define DOCK_C
////////////////////////////////////////////
#include "dock/dock.h"
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
static bool DOCK_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__dock(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_dock") != NULL) {
    log_debug("Enabling dock Debug Mode");
    DOCK_DEBUG_MODE = true;
  }
}
static void dock_internal_fxn1(void);

////////////////////////////////////////////
static void dock_internal_fxn1(void){
  log_info("Called function dock_internal_fxn1");
  if (DOCK_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}

////////////////////////////////////////////
void dock_fxn1(void){
  dock_internal_fxn1();
  log_info("Called function dock_fxn1");
  if (DOCK_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}
////////////////////////////////////////////
#endif
