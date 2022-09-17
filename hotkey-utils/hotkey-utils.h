#pragma once
#ifndef HOTKEY_UTILS_H
#define HOTKEY_UTILS_H
//////////////////////////////////////
#include "libcyaml/include/cyaml/cyaml.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define HOTKEYS_CONFIG_FILE_NAME    "config.yaml"
enum action_type_t {
  ACTION_TYPE_NONE,
  ACTION_TYPE_ACTIVATE_APPLICATION,
  ACTION_TYPE_DEACTIVATE_APPLICATION,
  ACTION_TYPES_QTY,
};
struct action_type_handler_t {
  int (*fxn)(void *);
};
struct key_t {
  const char         *name, *key, *action;
  bool               enabled;
  enum action_type_t action_type;
};
struct hotkeys_config_t {
  const char   *name;
  struct key_t *keys;
  size_t       keys_count;
};
//////////////////////////////////////
static const cyaml_strval_t action_type_strings[] = {
  { "None",                  ACTION_TYPE_NONE                   },
  { "ActivateApplication",   ACTION_TYPE_ACTIVATE_APPLICATION   },
  { "DeactivateApplication", ACTION_TYPE_DEACTIVATE_APPLICATION },
};
//////////////////////////////////////
char *get_yaml_config_file_path(char **argv);
struct hotkeys_config_t *load_yaml_config_file_path(char *config_file_path);
int activate_application(void *APPLICATION_NAME);
int deactivate_application(void *APPLICATION_NAME);
int handle_action(enum action_type_t action_type, void *action);
struct key_t *get_hotkey_config_key(struct hotkeys_config_t *cfg, char *key);
int execute_hotkey_config_key(struct key_t *key);
//////////////////////////////////////
static struct action_type_handler_t action_type_handlers[] = {
  [ACTION_TYPE_ACTIVATE_APPLICATION]   = { .fxn = activate_application,   },
  [ACTION_TYPE_DEACTIVATE_APPLICATION] = { .fxn = deactivate_application, },
};
//////////////////////////////////////
#endif
