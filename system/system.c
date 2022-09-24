#pragma once
#ifndef SYSTEM_C
#define SYSTEM_C
////////////////////////////////////////////
#include "system/system.h"
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
static bool SYSTEM_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__system(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_system") != NULL) {
    log_debug("Enabling system Debug Mode");
    SYSTEM_DEBUG_MODE = true;
  }
}
#endif
