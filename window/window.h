#pragma once
#ifndef WINDOW_H
#define WINDOW_H
#include "qoir/src/qoir.h"
char *select_window_name(void);
int get_window_id_from_name(char *NAME);
//////////////////////////////////////
#define MODULE_VAR_FUNCTIONS(FXN)                       \
  FXN(window, id_from_name, get_window_id_from_name, int, char *)\
  FXN(window, qoirs, qoirs_from_window_ids_v, struct Vector *, struct Vector *)\
//////////////////////////////////////
#define MODULE_FUNCTIONS(FXN)                           \
  FXN(window, ids, get_window_ids, struct Vector *)     \
  FXN(window, qty, get_windows_qty, size_t)             \
  FXN(window, names, get_window_names, struct Vector *) \
  FXN(window, props, get_window_props, struct Vector *) \
  FXN(window, select, select_window_name, char *) \
//////////////////////////////////////

#define MODULE_DEFINE_FUNCTION(MODULE_NAME, SUFFIX, FUNCTION, RETURN_TYPE)       RETURN_TYPE (*SUFFIX)(void);
#define MODULE_PROTOTYPE_FUNCTION(MODULE_NAME, SUFFIX, FUNCTION, RETURN_TYPE)    RETURN_TYPE MODULE_NAME ## _module_ ## SUFFIX(module(MODULE_NAME) * exports);
#define MODULE_PROTOTYPE_FUNCTION_INPUT(MODULE_NAME, SUFFIX, FUNCTION, RETURN_TYPE, INPUT_TYPE)    RETURN_TYPE MODULE_NAME ## _module_ ## SUFFIX(module(MODULE_NAME) * exports, INPUT_TYPE);
#define MODULE_EXPORT_FUNCTION(MODULE_NAME, SUFFIX, FUNCTION, RETURN_TYPE)       .SUFFIX = MODULE_NAME ## _module_ ## SUFFIX,
#define MODULE_FUNCTION(MODULE_NAME, SUFFIX, FUNCTION, RETURN_TYPE)              RETURN_TYPE MODULE_NAME ## _module_ ## SUFFIX(module(MODULE_NAME) * exports){ return(FUNCTION()); }
//////////////////////////////////////
#include "core/core.h"
//////////////////////////////////////
#include "window/info/info.h"
#include "window/utils/utils.h"
//////////////////////////////////////
#define MAP_FOREACH(key, val, map)                                            \
  for (struct _map_iterator iter __attribute__((cleanup(_map_iter_cleanup))); \
       (key) = iter->pair->key,                                               \
       (value) = iter->pair->value;                                           \
       iter = iter->get_next_cb())

enum window_mode {
  WINDOW_NONE,
  WINDOW_INFO,
  WINDOW_ERROR,
  WINDOW_DEBUG,
};

module(window) {
  define(window, CLIB_MODULE);
  enum window_mode mode;
  MODULE_FUNCTIONS(MODULE_DEFINE_FUNCTION)
};
#undef MODULE_DEFINE_FUNCTION

int  window_module_init(module(window) * exports);
void window_module_deinit(module(window) * exports);
MODULE_FUNCTIONS(MODULE_PROTOTYPE_FUNCTION)
MODULE_VAR_FUNCTIONS(MODULE_PROTOTYPE_FUNCTION_INPUT)
#undef MODULE_PROTOTYPE_FUNCTION

exports(window) {
  .mode = WINDOW_NONE,
  .init = window_module_init,
  MODULE_FUNCTIONS(MODULE_EXPORT_FUNCTION)
  .deinit = window_module_deinit,
};
#undef MODULE_EXPORT_FUNCTION
#endif
