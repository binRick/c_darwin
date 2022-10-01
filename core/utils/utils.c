#pragma once
#ifndef CORE_UTILS_C
#define CORE_UTILS_C
////////////////////////////////////////////
#include "core/utils/utils.h"
////////////////////////////////////////////

////////////////////////////////////////////
static bool CORE_UTILS_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__core_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_core_utils") != NULL) {
    log_debug("Enabling core_utils Debug Mode");
    CORE_UTILS_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
