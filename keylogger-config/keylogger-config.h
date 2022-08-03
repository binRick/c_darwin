#pragma once
//////////////////////////////////////////
#define KEYLOGGER_CONFIG_LOG_DEFAULT "DEBUG"
#define KEYLOGGER_KEYBINDS_CONFIG_PATH "etc/keybinds.json"
//////////////////////////////////////////
#include "keylogger-config-includes.h"
#include "keylogger-config-types.h"
#include "keylogger-config-logic.h"
//////////////////////////////////////////
module(keylogger_config) *init_keylogger_config(void);
void deinit_keylogger_config(module(keylogger_config) *keylogger_config);
