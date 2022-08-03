#pragma once
#include "c_vector/include/vector.h"
#include "keylogger-config.h"
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
#include "parson/parson.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
//////////////////////////////////////////////////////////////////////////////////////////////////
#define KEYLOGGER_CONFIG_COMMON_PROPERTIES(NAME) \
  define(NAME, CLIB_MODULE);                     \
  int  mode;                                     \
  bool is_loaded;
#define KEYLOGGER_CONFIG_COMMON_MODULE(NAME) \
    .mode    = KEYLOGGER_CONFIG_MODE_DEBUG,  \
  .is_loaded = false;
#define KEYLOGGER_CONFIG_COMMON_FUNCTIONS(NAME)     \
  int NAME ## _module_init(module(NAME) * exports); \
  void NAME ## _module_deinit(module(NAME) * exports);
#define KEYLOGGER_CONFIG_COMMON_EXPORTS(NAME) \
    .mode = 0,                                \
  .init   = NAME ## _module_init,             \
  .deinit = NAME ## _module_deinit,
//////////////////////////////////////////////////////////////////////////////////////////////////
enum keylogger_config_action_type_t;
enum keylogger_config_keybind_type_t;
enum keylogger_config_mode_type_t;
enum keylogger_config_key_type_t;
//////////////////////////////////////////////////////////////////////////////////////////////////
struct keylogger_config_keybind_action_result_t;
struct keylogger_config_keybind_action_t;
struct keylogger_config_keybind_keys_t;
struct keylogger_execution_t;
struct keylogger_config_keybind_t;
//////////////////////////////////////////////////////////////////////////////////////////////////
static bool keylogger_execution_stop();
static bool keylogger_execution_is_started();
static bool keylogger_execution_start();
static struct Vector *keylogger_get_config_keybind_actions(int ACTION_TYPE);
static bool keylogger_config_run_action(struct keylogger_config_keybind_action_t *KEYBIND_ACTION_TO_RUN);
static bool keylogger_config_keybind_is_active(struct keylogger_config_keybind_t *KEYBIND);
static struct Vector *keylogger_get_config_keybinds(int KEYBIND_TYPE);
static struct Vector *keylogger_get_config_keybind_keys(int KEY_TYPE);
size_t keylogger_config_get_actions_qty(int ACTION_TYPE);
size_t keylogger_config_get_keybind_keys_qty();
size_t keylogger_config_get_keybinds_qty(int KEYBIND_TYPE);
bool keylogger_config_has_keybind_v(struct Vector *KEYBINDS_VECTOR);

//////////////////////////////////////////////////////////////////////////////////////////////////
module(keylogger_config) {
  KEYLOGGER_CONFIG_COMMON_PROPERTIES(keylogger_config)
  bool          (*read_config)(void);
  bool          (*parse_config)(void);
  bool          (*start)(void);
  bool          (*stop)(void);
  bool          (*has_keybind_v)(struct Vector *KEYBINDS_VECTOR);
  bool          (*get_is_started)(void);
  unsigned long (*get_started_ts)(void);
  size_t        (*get_keybinds_qty)(int KEYBIND_TYPE);
  size_t        (*get_keybind_keys_qty)(void);
  size_t        (*get_actions_qty)(int ACTION_TYPE);
  pid_t         (*get_pid)();
  module(keylogger_config_actions) * actions;
  module(keylogger_config_keybinds) * keybinds;
  module(keylogger_config_keybind_keys) * keybind_keys;
  module(keylogger_config_execution) * execution;
  const char                   *config_s;
  struct keylogger_execution_t *execution_state;
};
KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config)
exports(keylogger_config) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config)
    .start              = NULL, .stop = NULL, .get_is_started = NULL, .get_started_ts = NULL, .get_pid = NULL, .read_config = NULL,
  .actions              = NULL,
  .keybinds             = NULL,
  .keybind_keys         = NULL,
  .execution            = NULL,
  .has_keybind_v        = keylogger_config_has_keybind_v,
  .get_keybinds_qty     = keylogger_config_get_keybinds_qty,
  .get_keybind_keys_qty = keylogger_config_get_keybind_keys_qty,
  .get_actions_qty      = keylogger_config_get_actions_qty,
};
//////////////////////////////////////////////////////////////////////////////////////////////////
