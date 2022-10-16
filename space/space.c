#pragma once
#ifndef SPACE_C
#define SPACE_C
////////////////////////////////////////////
#include "space/space.h"
////////////////////////////////////////////
static bool SPACE_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__space(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_space") != NULL) {
    log_debug("Enabling space Debug Mode");
    SPACE_DEBUG_MODE = true;
  }
}
#endif
