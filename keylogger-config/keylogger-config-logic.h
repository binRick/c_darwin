#pragma once
#include "c_vector/vector/vector.h"
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
enum keylogger_config_mode_type_t;
enum keylogger_config_log_mode_type_t;
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
struct Vector *keylogger_config_get_keybind_actions_v();
bool keylogger_config_has_keybind_v(struct Vector *KEYBINDS_VECTOR);
struct Vector *keylogger_config_get_active_keybinds_v(void);
struct Vector *keylogger_config_get_inactive_keybinds_v(void);
size_t keylogger_config_get_inactive_keybinds_qty(void);
size_t keylogger_config_get_keybind_actions_qty(int ACTION_TYPE);
struct Vector *keylogger_config_get_keybinds_v();
struct Vector *keylogger_config_get_keybind_keys_v();
size_t keylogger_config_get_keybind_keys_qty();
size_t keylogger_config_get_keybinds_qty();
size_t keylogger_config_get_active_keybinds_qty(void);

//////////////////////////////////////////////////////////////////////////////////////////////////
module(keylogger_config) {
  ///////////////////////////////////////////////////////
  KEYLOGGER_CONFIG_COMMON_PROPERTIES(keylogger_config)
  ///////////////////////////////////////////////////////
  module(keylogger_config_actions) * actions;
  module(keylogger_config_keybinds) * keybinds;
  module(keylogger_config_keybind_keys) * keybind_keys;
  module(keylogger_config_execution) * execution;
  ///////////////////////////////////////////////////////
  struct keylogger_execution_t *execution_state;
  ///////////////////////////////////////////////////////
  const char                   *config_s;
  ///////////////////////////////////////////////////////
  bool (*read_config)(void), (*parse_config)(void), (*start)(void), (*stop)(void),
  (*has_keybind_v)(struct Vector *KEYBINDS_VECTOR), (*get_is_started)(void);
  ///////////////////////////////////////////////////////
  unsigned long (*get_started_ts)(void);
  ///////////////////////////////////////////////////////
  size_t (*get_keybind_keys_qty)(void),
  (*get_keybind_actions_qty)(int ACTION_TYPE),
  (*get_keybinds_qty)(void),
  (*get_active_keybinds_qty)(void),
  (*get_inactive_keybinds_qty)(void);
  ///////////////////////////////////////////////////////
  struct Vector
  *keybinds_v,
  *(*get_keybinds_v)(void),
  *(*get_keybind_keys_v)(void),
  *(*get_active_keybinds_v)(void),
  *(*get_inactive_keybinds_v)(void),
  *(*get_keybind_actions_v)(void);
  ///////////////////////////////////////////////////////
  pid_t (*get_pid)();
  ///////////////////////////////////////////////////////
};
KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config)
exports(keylogger_config) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config)
    .execution               = NULL,
  .start                     = NULL, .stop = NULL, .get_is_started = NULL, .get_started_ts = NULL,
  .get_pid                   = NULL, .read_config = NULL,
  .actions                   = NULL,
  .keybinds                  = NULL,
  .keybind_keys              = NULL,
  .get_keybinds_qty          = keylogger_config_get_keybinds_qty,
  .has_keybind_v             = keylogger_config_has_keybind_v,
  .get_keybind_keys_v        = keylogger_config_get_keybind_keys_v,
  .get_keybind_keys_qty      = keylogger_config_get_keybind_keys_qty,
  .get_keybind_actions_qty   = keylogger_config_get_keybind_actions_qty,
  .get_keybind_actions_v     = keylogger_config_get_keybind_actions_v,
  .get_active_keybinds_v     = keylogger_config_get_active_keybinds_v,
  .get_inactive_keybinds_v   = keylogger_config_get_inactive_keybinds_v,
  .get_active_keybinds_qty   = keylogger_config_get_active_keybinds_qty,
  .get_inactive_keybinds_qty = keylogger_config_get_inactive_keybinds_qty,
  .get_keybinds_v            = keylogger_config_get_keybinds_v,
};
//////////////////////////////////////////////////////////////////////////////////////////////////
