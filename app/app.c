#pragma once
#ifndef APP_C
#define APP_C
#include "ansi-codes/ansi-codes.h"
#include "app/app.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
////////////////////////////////////////////
static bool      APP_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__app(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_APP") != NULL) {
    log_debug("Enabling app Debug Mode");
    APP_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
