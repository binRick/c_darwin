#pragma once
#ifndef CORE_C
#define CORE_C
////////////////////////////////////////////
#include "core/core.h"
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
static bool CORE_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__core(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_core") != NULL) {
    log_debug("Enabling core Debug Mode");
    CORE_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
