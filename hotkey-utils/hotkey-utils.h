#pragma once
#ifndef HOTKEY_UTILS_H
#define HOTKEY_UTILS_H
//////////////////////////////////////
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
struct key_t {
  const char *name, *key, *action;
  bool enabled;
};
struct hotkeys_config_t {
  const char *name;
  struct key_t *keys;
  size_t    keys_count;
};
//////////////////////////////////////
char *get_yaml_config_file_path(char **argv);
struct hotkeys_config_t *load_yaml_config_file_path(char *config_file_path);
//////////////////////////////////////
#endif
