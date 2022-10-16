#pragma once
#ifndef HOTKEY_UTILS_TYPES_H
#define HOTKEY_UTILS_TYPES_H
//////////////////////////////////////
#include "hotkey-utils/hotkey-utils.h"
#include "libcyaml/include/cyaml/cyaml.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define KEY_FIELDS_QTY               5
#define KEY_SCHEMA_QTY               1
#define HOTKEYS_CONFIG_SCHEMA_QTY    3
//////////////////////////////////////
const cyaml_schema_field_t app_fields_schema[] = {
  CYAML_FIELD_STRING_PTR("name", CYAML_FLAG_POINTER, struct hk_app_t, name, 0, CYAML_UNLIMITED),
  CYAML_FIELD_END,
};
const cyaml_schema_value_t app_schema = {
  CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, struct hk_app_t, app_fields_schema),
};
const cyaml_schema_field_t layout_fields_schema[] = {
  CYAML_FIELD_STRING_PTR("name", CYAML_FLAG_POINTER, struct hk_layout_t, name,     0,           CYAML_UNLIMITED),
  CYAML_FIELD_STRING_PTR("app",  CYAML_FLAG_POINTER, struct hk_layout_t, app,      0,           CYAML_UNLIMITED),
  CYAML_FIELD_STRING_PTR("mode", CYAML_FLAG_POINTER, struct hk_layout_t, mode,     0,           CYAML_UNLIMITED),
  CYAML_FIELD_INT("display",     CYAML_FLAG_DEFAULT, struct hk_layout_t, display),
  CYAML_FIELD_INT("display_space",     CYAML_FLAG_DEFAULT, struct hk_layout_t, display_space),
  CYAML_FIELD_FLOAT("width",     CYAML_FLAG_DEFAULT, struct hk_layout_t, width),
  CYAML_FIELD_BOOL("debug",      CYAML_FLAG_DEFAULT, struct hk_layout_t, debug),
  CYAML_FIELD_SEQUENCE("apps",   CYAML_FLAG_POINTER, struct hk_layout_t, apps,     &app_schema, 0,               CYAML_UNLIMITED),
  CYAML_FIELD_END,
};
const cyaml_schema_value_t layout_schema = {
  CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, struct hk_layout_t, layout_fields_schema),
};
const cyaml_schema_field_t key_fields_schema[] = {
  CYAML_FIELD_STRING_PTR("name",   CYAML_FLAG_POINTER, struct key_t, name,        0,                   CYAML_UNLIMITED),
  CYAML_FIELD_STRING_PTR("key",    CYAML_FLAG_POINTER, struct key_t, key,         0,                   CYAML_UNLIMITED),
  CYAML_FIELD_STRING_PTR("action", CYAML_FLAG_POINTER, struct key_t, action,      0,                   CYAML_UNLIMITED),
  CYAML_FIELD_ENUM("action_type",  CYAML_FLAG_DEFAULT, struct key_t, action_type, action_type_strings, CYAML_ARRAY_LEN(action_type_strings)),
  CYAML_FIELD_BOOL("enabled",      CYAML_FLAG_DEFAULT, struct key_t, enabled),
  CYAML_FIELD_END,
};
const cyaml_schema_value_t key_schema = {
  CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, struct key_t, key_fields_schema),
};
const cyaml_schema_field_t hotkeys_config_schema[] = {
  CYAML_FIELD_STRING_PTR("name",     CYAML_FLAG_POINTER, struct hotkeys_config_t, name,        0,              CYAML_UNLIMITED),
  CYAML_FIELD_INT("todo_width",      CYAML_FLAG_DEFAULT, struct hotkeys_config_t, todo_width),
  CYAML_FIELD_STRING_PTR("todo_app", CYAML_FLAG_POINTER, struct hotkeys_config_t, todo_app,    0,              CYAML_UNLIMITED),
  CYAML_FIELD_SEQUENCE("keys",       CYAML_FLAG_POINTER, struct hotkeys_config_t, keys,        &key_schema,    0,               CYAML_UNLIMITED),
  CYAML_FIELD_SEQUENCE("layouts",    CYAML_FLAG_POINTER, struct hotkeys_config_t, layouts,     &layout_schema, 0,               CYAML_UNLIMITED),
  CYAML_FIELD_END,
};
const cyaml_schema_value_t top_schema = {
  CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct hotkeys_config_t, hotkeys_config_schema),
};
const cyaml_config_t       config = {
  .log_fn    = cyaml_log,
  .mem_fn    = cyaml_mem,
  .log_level = CYAML_LOG_WARNING,
};
#endif
