#pragma once
////////////////////////////////////
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
#include "screen/utils/utils.h"
////////////////////////////////////

struct private_screen_capture_t;
struct private_screen_capture_handlers_t {
  int    (^get_pid)(void);
  size_t (^get_displays_qty)(void);
  void   (^set_log_level)(enum screen_capture_log_level_t);
};

module(screen_utils) {
  define(screen_utils, CLIB_MODULE);
  //////////////////////////////////////////////////////
  struct private_screen_capture_t          *priv;
  struct private_screen_capture_handlers_t *handlers;
  //////////////////////////////////////////////////////
  void                                     (^set_log_level)(enum screen_capture_log_level_t);
  size_t                                   (^get_displays_qty)(void);
  int                                      (^get_pid)(void);
  //////////////////////////////////////////////////////
};

int  screen_utils_init(module(screen_utils) * exports);
void screen_utils_deinit(module(screen_utils) * exports);

#define self    require(screen_utils)
exports(screen_utils) {
  .init             = screen_utils_init,
  .deinit           = screen_utils_deinit,
  .priv             = 0,
  .handlers         = 0,
  .get_pid          = ^ int (void){ return(self->handlers->get_pid()); },
  .get_displays_qty = ^ size_t (void){ return(self->handlers->get_displays_qty()); },
  .set_log_level    = ^ void (enum screen_capture_log_level_t l){ self->handlers->set_log_level(l); },
};
#undef self

#define ScreenUtils        module(screen_utils)
#define FreeScreenUtils    clib_module_free
