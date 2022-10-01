#pragma once
#ifndef CORE_IMAGE_C
#define CORE_IMAGE_C
////////////////////////////////////////////
#include "core/core.h"
#include "core/image/image.h"
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
static bool CORE_IMAGE_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__core_image(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_core_image") != NULL) {
    log_debug("Enabling core_image Debug Mode");
    CORE_IMAGE_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
