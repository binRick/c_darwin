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
#include "murmurhash.c/murmurhash.h"
#include <libgen.h>
#include <limits.h>
#define HOTKEY_UTILS_HASH_SEED    212136436
#define RELOAD_CONFIG_MS 10000
#define HOTKEYS_CONFIG_FILE_NAME    "config.yaml"
////////////////////////////////////////////
static bool HOTKEY_UTILS_DEBUG_MODE = false, HOTKEY_UTILS_VERBOSE_DEBUG_MODE;
static char *EXECUTABLE_PATH_DIRNAME;

///////////////////////////////////////////////////////////////////////
size_t get_config_file_hash(char *CONFIG_FILE_PATH){
  char *config_contents = fsio_read_text_file(CONFIG_FILE_PATH);
  size_t config_hash = ((size_t)murmurhash(
           (const char *)config_contents,
           (uint32_t)strlen((const char *)config_contents),
           (uint32_t)HOTKEY_UTILS_HASH_SEED
           )
         );
  if(config_contents)
    free(config_contents);
  return(config_hash);
}

char *get_homedir_yaml_config_file_path(){
  char *path;
  char *home = getenv("HOME");
  if (!home) {
    log_error("HOME Environment variable not set!");
    exit(EXIT_FAILURE);
  }
  asprintf(&path, "%s/.config/darwin-ls/hotkeys.yaml", home);
  if (fsio_file_exists(path) == false) {
    fsio_mkdirs_parent(path, 0700);
    log_error("Config File %s is Missing. Create it.", path);
    exit(EXIT_FAILURE);
  }
  return(path);
}

int execute_hotkey_config_key(struct key_t *key){
  return(handle_action(key->action_type, key->action));
}

struct key_t *get_hotkey_config_key(struct hotkeys_config_t *cfg, char *key){
  for (size_t i = 0; i < cfg->keys_count; i++) {
    if (strcmp(cfg->keys[i].key, key) == 0) {
      return(&(cfg->keys[i]));
    }
  }
  return(NULL);
}

#define DEBUG_WINDOW_RESIZE(RESIZE_MODE) { do { \
    int cur_display_width = get_display_width(), cur_display_height = get_display_height();\
    log_debug("%s focused app width %f%% & height %f%%|pid:%d|name:%s|windowid:%lu|\n"\
           "                                 |cur size:%dx%d|cur pos:%dx%d|\n"\
           "                                 |new size:%dx%d,new pos:%dx%d|\n"\
           "                                 |cur dis size:%dx%d|\n"\
           "%s",\
           RESIZE_MODE,\
           width_factor,height_factor,\
           focused_window_info->pid,\
           focused_window_info->name,\
           focused_window_info->window_id,\
           (int)focused_window_info->rect.size.width,(int)focused_window_info->rect.size.height,\
           (int)focused_window_info->rect.origin.x,(int)focused_window_info->rect.origin.y,\
           (int)new_rect.size.width,(int)new_rect.size.height,\
           (int)new_rect.origin.x,(int)new_rect.origin.y,\
           cur_display_width,cur_display_height,\
           ""\
           );\
} while(0); }

#define WINDOW_RESIZE()\
  if((int)new_rect.origin.x != (int)focused_window_info->rect.origin.x || (int)new_rect.origin.y != (int)focused_window_info->rect.origin.y){\
    if(HOTKEY_UTILS_VERBOSE_DEBUG_MODE)\
      log_debug("Moving window prior to resize!");\
    move_window_info(focused_window_info, (int)new_rect.origin.x, (int)new_rect.origin.y);\
  }\
  bool ok = resize_window_info(focused_window_info, (int)new_rect.size.width,(int)new_rect.size.height);\
  return((ok==true) ? EXIT_SUCCESS : EXIT_FAILURE);

int decrease_focused_application_height_five_percent(){
  float width_factor = 1.00, height_factor = 0.95;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Decreasing Height")
  WINDOW_RESIZE()
}
int increase_focused_application_height_five_percent(){
  float width_factor = 1.00, height_factor = 1.05;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Increasing Height")
  WINDOW_RESIZE()
}

int increase_focused_application_width_five_percent(){
  float width_factor = 1.05, height_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Increasing Width")
  WINDOW_RESIZE()
}

int decrease_focused_application_width_five_percent(){
  float width_factor = 0.95, height_factor = 1.00;
  struct window_info_t *focused_window_info;

  focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Decreasing Width")
  WINDOW_RESIZE()
}


int decrease_focused_application_height_ten_percent(){
  float width_factor = 1.00, height_factor = 0.90;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Decreasing Height")
  WINDOW_RESIZE()
}
int increase_focused_application_height_ten_percent(){
  float width_factor = 1.00, height_factor = 1.10;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Increasing Height")
  WINDOW_RESIZE()
}

int increase_focused_application_width_ten_percent(){
  float width_factor = 1.10, height_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Increasing Width")
  WINDOW_RESIZE()
}

int decrease_focused_application_width_ten_percent(){
  float width_factor = 0.90, height_factor = 1.00;
  struct window_info_t *focused_window_info;

  focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Decreasing Width")
  WINDOW_RESIZE()
}

int left_percent_focused_application(float left_factor){
  float width_factor = left_factor, height_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor_left_side(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Left Side 50%")
  WINDOW_RESIZE()
  return(EXIT_SUCCESS);
}

#define GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(DIRECTION, FACTOR_TEXT, FACTOR_FLOAT)\
  int DIRECTION##_##FACTOR_TEXT##_percent_focused_application(){\
    return(DIRECTION##_percent_focused_application(FACTOR_FLOAT));\
  }
///////////////////////////////////////////////////////////////////
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, twenty_five, 0.25);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, twenty_five, 0.25);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, fourty, 0.40);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, fourty, 0.40);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, fifty, 0.50);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, fifty, 0.50);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, sixty, 0.60);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, sixty, 0.60);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, seventy, 0.70);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, seventy, 0.70);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, seventy_five, 0.75);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, seventy_five, 0.75);
///////////////////////////////////////////////////////////////////
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(top, twenty_five, 0.25);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(bottom, twenty_five, 0.25);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(top, fourty, 0.40);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(bottom, fourty, 0.40);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(top, fifty, 0.50);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(bottom, fifty, 0.50);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(top, sixty, 0.60);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(bottom, sixty, 0.60);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(top, seventy_five, 0.75);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(bottom, seventy_five, 0.75);
///////////////////////////////////////////////////////////////////
#undef GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION

int bottom_percent_focused_application(float bottom_factor){
  float height_factor = bottom_factor, width_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor_bottom_side(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Bottom Side 50%")
  WINDOW_RESIZE()
  return(EXIT_SUCCESS);
}

int top_percent_focused_application(float top_factor){
  float height_factor = top_factor, width_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor_top_side(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Top Side 50%")
  WINDOW_RESIZE()
  return(EXIT_SUCCESS);
}

int right_percent_focused_application(float right_factor){
  float width_factor = right_factor, height_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor_right_side(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Right Side 50%")
  WINDOW_RESIZE()
  return(EXIT_SUCCESS);
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
    if (HOTKEY_UTILS_VERBOSE_DEBUG_MODE == true) {
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
  static struct hotkeys_config_t *hotkeys_config = NULL;
  char *config_contents = fsio_read_text_file(config_file_path);
  size_t config_hash = get_config_file_hash(config_file_path);
  static size_t loaded_config_hash = 0;
  static unsigned long loaded_config_ts = 0;
  if((hotkeys_config == NULL) || (loaded_config_hash != config_hash) || ((timestamp()-loaded_config_ts) > RELOAD_CONFIG_MS)){
    if(HOTKEY_UTILS_DEBUG_MODE)
      log_info("Loading %s Config with hash %lu", bytes_to_string(strlen(config_contents)), config_hash);
    cyaml_err_t             err = cyaml_load_file(config_file_path, &config, &top_schema, (cyaml_data_t **)&hotkeys_config, NULL);
    if (err != CYAML_OK) {
      log_error("%s", cyaml_strerror(err));
      exit(EXIT_FAILURE);
    }
    loaded_config_hash = config_hash;
    loaded_config_ts = timestamp();
    if(HOTKEY_UTILS_DEBUG_MODE)
      log_info("todo app: %s | width: %d",hotkeys_config->todo_app, hotkeys_config->todo_width);
  }else{
    if(HOTKEY_UTILS_VERBOSE_DEBUG_MODE)
      log_info("Config loaded %s ago", milliseconds_to_string(timestamp()-loaded_config_ts));
  }
  return(hotkeys_config);
}

char *get_yaml_config_file_path(char **argv){
  char EXECUTABLE_PATH[PATH_MAX + 1] = { 0 };

  realpath(argv[0], EXECUTABLE_PATH);
  EXECUTABLE_PATH_DIRNAME = dirname(EXECUTABLE_PATH);
  char *path;

  asprintf(&path, "%s/../../hotkey-utils/%s", EXECUTABLE_PATH_DIRNAME, HOTKEYS_CONFIG_FILE_NAME);
  return(path);
}

bool disable_hotkey_config_key(struct key_t *key){
  log_info("disabling key %s:%s=>%s", key->name,key->key,key->action);
  return(true);
}
bool enable_hotkey_config_key(struct key_t *key){
  log_info("enabling key %s:%s=>%s", key->name,key->key,key->action);
  return(true);
}
////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__hotkey_utils(void){
  if (getenv("DEBUG") != NULL || getenv("VERBOSE_DEBUG_HOTKEY_UTILS") != NULL) {
    log_debug("Enabling hotkey-utils Verbose Debug Mode");
    HOTKEY_UTILS_DEBUG_MODE = true;
    HOTKEY_UTILS_VERBOSE_DEBUG_MODE = true;
  }else if (getenv("DEBUG") != NULL || getenv("DEBUG_HOTKEY_UTILS") != NULL) {
    log_debug("Enabling hotkey-utils Debug Mode");
    HOTKEY_UTILS_DEBUG_MODE = true;
  }
}
#endif
