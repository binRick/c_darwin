#pragma once
#ifndef DISPLAY_C
#define DISPLAY_C
#include "display/display.h"
static bool DISPLAY_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__display(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_DISPLAY") != NULL) {
    log_debug("Enabling display Debug Mode");
    DISPLAY_DEBUG_MODE = true;
  }
}
#endif
