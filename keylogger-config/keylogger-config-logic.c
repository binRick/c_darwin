#pragma once
////////////////////////////////////////////////////////
#include <inttypes.h>
#include <libgen.h>
#include <unistd.h>
#include <libproc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
////////////////////////////////////////////////////////
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
#include "keylogger-config-logic.h"
////////////////////////////////////////////////////////
#include "c_vector/include/vector.h"
//////////////////////////////////////////////////////////////////////////////////////////////////
static bool keylogger_execution_stop(){
    return(true);
}
static bool keylogger_execution_is_started(){
    return(true);
}
static bool keylogger_execution_start(){
    return(true);
}
static struct Vector *keylogger_get_config_keybind_actions(int ACTION_TYPE){
    struct Vector *v = vector_new();
    return(v);
}
static bool keylogger_config_run_action(struct keylogger_config_keybind_action_t *KEYBIND_ACTION_TO_RUN){
    return(false);
}
static bool keylogger_config_keybind_is_active(struct keylogger_config_keybind_t *KEYBIND){
    return(false);
}
static struct Vector *keylogger_get_config_keybinds(int KEYBIND_TYPE){
    struct Vector *v = vector_new();
    return(v);
}
static struct Vector *keylogger_get_config_keybind_keys(int KEY_TYPE){
    struct Vector *v = vector_new();
    return(v);
}
size_t keylogger_config_get_actions_qty(int ACTION_TYPE){
    return(0);
}
size_t keylogger_config_get_keybind_keys_qty(){
    return(0);
}
size_t keylogger_config_get_keybinds_qty(int KEYBIND_TYPE){
    return(0);
}
bool keylogger_config_has_keybind_v(struct Vector *KEYBINDS_VECTOR){
    return(false);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/*
module(keylogger_config) {
  KEYLOGGER_CONFIG_COMMON_PROPERTIES(keylogger_config)
  bool (*start)(void);
  bool (*has_keybind_v)(struct Vector *KEYBINDS_VECTOR);
  size_t (*get_keybinds_qty)(int KEYBIND_TYPE);
  size_t (*get_keybind_keys_qty)(void);
  size_t (*get_actions_qty)(int ACTION_TYPE);
  keylogger_config_action_t *actions;
  keylogger_config_keybind_t *keybinds;
  keylogger_config_keybind_key_t *keybind_keys;
  keylogger_config_execution_t *execution;
};
KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config)
exports(keylogger_config) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config)
  .has_keybind_v = keylogger_config_has_keybind_v,
  .get_keybinds_qty = keylogger_config_get_keybinds_qty,
  .get_keybind_keys_qty = keylogger_config_get_keybind_keys_qty,
  .get_actions_qty = keylogger_config_get_actions_qty,
  .start = NULL,
  .execution = NULL,
  .keybind_keys = NULL,
  .keybinds = NULL,
  .actions = NULL,
};
*/
struct keylogger_config_action_t {
    char *a;
};
struct keylogger_config_keybind_action_result_t {
};

struct keylogger_config_keybind_keys_t {
};
struct keylogger_config_keybind_t {
};
struct keylogger_execution_t {
    pid_t pid;
    bool (*start)(void);
    bool (*stop)(void);
    bool (*is_started)(void);
};
//////////////////////////////////////////////////////////////////////////////////////////////////

module(keylogger_config_actions) {
  KEYLOGGER_CONFIG_COMMON_PROPERTIES(keylogger_config_actions)
};
KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config_actions)
exports(keylogger_config_actions) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config_actions)
};

module(keylogger_config_execution) {
  KEYLOGGER_CONFIG_COMMON_PROPERTIES(keylogger_config_execution)
  struct keylogger_execution_t *state;
};
KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config_execution)
exports(keylogger_config_execution) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config_execution)
      .state = &(struct keylogger_execution_t){
          .pid = -1,
          .start = keylogger_execution_start,
          .stop = keylogger_execution_stop,
          .is_started = keylogger_execution_is_started,
      },
};

module(keylogger_config_keybind_keys) {
  KEYLOGGER_CONFIG_COMMON_PROPERTIES(keylogger_config_keybind_keys)
};
KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config_keybind_keys)
exports(keylogger_config_keybind_keys) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config_keybind_keys)
};

module(keylogger_config_keybinds) {
  KEYLOGGER_CONFIG_COMMON_PROPERTIES(keylogger_config_keybinds)
};
KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config_keybinds)
exports(keylogger_config_keybinds) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config_keybinds)
};

int keylogger_config_keybinds_module_init(module(keylogger_config_keybinds) *exports) {
  if(exports->is_loaded == true){
    printf("init module(keybinds) already loaded)\n");
        return(EXIT_SUCCESS);
  }
  exports->is_loaded = true;
  printf("init module(keybinds)\n");
  return 0;
}
int keylogger_config_keybind_keys_module_init(module(keylogger_config_keybind_keys) *exports) {
  if(exports->is_loaded == true){
    printf("init module(keylogger_keybind_keys) already loaded)\n");
        return(EXIT_SUCCESS);
  }
  exports->is_loaded = true;
  printf("init module(keybind_keys)\n");
  return 0;
}
int keylogger_config_actions_module_init(module(keylogger_config_actions) *exports) {
  if(exports->is_loaded == true){
    printf("init module(actions) already loaded)\n");
        return(EXIT_SUCCESS);
  }
  exports->is_loaded = true;
  printf("init module(actions)\n");
  return 0;
}

int keylogger_config_execution_module_init(module(keylogger_config_execution) *exports) {
  if(exports->is_loaded == true){
    printf("init module(execution) already loaded)\n");
        return(EXIT_SUCCESS);
  }
  exports->state->pid = getpid();
  exports->is_loaded = true;
  printf("init module(execution)\n");
  return 0;
}
int keylogger_config_module_init(module(keylogger_config) *exports) {
  clib_module_init(keylogger_config, exports);
  if(exports->is_loaded == true){
    printf("init module(config already loaded)\n");
        return(EXIT_SUCCESS);
  }
  exports->actions = require(keylogger_config_actions);
  exports->keybinds = require(keylogger_config_keybinds);
  exports->keybind_keys = require(keylogger_config_keybind_keys);
  exports->execution = require(keylogger_config_execution);
  exports->start = exports->execution->state->start;
  printf("klc> Loaded %lu Actions\n", exports->get_actions_qty(0));
  printf("klc> Loaded %lu Keybinds\n", exports->get_keybinds_qty(0));
  printf("klc> Loaded %lu Keybind Keys\n", exports->get_keybind_keys_qty());
//  printf("klc> Execution PID %d\n", exports->execution->state->pid);
  exports->is_loaded = true;
  printf("init module(config)\n");
  return 0;
}

void deinit_keylogger_config(module(keylogger_config) *keylogger_config){
  printf("klc> Unloadin\n");
  clib_module_free(keylogger_config);
}

__attribute__((constructor))static void __keylogger_config_constructor(void){
}
__attribute__((destructor))static void __keylogger_config_destructor(void){}
//////////////////////////////////////////////////////////////////////////////////////////////////
void keylogger_config_actions_module_deinit(module(keylogger_config_actions) *exports) {
  clib_module_deinit(keylogger_config_actions);
  printf("deinit module(actions)\n");
  exports->is_loaded = false;
}
void keylogger_config_keybinds_module_deinit(module(keylogger_config_keybinds) *exports) {
  clib_module_deinit(keylogger_config_keybinds);
  printf("deinit module(keybind)\n");
  exports->is_loaded = false;
}
void keylogger_config_execution_module_deinit(module(keylogger_config_execution) *exports) {
  clib_module_deinit(keylogger_config_execution);
  printf("deinit module(execution)\n");
  exports->is_loaded = false;
}
void keylogger_config_keybind_keys_module_deinit(module(keylogger_config_keybind_keys) *exports) {
  clib_module_deinit(keylogger_config_keybind_keys);
  printf("deinit module(keybind keys)\n");
  exports->is_loaded = false;
}
void keylogger_config_module_deinit(module(keylogger_config) *exports) {
  if(exports->is_loaded != true){
    printf("deinit module(config not loaded)\n");
        return(EXIT_SUCCESS);
  }
  printf("deinit module(keylogger_config) -> unloading)\n");
  clib_module_deinit(keylogger_config);
  clib_module_free(exports->keybinds);
  clib_module_free(exports->keybind_keys);
  clib_module_free(exports->actions);
  clib_module_free(exports->execution);
  exports->is_loaded = false;
  printf("deinit module(keylogger_config)\n");
}
//////////////////////////////////////////////////////////////////////////////////////////////////
module(keylogger_config) *init_keylogger_config(void){
  keylogger_config_t *klc;
  klc = require(keylogger_config);
  return(klc);
}
