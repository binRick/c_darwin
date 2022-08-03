#pragma once
//////////////////////////////////////////
#define KEYLOGGER_CONFIG_LOG_DEFAULT "DEBUG"
#define KEYLOGGER_KEYBINDS_CONFIG_PATH "etc/keybinds.json"
//////////////////////////////////////////
#include "keylogger-config-includes.h"
#include "keylogger-config-types.h"
#include "keylogger-config-logic.h"
//////////////////////////////////////////
/*
#define KEYLOGGER_CONFIG_COMMON_PROPERTIES(NAME) \
  define(NAME, CLIB_MODULE);                     \
  int mode;\
  bool is_loaded;\
//////////////////////////////////////////////////////////////////////////////////////////////////
#define KEYLOGGER_CONFIG_COMMON_MODULE(NAME) \
  .mode = KEYLOGGER_CONFIG_MODE_DEBUG,\
  .is_loaded = false;\
//////////////////////////////////////////////////////////////////////////////////////////////////
#define KEYLOGGER_CONFIG_COMMON_FUNCTIONS(NAME)     \
  int NAME ## _module_init(module(NAME) * exports); \
  void NAME ## _module_deinit(module(NAME) * exports);
//////////////////////////////////////////////////////////////////////////////////////////////////
#define KEYLOGGER_CONFIG_COMMON_EXPORTS(NAME) \
  .mode = 0,                  \
  .init   = NAME ## _module_init,             \
  .deinit = NAME ## _module_deinit,
  */
//////////////////////////////////////////
module(keylogger_config) *init_keylogger_config(void);
void deinit_keylogger_config(module(keylogger_config) *keylogger_config);
