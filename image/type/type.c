#pragma once
#ifndef IMAGE_TYPE_C
#define IMAGE_TYPE_C
#define LOCAL_DEBUG_MODE    IMAGE_TYPE_DEBUG_MODE
////////////////////////////////////////////
#include "image/type/type.h"
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
#include "vips/vips.h"

////////////////////////////////////////////
static bool IMAGE_TYPE_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__type(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_type") != NULL) {
    log_debug("Enabling image type Debug Mode");
    IMAGE_TYPE_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#endif
