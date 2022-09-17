#pragma once
#ifndef HOTKEY_UTILS_C
#define HOTKEY_UTILS_C
////////////////////////////////////////////
#include "hotkey-utils/hotkey-utils-types.h"
#include "hotkey-utils/hotkey-utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "libcyaml/include/cyaml/cyaml.h"
#include "log/log.h"
#include "ms/ms.h"
#include "path_module/src/path.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"
#include <libgen.h>
#include <limits.h>
////////////////////////////////////////////
static bool HOTKEY_UTILS_DEBUG_MODE = false;
static char *EXECUTABLE_PATH_DIRNAME;

///////////////////////////////////////////////////////////////////////
char *get_homedir_yaml_config_file_path(){
  char *path;
  char *home = getenv("HOME");

  if (!home) {
    log_error("HOME Environment variable not set!");
    exit(EXIT_FAILURE);
  }
  asprintf(&path, "%s/.config/darwin-ls/hotkeys.yaml", home);
  if (HOTKEY_UTILS_DEBUG_MODE == true) {
    log_debug("Config path:  %s", path);
  }
  if (fsio_file_exists(path) == false) {
    fsio_mkdirs_parent(path, 0700);
    log_error("Config File %s is Missing. Create it.", path);
    exit(EXIT_FAILURE);
  }
  if (HOTKEY_UTILS_DEBUG_MODE == true) {
    log_debug("Using Hotkeys file %s", path);
  }
  return(path);
}

int execute_hotkey_config_key(struct key_t *key){
  return(handle_action(key->action_type, key->action));
}

struct key_t *get_hotkey_config_key(struct hotkeys_config_t *cfg, char *key){
  for (size_t i = 0; i < cfg->keys_count; i++) {
    if (HOTKEY_UTILS_DEBUG_MODE == true) {
      log_info("Comparing key %s to %s", cfg->keys[i].key, key);
    }
    if (strcmp(cfg->keys[i].key, key) == 0) {
      if (HOTKEY_UTILS_DEBUG_MODE == true) {
        log_debug("Key Match!");
      }
      return(&(cfg->keys[i]));
    }
  }
  return(NULL);
}

int increase_focused_application_width_ten_percent(){
  int cur_width = -1, cur_height = -1, cur_x = -1, cur_y = -1, new_width = -1, new_height = -1, new_x = -1, new_y = -1, pid = -1;

  pid = get_focused_pid();
  log_info("Increasing focused app width 10%%|pid:%d|cur size:%dx%d|cur pos:%dx%d|new size:%dx%d,new pos:%dx%d",
           pid,
           cur_width, cur_height,
           cur_x, cur_y,
           new_width, new_height,
           new_x, new_y
           );

//  log_debug("resizing window %lu to %dx%d", w->window_id, args->width, args->height);
  //resize_window(w, args->width, args->height);
  return(EXIT_FAILURE);
}

int decrease_focused_application_width_ten_percent(){
  int cur_width = -1, cur_height = -1, cur_x = -1, cur_y = -1, new_width = -1, new_height = -1, new_x = -1, new_y = -1, pid = -1;

  pid = get_focused_pid();
  log_info("Decreasing focused app width 10%%|pid:%d|cur size:%dx%d|cur pos:%dx%d|new size:%dx%d,new pos:%dx%d",
           pid,
           cur_width, cur_height,
           cur_x, cur_y,
           new_width, new_height,
           new_x, new_y
           );
  return(EXIT_FAILURE);
}

int minimize_application(void *APPLICATION_NAME){
  log_info("Minimize app %s", (char *)APPLICATION_NAME);
  size_t window_id = get_first_window_id_by_app_name((char *)APPLICATION_NAME);
  set_window_id_flags(window_id, WINDOW_FLAG_MINIMIZE);
  return(EXIT_SUCCESS);
}

int fullscreen_application(void *APPLICATION_NAME){
  log_info("Fullscreen app %s", (char *)APPLICATION_NAME);
  return(EXIT_SUCCESS);
}

int activate_application(void *APPLICATION_NAME){
  if (HOTKEY_UTILS_DEBUG_MODE == true) {
    log_info("Activating Application %s", (char *)APPLICATION_NAME);
  }
  size_t window_id = get_first_window_id_by_app_name((char *)APPLICATION_NAME);
  if (window_id == 0) {
    log_error("Failed to find window named '%s'", (char *)APPLICATION_NAME);
  }else{
    if (HOTKEY_UTILS_DEBUG_MODE == true) {
      log_info("Got Window ID %lu", window_id);
    }
    focus_window_id(window_id);
  }
  return(EXIT_SUCCESS);
}

int deactivate_application(void *APPLICATION_NAME){
  log_info("Deactivating Application %s", (char *)APPLICATION_NAME);
  return(EXIT_SUCCESS);
}

int handle_action(enum action_type_t action_type, void *action){
  return(action_type_handlers[action_type].fxn(action));
}

struct hotkeys_config_t *load_yaml_config_file_path(char *config_file_path){
  unsigned long           started = timestamp();
  struct hotkeys_config_t *hotkeys_config;

  cyaml_err_t             err = cyaml_load_file(config_file_path, &config, &top_schema, (cyaml_data_t **)&hotkeys_config, NULL);

  if (err != CYAML_OK) {
    log_error("%s", cyaml_strerror(err));
    return(NULL);
  }
  if (HOTKEY_UTILS_DEBUG_MODE == true) {
    log_info("Loaded %lu hotkeys from Config file %s in %s",
             hotkeys_config->keys_count, config_file_path,
             milliseconds_to_string(timestamp() - started)
             );
  }
  return(hotkeys_config);
}

char *get_yaml_config_file_path(char **argv){
  char EXECUTABLE_PATH[PATH_MAX + 1] = { 0 };

  realpath(argv[0], EXECUTABLE_PATH);
  EXECUTABLE_PATH_DIRNAME = dirname(EXECUTABLE_PATH);
  if (HOTKEY_UTILS_DEBUG_MODE == true) {
    log_info("EXECUTABLE_PATH_DIRNAME:%s", EXECUTABLE_PATH_DIRNAME);
  }
  char *path;

  asprintf(&path, "%s/../../hotkey-utils/%s", EXECUTABLE_PATH_DIRNAME, HOTKEYS_CONFIG_FILE_NAME);
  return(path);
}

////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__hotkey_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_hotkey_utils") != NULL) {
    log_debug("Enabling hotkey-utils Debug Mode");
    HOTKEY_UTILS_DEBUG_MODE = true;
  }
}
#endif
