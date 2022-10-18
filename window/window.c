#pragma once
#ifndef WINDOW_C
#define WINDOW_C
#include "window/window.h"
static bool WINDOW_DEBUG_MODE = false;

MODULE_FUNCTIONS(MODULE_FUNCTION)
#undef MODULE_FUNCTION

int window_module_init(module(window) *exports) {
  clib_module_init(window, exports);
  exports->mode = WINDOW_NONE;
  return(0);
}

void window_module_deinit(module(window) *exports) {
  clib_module_deinit(window);
}

static void __attribute__((constructor)) __constructor__window(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_window") != NULL) {
    log_debug("Enabling window Debug Mode");
    WINDOW_DEBUG_MODE = true;
  }
}
#endif
#undef MODULE_FUNCTIONS
