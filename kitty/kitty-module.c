#pragma once
//////////////////////////////////////
#include <stdio.h>
//////////////////////////////////////
#include "kitty/kitty-module.h"
//////////////////////////////////////
#define KITTY_MODULE_DEBUG_MODE    true


//////////////////////////////////////


int kitty_utils_module_init(module(kitty_utils) *exports) {
  if (exports->mode >= KITTY_LOG_DEBUG) {
    fprintf(stderr, "<%d> [%s] [mode:%d] <module init>\n",
            getpid(),
            __FUNCTION__,
            require(kitty_utils)->mode
            );
  }
  clib_module_init(kitty_utils, exports);
  exports->mode = ((KITTY_MODULE_DEBUG_MODE == true) || getenv("DEBUG_MODE") != NULL) ? KITTY_LOG_DEBUG : KITTY_LOG_DEFAULT;

  exports->pids_v         = get_kitty_pids();
  exports->pids_qty       = vector_size(exports->pids_v);
  exports->listen_ons_v   = get_kitty_listen_ons();
  exports->listen_ons_qty = vector_size(exports->listen_ons_v);


  return(0);
}


//////////////////////////////////////
void kitty_utils_module_deinit(module(kitty_utils) *exports) {
  if (KITTY_MODULE_DEBUG_MODE == true) {
    fprintf(stderr, "<%d> [%s] [mode:%d] <module cleanup>\n",
            getpid(),
            __FUNCTION__,
            require(kitty_utils)->mode
            );
  }

  clib_module_deinit(kitty_utils);
}
