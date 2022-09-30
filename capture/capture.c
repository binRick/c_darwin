#pragma once
#ifndef CAPTURE_C
#define CAPTURE_C
////////////////////////////////////////////
#include "capture/capture.h"
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
static bool CAPTURE_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__capture(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_capture") != NULL) {
    log_debug("Enabling capture Debug Mode");
    CAPTURE_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
