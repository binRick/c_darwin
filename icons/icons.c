#pragma once
#ifndef ICONS_C
#define ICONS_C
////////////////////////////////////////////
#include "icons/icons.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "incbin/incbin.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

////////////////////////////////////////////
static bool ICONS_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__icons(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_icons") != NULL) {
    log_debug("Enabling icons Debug Mode");
    ICONS_DEBUG_MODE = true;
  }
  if (ICONS_DEBUG_MODE) {
    // log_info("Loaded %lu Icons", icons_qty);
  }
}
////////////////////////////////////////////
#endif
