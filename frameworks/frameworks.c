#pragma once
#ifndef FRAMEWORKS_C
#define FRAMEWORKS_C
////////////////////////////////////////////

////////////////////////////////////////////
#include "frameworks/frameworks.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

////////////////////////////////////////////
static bool FRAMEWORKS_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__frameworks(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_FRAMEWORKS") != NULL) {
    log_debug("Enabling frameworks Debug Mode");
    FRAMEWORKS_DEBUG_MODE = true;
  }
}
static void frameworks_internal_fxn1(void);

////////////////////////////////////////////
static void frameworks_internal_fxn1(void){
  log_info("Called function frameworks_internal_fxn1");
  if (FRAMEWORKS_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}

////////////////////////////////////////////
void frameworks_fxn1(void){
  frameworks_internal_fxn1();
  log_info("Called function frameworks_fxn1");
  if (FRAMEWORKS_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}
////////////////////////////////////////////
#endif
