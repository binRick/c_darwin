#pragma once
#ifndef WINDOW_C
#define WINDOW_C
////////////////////////////////////////////
#include "window/window.h"
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
static bool WINDOW_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__window(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_window") != NULL) {
    log_debug("Enabling window Debug Mode");
    WINDOW_DEBUG_MODE = true;
  }
}
#endif
