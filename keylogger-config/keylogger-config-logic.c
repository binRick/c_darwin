#pragma once
////////////////////////////////////////////////////////
#include <stdio.h>
////////////////////////////////////////////////////////
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
////////////////////////////////////////////////////////
#include "c_vector/include/vector.h"
////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
struct keylogger_config_keybind_action_result_t {
};
struct keylogger_config_keybind_action_t {
};
struct keylogger_config_keybind_keys_t {
};
struct keylogger_execution_t {
};
struct keylogger_config_keybind_t {
};


enum keylogger_config_action_type_t {
    X,
} keylogger_config_action_type_t;
enum keylogger_config_keybind_type_t {
    XZ,
} static  keylogger_config_keybind_type_t;
enum keylogger_config_mode_type_t {
    XY,
} static keylogger_config_mode_type_t;
enum keylogger_config_key_type_t {
    XX,
} static keylogger_config_key_type_t;
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
//////////////////////////////////////////////////////////////////////////////////////////////////
static struct Vector *keylogger_get_config_keybind_actions(int ACTION_TYPE){
    struct Vector *v = vector_new();
    return(v);
}
static size_t keylogger_config_get_actions_qty(int ACTION_TYPE){
    return(0);
}
static size_t keylogger_config_get_keybind_keys_qty(){
    return(0);
}
static size_t keylogger_config_get_keybinds_qty(int KEYBIND_TYPE){
    return(0);
}
static bool keylogger_config_run_action(struct keylogger_config_keybind_action_t *KEYBIND_ACTION_TO_RUN){
    return(false);
}
static bool keylogger_config_keybind_is_active(struct keylogger_config_keybind_t *KEYBIND){
    return(false);
}
static bool keylogger_config_has_keybind_v(struct Vector *KEYBINDS_VECTOR){
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
//////////////////////////////////////////////////////////////////////////////////////////////////
module(keylogger_config_keybind) {
  KEYLOGGER_CONFIG_COMMON_PROPERTIES(keylogger_config_keybind)
  bool           (*is_active)(struct keylogger_config_keybind_t *KEYBIND);
  struct Vector *(*get_keybinds)(int KEYBIND_TYPE);
  struct Vector *(*get_actions)(int ACTION_TYPE);
};
KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config_keybind)
exports(keylogger_config_keybind) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config_keybind)
  .is_active = keylogger_config_keybind_is_active,
  .get_keybinds = keylogger_get_config_keybinds,
  .get_actions = keylogger_get_config_keybind_actions,
};
//////////////////////////////////////////////////////////////////////////////////////////////////
module(keylogger_config_keybind_key) {
  KEYLOGGER_CONFIG_COMMON_PROPERTIES(keylogger_config_keybind)
  struct Vector *(*get_keys)(int KEY_TYPE);
};
KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config_keybind_key)
exports(keylogger_config_keybind_key) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config_keybind_key)
  .get_keys = keylogger_get_config_keybind_keys,
};
//////////////////////////////////////////////////////////////////////////////////////////////////
module(keylogger_config_action) {
  KEYLOGGER_CONFIG_COMMON_PROPERTIES(keylogger_config_action)
  bool      (*run)(struct keylogger_config_keybind_action_t *ACTION);
};
KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config_action)
exports(keylogger_config_action) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config_action)
  .run = keylogger_config_run_action,
};
//////////////////////////////////////////////////////////////////////////////////////////////////
module(keylogger_config) {
  KEYLOGGER_CONFIG_COMMON_PROPERTIES(keylogger_config)
  bool (*has_keybind_v)(struct Vector *KEYBINDS_VECTOR);
  size_t (*get_keybinds_qty)(int KEYBIND_TYPE);
  size_t (*get_keybind_keys_qty)(void);
  size_t (*get_actions_qty)(int ACTION_TYPE);
  keylogger_config_action_t *actions;
  keylogger_config_keybind_t *keybinds;
  keylogger_config_keybind_key_t *keybind_keys;
};
KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config)
exports(keylogger_config) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config)
  .has_keybind_v = keylogger_config_has_keybind_v,
  .get_keybinds_qty = keylogger_config_get_keybinds_qty,
  .get_keybind_keys_qty = keylogger_config_get_keybind_keys_qty,
  .get_actions_qty = keylogger_config_get_actions_qty,
};
int keylogger_config_action_module_init(module(keylogger_config_action) *exports) {
  if(exports->is_loaded == true){
    printf("init module(action) already loaded)\n");
        return(EXIT_SUCCESS);
  }
  exports->is_loaded = true;
  printf("init module(action)\n");
  return 0;
}


int keylogger_config_keybind_module_init(module(keylogger_config_keybind) *exports) {
  if(exports->is_loaded == true){
    printf("init module(keybind) already loaded)\n");
        return(EXIT_SUCCESS);
  }
  exports->is_loaded = true;
  printf("init module(keybind)\n");
  return 0;
}
int keylogger_config_keybind_key_module_init(module(keylogger_config_keybind_key) *exports) {
  if(exports->is_loaded == true){
    printf("init module(keybind_key) already loaded)\n");
        return(EXIT_SUCCESS);
  }
  exports->is_loaded = true;
  printf("init module(keybind key)\n");
  return 0;
}
int keylogger_config_module_init(module(keylogger_config) *exports) {
  clib_module_init(keylogger_config, exports);
  if(exports->is_loaded == true){
    printf("init module(config already loaded)\n");
        return(EXIT_SUCCESS);
  }
  exports->actions = require(keylogger_config_action);
  exports->keybinds = require(keylogger_config_keybind);
  exports->keybind_keys = require(keylogger_config_keybind_key);
  printf("klc> Loaded %lu Actions\n", exports->get_actions_qty(0));
  printf("klc> Loaded %lu Keybinds\n", exports->get_keybinds_qty(0));
  printf("klc> Loaded %lu Keybind Keys\n", exports->get_keybind_keys_qty());
  exports->is_loaded = true;
  printf("init module(config)\n");
  return 0;
}
void keylogger_config_action_module_deinit(module(keylogger_config_action) *exports) {
  clib_module_deinit(keylogger_config_action);
  printf("deinit module(action)\n");
  exports->is_loaded = false;
}
void keylogger_config_keybind_module_deinit(module(keylogger_config_keybind) *exports) {
  clib_module_deinit(keylogger_config_keybind);
  printf("deinit module(keybind)\n");
  exports->is_loaded = false;
}
void keylogger_config_keybind_key_module_deinit(module(keylogger_config_keybind_key) *exports) {
  clib_module_deinit(keylogger_config_keybind_key);
  printf("deinit module(keybind key)\n");
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
  exports->is_loaded = false;
  printf("deinit module(keylogger_config)\n");
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void deinit_keylogger_config(module(keylogger_config) *keylogger_config){
  printf("klc> Unloadin\n");
  clib_module_free(keylogger_config);
}

module(keylogger_config) *init_keylogger_config(void){
  keylogger_config_t *klc;
  klc = require(keylogger_config);
  return(klc);
}
__attribute__((constructor))static void __keylogger_config_constructor(void){
}
__attribute__((destructor))static void __keylogger_config_destructor(void){}
//////////////////////////////////////////////////////////////////////////////////////////////////
