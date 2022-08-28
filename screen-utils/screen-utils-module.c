#pragma once
////////////////////////////////////
#include "screen-utils/screen-utils-module.h"
#define self    module(screen_utils)

struct private_screen_capture_t {
  int                             pid;
  enum screen_capture_log_level_t log_level;
  struct screen_capture_t         *screen_capture;
};

struct private_screen_capture_t          *priv     = &(struct private_screen_capture_t){ 0 };
struct private_screen_capture_handlers_t *handlers = &(struct private_screen_capture_handlers_t){
  .get_pid          = ^ int (void){ return(priv->pid); },
  .get_displays_qty = ^ size_t (void){ return(vector_size(priv->screen_capture->displays_v)); },
  .set_log_level    = ^ void (enum screen_capture_log_level_t l){ priv->log_level = l; },
};

int screen_utils_init(module(screen_utils) *exports) {
  clib_module_init(screen_utils, exports);
  exports->handlers             = handlers;
  exports->priv                 = priv;
  exports->priv->pid            = getpid();
  exports->priv->screen_capture = init_screen_capture(),

  fprintf(stderr, "<%d> <module init> [%lu displays] \n",
          exports->get_pid(),
          exports->get_displays_qty()
          );

  return(0);
}

//////////////////////////////////////
void screen_utils_deinit(module(screen_utils) *exports) {
  fprintf(stderr, "<%d> [%d] <module cleanup>\n",
          exports->get_pid(),
          exports->priv->log_level
          );
  clib_module_deinit(screen_utils);
}

#undef self
