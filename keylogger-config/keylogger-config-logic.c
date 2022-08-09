#pragma once
////////////////////////////////////////////////////////
#include <assert.h>
#include <inttypes.h>
#include <libgen.h>
#include <libproc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
////////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/include/vector.h"
#include "keylogger-config-logic.h"
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
#include "timestamp/timestamp.h"
////////////////////////////////////////////////////////
#include "c_vector/include/vector.h"
//////////////////////////////////////////////////////////////////////////////////////////////////
enum keylogger_config_action_type_t {
  KEYLOGGER_CONFIG_ACTION_TYPE_EXECUTE,
  KEYLOGGER_CONFIG_ACTION_TYPE_FOCUS_APP,
  KEYLOGGER_CONFIG_ACTION_TYPES_QTY,
} keylogger_config_action_type_t;
enum keylogger_config_keybind_type_t {
  KEYLOGGER_CONFIG_KEYBIND_TYPE_ANY,
  KEYLOGGER_CONFIG_KEYBIND_TYPE_ACTIVE,
  KEYLOGGER_CONFIG_KEYBIND_TYPE_INACTIVE,
  KEYLOGGER_CONFIG_KEYBIND_TYPES_QTY,
} keylogger_config_keybind_t;
enum keylogger_config_log_mode_type_t {
  KEYLOGGER_LOG_MODE_ERROR,
  KEYLOGGER_LOG_MODE_WARNING,
  KEYLOGGER_LOG_MODE_DEBUG,
  KEYLOGGER_LOG_MODE_TRACE,
  KEYLOGGER_LOG_MODES_QTY,
} keylogger_config_log_mode_type_t;
enum keylogger_config_mode_type_t {
  KEYLOGGER_MODE_STARTED,
  KEYLOGGER_MODE_STOPPED,
  KEYLOGGER_MODES_QTY,
} keylogger_config_mode_type_t;
enum keylogger_config_key_type_t {
  KEYLOGGER_CONFIG_KEY_TYPES_QTY,
} keylogger_config_key_type_t;
//////////////////////////////////////////////////////////////////////////////////////////////////
static bool keylogger_execution_parse_config(void);
static bool keylogger_execution_read_config(void);
static bool keylogger_execution_get_is_started(void);
static pid_t keylogger_execution_get_pid(void);
static size_t keylogger_execution_get_started_ts(void);

//////////////////////////////////////////////////////////////////////////////////////////////////
struct keylogger_config_action_t {
  char *type, *data;
  bool active;
};
struct keylogger_config_keybind_action_result_t {
};
struct keylogger_config_keybind_keys_t {
  bool          active;
  struct Vector *keys_v;
};
struct keylogger_config_keybind_t {
  char          *name;
  bool          active;
  struct Vector *actions_v;
  struct Vector *keys_v;
};
//////////////////////////////////////////////////////////////////
struct keylogger_execution_t {
  pid_t  pid;
  size_t started_ts;
  bool   is_started;
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
  struct keylogger_execution_t state;
  bool                         (*start)(void);
  bool                         (*stop)(void);
  bool                         (*get_is_started)(void);
  pid_t                        (*get_pid)(void);
  size_t                       (*get_started_ts)(void);
};

KEYLOGGER_CONFIG_COMMON_FUNCTIONS(keylogger_config_execution)
exports(keylogger_config_execution) {
  KEYLOGGER_CONFIG_COMMON_EXPORTS(keylogger_config_execution)
    .get_is_started = NULL, .get_pid = NULL, .get_started_ts = NULL,
  .state            = {
    .pid        = 0,
    .started_ts = 0,
    .is_started = false,
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
  if (exports->is_loaded == true) {
    printf("init module(keybinds) already loaded)\n");
    return(EXIT_SUCCESS);
  }
  clib_module_init(keylogger_config_keybinds, exports);
  exports->is_loaded = true;
  printf("init module(keybinds)\n");
  return(0);
}


int keylogger_config_keybind_keys_module_init(module(keylogger_config_keybind_keys) *exports) {
  if (exports->is_loaded == true) {
    printf("init module(keylogger_keybind_keys) already loaded)\n");
    return(EXIT_SUCCESS);
  }
  clib_module_init(keylogger_config_keybind_keys, exports);
  exports->is_loaded = true;
  printf("init module(keybind_keys)\n");
  return(0);
}


int keylogger_config_actions_module_init(module(keylogger_config_actions) *exports) {
  if (exports->is_loaded == true) {
    printf("init module(actions) already loaded)\n");
    return(EXIT_SUCCESS);
  }
  clib_module_init(keylogger_config_actions, exports);
  exports->is_loaded = true;
  printf("init module(actions)\n");
  return(0);
}


void keylogger_config_execution_module_deinit(module(keylogger_config_execution) *exports) {
  clib_module_deinit(keylogger_config_execution);
  printf("deinit module(execution)\n");
  exports->is_loaded = false;
}


int keylogger_config_execution_module_init(module(keylogger_config_execution) *exports) {
  if (exports->is_loaded == true) {
    printf("init module(execution) already loaded)\n");
    return(EXIT_SUCCESS);
  }
  printf("init module(execution) loading\n");
  clib_module_init(keylogger_config_execution, exports);
  exports->is_loaded        = true;
  exports->get_started_ts   = keylogger_execution_get_started_ts;
  exports->state.started_ts = exports->get_started_ts();
  exports->state.is_started = false;
  exports->get_pid          = keylogger_execution_get_pid;
  exports->state.pid        = exports->get_pid();
  exports->stop             = keylogger_execution_stop;
  exports->start            = keylogger_execution_start;
  exports->get_is_started   = keylogger_execution_get_is_started;
  exports->is_loaded        = true;
  printf("init module(execution)\n");
  return(0);
}


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


void keylogger_config_keybind_keys_module_deinit(module(keylogger_config_keybind_keys) *exports) {
  clib_module_deinit(keylogger_config_keybind_keys);
  printf("deinit module(keybind keys)\n");
  exports->is_loaded = false;
}


void keylogger_config_module_deinit(module(keylogger_config) *exports) {
  if (exports->is_loaded != true) {
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
module(keylogger_config_execution) * get_keylogger_config_execution(void){
  return(require(keylogger_config_execution));
}
module(keylogger_config) * init_keylogger_config(void){
  return(require(keylogger_config));
}
__attribute__((constructor))static void __keylogger_config_constructor(void){
}
__attribute__((destructor))static void __keylogger_config_destructor(void){
}

#define KEYLOGGER_IS_STARTED    ()


static bool keylogger_execution_get_is_started(){
  return(true);
// return(require(keylogger_execution)->state.started == true);
}


void deinit_keylogger_config(module(keylogger_config) *keylogger_config){
  printf("klc> Unloadin\n");
  clib_module_free(keylogger_config);
}


int keylogger_config_module_init(module(keylogger_config) *exports) {
  clib_module_init(keylogger_config, exports);
  if (exports->is_loaded == true) {
    printf("init module(config already loaded)\n");
    return(EXIT_SUCCESS);
  }
  exports->keybinds_v   = vector_new();
  exports->actions      = require(keylogger_config_actions);
  exports->keybinds     = require(keylogger_config_keybinds);
  exports->keybind_keys = require(keylogger_config_keybind_keys);
  exports->execution    = require(keylogger_config_execution);
  ///////////////////////////////////////////////////////////////////
  exports->start          = exports->execution->start;
  exports->stop           = exports->execution->stop;
  exports->get_pid        = exports->execution->get_pid;
  exports->get_started_ts = exports->execution->get_started_ts;
  exports->get_is_started = exports->execution->get_is_started;
  exports->read_config    = keylogger_execution_read_config;
  exports->parse_config   = keylogger_execution_parse_config;
  ///////////////////////////////////////////////////////////////////
  bool read_ok  = exports->read_config();
  bool parse_ok = exports->parse_config();
  bool start_ok = exports->start();
  ///////////////////////////////////////////////////////////////////
  exports->is_loaded = (read_ok && parse_ok && start_ok);
  printf("klc> Is Loaded: %s\n", exports->is_loaded == true ? "Yes" : "No");
  printf("klc> Read Config: %s\n", read_ok == true ? "Yes" : "No");
  printf("klc> Parsed Config: %s\n", parse_ok == true ? "Yes" : "No");
  printf("klc> Loaded %lu Keybinds (%lu active, %lu inactive)\n",
         exports->get_keybinds_qty(),
         exports->get_active_keybinds_qty(),
         exports->get_inactive_keybinds_qty()
         );
  printf("klc> Loaded %lu Actions\n", exports->get_keybind_actions_qty(0));
//  printf("klc> Loaded %lu Keybind Keys\n", exports->get_keybind_keys_qty());
  printf("klc> Execution PID %d\n", exports->get_pid());
  printf("klc> Started Timestamp %ld\n", exports->get_started_ts());
  ///////////////////////////////////////////////////////////////////
  return(0);
}


static pid_t keylogger_execution_get_pid(){
  printf("getting pid\n");
  get_keylogger_config_execution()->state.pid    = getpid();
  require(keylogger_config_execution)->state.pid = getpid();
  return(require(keylogger_config_execution)->state.pid);
}


static unsigned long keylogger_execution_get_started_ts(void){
  printf("getting ts\n");
  return(require(keylogger_config_execution)->state.started_ts);
}


static bool keylogger_execution_start(){
  printf("starting...............\n");
  require(keylogger_config_execution)->state.started_ts = timestamp();
  return(true);
}


static bool keylogger_execution_read_config(void){
  printf("READING CONFIG...............\n");
  char *cfg = fsio_read_text_file(KEYLOGGER_KEYBINDS_CONFIG_PATH);
  printf(AC_YELLOW "%s" AC_RESETALL, cfg);
  require(keylogger_config)->config_s = cfg;
  return(true);
}


static bool keylogger_execution_parse_config(){
  if (require(keylogger_config)->config_s == NULL) {
    keylogger_execution_read_config();
  }
  struct Vector *keybinds_v = vector_new();

  char          *cfg = require(keylogger_config)->config_s;
  printf(AC_BLUE "%s" AC_RESETALL, cfg);
  JSON_Value    *parsed_cfg_r = json_parse_string(require(keylogger_config)->config_s);
  JSON_Object   *parsed_cfg_o = json_value_get_object(parsed_cfg_r);
  JSON_Array    *keybinds_a   = json_object_dotget_array(parsed_cfg_o, "keybinds");
  char          *msg;
  size_t        keybinds_qty = json_array_get_count(keybinds_a);
  printf(AC_YELLOW "%lu keybinds!\n" AC_RESETALL, keybinds_qty);
  for (size_t i = 0; i < keybinds_qty; i++) {
    struct keylogger_config_keybind_t *kb = malloc(sizeof(struct keylogger_config_keybind_t));
    struct Vector                     *keybind_keys_v = vector_new, *keybind_actions_v = vector_new();
    kb->actions_v = vector_new();
    kb->keys_v    = vector_new();
    JSON_Value  *kb_v = json_array_get_value(keybinds_a, i);
    JSON_Object *kb_o = json_value_get_object(kb_v);
    kb->name   = json_object_dotget_string(kb_o, "name");
    kb->active = json_object_dotget_boolean(kb_o, "active");
    JSON_Array *kb_keys_a    = json_object_dotget_array(kb_o, "keys");
    JSON_Array *kb_actions_a = json_object_dotget_array(kb_o, "actions");
    asprintf(&msg, "Keybind #%lu (%s) [active:%s] |%lu sets of keys matches|%lu actions|",
             i,
             kb->name,
             (kb->active == true) ? "Yes" : "No",
             json_array_get_count(kb_keys_a),
             json_array_get_count(kb_actions_a)
             );
    printf(AC_CYAN "%s\n" AC_RESETALL, msg);
    for (size_t ii = 0; ii < json_array_get_count(kb_actions_a); ii++) {
      JSON_Value                       *kb_action_v = json_array_get_value(kb_actions_a, ii);
      JSON_Object                      *kb_action_o = json_value_get_object(kb_action_v);
      struct keylogger_config_action_t *ac          = malloc(sizeof(struct keylogger_config_action_t));
      ac->active = json_object_dotget_boolean(kb_action_o, "active");
      ac->type   = json_object_dotget_string(kb_action_o, "type");
      ac->data   = json_object_dotget_string(kb_action_o, "data");
      vector_push(kb->actions_v, ac);

      asprintf(&msg, " - Action #%lu|type:%s|data:%s|active:%s|",
               ii, ac->type, ac->data,
               (ac->active == true) ? "Yes" : "No"
               );
      printf(AC_CYAN "%s\n" AC_RESETALL, msg);
    }
    for (size_t ii = 0; ii < json_array_get_count(kb_keys_a); ii++) {
      JSON_Value                             *key_v        = json_array_get_value(kb_keys_a, ii);
      JSON_Object                            *key_o        = json_value_get_object(key_v);
      JSON_Array                             *key_combos_a = json_object_dotget_array(key_o, "combos");
      struct keylogger_config_keybind_keys_t *bk           = malloc(sizeof(struct keylogger_config_keybind_keys_t));
      bk->active = json_object_dotget_boolean(key_o, "active");
      bk->keys_v = vector_new();

      bool key_combos_active = json_object_dotget_boolean(key_o, "active");
      asprintf(&msg, " - Key #%lu- |active:%s|%lu key combos: ",
               ii,
               (key_combos_active == true) ? "Yes" : "No",
               json_array_get_count(key_combos_a)
               );
      printf(AC_MAGENTA "%s" AC_RESETALL, msg);
      printf("         ");
      for (size_t iii = 0; iii < json_array_get_count(key_combos_a); iii++) {
        char *k_s = json_array_get_string(key_combos_a, iii);
        //   vector_push(bk->keys_v, k_s);
        asprintf(&msg, "%s ", k_s);
        printf(AC_RED "%s" AC_RESETALL, msg);
      }
      printf("\n");
      //vector_push(kb->keys_v, bk);
    }
    vector_push(keybinds_v, kb);
  }
  require(keylogger_config)->keybinds_v = keybinds_v;
  printf("Loaded %lu keybinds\n", vector_size(keybinds_v));
  printf("\
        KEYLOGGER_LOG_MODE_ERROR: %d\n\
        KEYLOGGER_LOG_MODE_WARNING: %d\n\
        KEYLOGGER_LOG_MODE_TRACE: %d\n\
        KEYLOGGER_LOG_MODES_QTY: %d\n\
        ",
         KEYLOGGER_LOG_MODE_ERROR,
         KEYLOGGER_LOG_MODE_WARNING,
         KEYLOGGER_LOG_MODE_TRACE,
         KEYLOGGER_LOG_MODES_QTY
         );
  return(true);
} /* keylogger_execution_parse_config */


static bool keylogger_execution_stop(){
  printf("stopping...............\n");
  return(true);
}
static bool keylogger_execution_start();

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
struct Vector *keylogger_config_get_keybind_keys_v(){
  struct Vector *v   = vector_new();
  struct Vector *cur = require(keylogger_config)->keybinds_v;

  for (size_t i = 0; i < vector_size(cur); i++) {
    struct keylogger_config_keybind_keys_t *kk = vector_get(cur, i);
    for (size_t ii = 0; ii < vector_size(kk->keys_v); ii++) {
//      vector_push(v, (struct keylogger_config_keybind_keys_t*)vector_get(kk->keys_v, ii));
    }
  }
  return(v);
}
struct Vector *keylogger_config_get_keybind_actions_v(){
  struct Vector *v   = vector_new();
  struct Vector *cur = require(keylogger_config)->keybinds_v;

  for (size_t i = 0; i < vector_size(cur); i++) {
    struct keylogger_config_keybind_t *kb = vector_get(cur, i);
    for (size_t ii = 0; ii < vector_size(kb->actions_v); ii++) {
      vector_push(v, (struct keylogger_config_keybind_action_t *)vector_get(kb->actions_v, ii));
    }
  }
  return(v);
}

struct Vector *keylogger_config_get_active_keybinds_v(void){
  struct Vector *v   = vector_new();
  struct Vector *cur = require(keylogger_config)->keybinds_v;

  for (size_t i = 0; i < vector_size(cur); i++) {
    struct keylogger_config_keybind_t *kb = vector_get(cur, i);
    if (kb->active == true) {
      vector_push(v, vector_get(cur, i));
    }
  }
  return(v);
}
struct Vector *keylogger_config_get_inactive_keybinds_v(void){
  struct Vector *v   = vector_new();
  struct Vector *cur = require(keylogger_config)->keybinds_v;

  for (size_t i = 0; i < vector_size(cur); i++) {
    struct keylogger_config_keybind_t *kb = vector_get(cur, i);
    if (kb->active == false) {
      vector_push(v, vector_get(cur, i));
    }
  }
  return(v);
}


size_t keylogger_config_get_active_keybinds_qty(void){
  return(vector_size(keylogger_config_get_active_keybinds_v()));
}


size_t keylogger_config_get_inactive_keybinds_qty(void){
  return(vector_size(keylogger_config_get_inactive_keybinds_v()));
}

struct Vector *keylogger_config_get_keybinds_v(){
  return(require(keylogger_config)->keybinds_v);
}


size_t keylogger_config_get_keybinds_qty(){
  return(vector_size(require(keylogger_config)->keybinds_v));
}


size_t keylogger_config_get_keybind_actions_qty(int ACTION_TYPE){
  return(vector_size(require(keylogger_config)->get_keybind_actions_v()));
}


size_t keylogger_config_get_keybind_keys_qty(){
  return(vector_size(require(keylogger_config)->get_keybind_keys_v()));
}


bool keylogger_config_has_keybind_v(struct Vector *KEYBINDS_VECTOR){
  return(false);
}
