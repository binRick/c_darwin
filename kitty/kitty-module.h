#pragma once
////////////////////////////////////
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
////////////////////////////////////
#include "kitty/kitty.h"
////////////////////////////////////

enum kitty_utils_mode {
  KITTY_LOG_NONE,
  KITTY_LOG_INFO,
  KITTY_LOG_ERROR,
  KITTY_LOG_DEBUG,
  KITTY_LOG_QTY,
};
#define KITTY_LOG_DEFAULT    KITTY_LOG_INFO

module(kitty_utils) {
  define(kitty_utils, CLIB_MODULE);
  ///////////////////////////////////////////////////
  enum kitty_utils_mode mode;
  ///////////////////////////////////////////////////
  struct Vector         *pids_v;         size_t pids_qty;
  ///////////////////////////////////////////////////
  struct Vector         *listen_ons_v;   size_t listen_ons_qty;
  ///////////////////////////////////////////////////
};


int  kitty_utils_module_init(module(kitty_utils) * exports);
void kitty_utils_module_deinit(module(kitty_utils) * exports);

exports(kitty_utils) {
  .init   = kitty_utils_module_init,
  .deinit = kitty_utils_module_deinit,
  ///////////////////////////////////////////////////
  .mode = KITTY_LOG_DEFAULT,
  ///////////////////////////////////////////////////
  .pids_v = NULL, .pids_qty = 0,
  ///////////////////////////////////////////////////
  .listen_ons_v = NULL, .listen_ons_qty = 0,
  ///////////////////////////////////////////////////
};
