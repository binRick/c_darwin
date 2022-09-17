#pragma once
#ifndef HOTKEY_UTILS_C
#define HOTKEY_UTILS_C
////////////////////////////////////////////
#include "hotkey-utils/hotkey-utils.h"
#include "hotkey-utils/hotkey-utils-types.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "libcyaml/include/cyaml/cyaml.h"
#include <limits.h>
#include "path_module/src/path.h"
#include <libgen.h>
////////////////////////////////////////////
static bool HOTKEY_UTILS_DEBUG_MODE = false;
static char *EXECUTABLE_PATH_DIRNAME;
///////////////////////////////////////////////////////////////////////
struct hotkeys_config_t *load_yaml_config_file_path(char *config_file_path){
  unsigned long started = timestamp();
  struct hotkeys_config_t *hotkeys_config;

  cyaml_err_t err = cyaml_load_file(config_file_path, &config, &top_schema, (cyaml_data_t **)&hotkeys_config, NULL);
  if (err != CYAML_OK) {
    log_error("%s", cyaml_strerror(err));
    return(NULL);
  }
  log_info("Loaded %lu hotkeys from Config file %s in %s",
      hotkeys_config->keys_count, config_file_path,
      milliseconds_to_string(timestamp()-started)
      );
  return(hotkeys_config);

}
char *get_yaml_config_file_path(char **argv){
  char EXECUTABLE_PATH[PATH_MAX + 1] = { 0 };
  realpath(argv[0], EXECUTABLE_PATH);
  EXECUTABLE_PATH_DIRNAME = dirname(EXECUTABLE_PATH);
  log_info("EXECUTABLE_PATH_DIRNAME:%s", EXECUTABLE_PATH_DIRNAME);
  char *path;
  asprintf(&path,"%s/../../hotkey-utils/%s",EXECUTABLE_PATH_DIRNAME,"hotkeys.yaml");
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
