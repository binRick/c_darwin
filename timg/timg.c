#pragma once
#ifndef TIMG_C
#define TIMG_C
////////////////////////////////////////////
#include "timg/timg.h"
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
static bool TIMG_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__timg(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_timg") != NULL) {
    log_debug("Enabling timg Debug Mode");
    TIMG_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
