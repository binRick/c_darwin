#pragma once
#ifndef EVENT_C
#define EVENT_C
////////////////////////////////////////////
#include "event/event.h"
#include "core/utils/utils.h"

////////////////////////////////////////////
static bool EVENT_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__event(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_event") != NULL) {
    log_debug("Enabling event Debug Mode");
    EVENT_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
