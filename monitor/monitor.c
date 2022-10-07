#pragma once
#ifndef MONITOR_C
#define MONITOR_C
////////////////////////////////////////////
#include "monitor/monitor.h"
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
static bool MONITOR_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__monitor(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_monitor") != NULL) {
    log_debug("Enabling monitor Debug Mode");
    MONITOR_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
