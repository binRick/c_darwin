#pragma once
#ifndef SPACE_C
#define SPACE_C
////////////////////////////////////////////
#include "space/space.h"
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
static bool SPACE_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__space(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_space") != NULL) {
    log_debug("Enabling space Debug Mode");
    SPACE_DEBUG_MODE = true;
  }
}
static void space_internal_fxn1(void);

////////////////////////////////////////////
static void space_internal_fxn1(void){
  log_info("Called function space_internal_fxn1");
  if (SPACE_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}

////////////////////////////////////////////
void space_fxn1(void){
  space_internal_fxn1();
  log_info("Called function space_fxn1");
  if (SPACE_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}
////////////////////////////////////////////
#endif
