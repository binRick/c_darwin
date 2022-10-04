#pragma once
#ifndef CORE_DEBUG_C
#define CORE_DEBUG_C
////////////////////////////////////////////
#include "core/debug/debug.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#define LOCAL_DEBUG_MODE    CORE_DEBUG_DEBUG_MODE
#define debug               log_debug
////////////////////////////////////////////
static bool CORE_DEBUG_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__core_debug(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_core_debug") != NULL) {
    log_debug("Enabling core_debug Debug Mode");
    CORE_DEBUG_DEBUG_MODE = true;
  }
  log_debug("test123");

  LOCAL_DEBUG_MODE = true;
  log_debug("1");
  debug("1");
  LOCAL_DEBUG_MODE = false;
  log_debug("2");
  debug("2");

  log_dbg("abc", % s);
  int abc = 213;
  log_dbg(abc, % d);

  Debug("abc123 %d", abc);
  Info("abc123 %d", abc);
  Warn("abc123 %d", abc);
  Error("abc123 %d", abc);
  Dbg(abc, % d);
}
////////////////////////////////////////////

#undef LOCAL_DEBUG_MODE
#endif
