#pragma once
#ifndef APP_UTILS_H
#define APP_UTILS_H
///////////////////////////////////////////////////////////////////////
#include "core/core.h"
///////////////////////////////////////////////////////////////////////
#include "iowow/src/fs/iwfile.h"
#include "iowow/src/iowow.h"
#include "iowow/src/kv/iwkv.h"
#include "iowow/src/log/iwlog.h"
#include "iowow/src/platform/iwp.h"
///////////////////////////////////////////////////////////////////////
enum app_type_t {
  APP_TYPE_APPLE,
  APP_TYPE_ABSENT,
  APP_TYPE_EXISTANT,
  APP_TYPE_SYSTEM,
  APP_TYPE_SYSTEM_LIBRARY,
  APP_TYPE_USER_LIBRARY,
  APP_TYPE_APPLICATIONS,
  APP_TYPE_IDENTIFIED,
  APP_TYPE_UNIDENTIFIED,
  APP_TYPE_VERSIONED,
  APP_TYPE_UNVERSIONED,
  APP_TYPES_QTY,
};
struct app_t {
  char          *name;
  char          *version;
  char          *arch;
  char          *path;
  char          *last_modified_s;
  char          *obtained_from;
  char          *info;
  char          **signed_by;
  timelib_time  *last_modified_time;
  unsigned long last_modified_timestamp;
  bool          path_exists;
};
const char *app_type_names[APP_TYPES_QTY + 1];
///////////////////////////////////////////////////////////////////////
typedef bool (*authorized_test_function_t)(void);
typedef struct authorized_test_t    authorized_test_t;
typedef struct authorized_tests_t   authorized_tests_t;
bool is_authorized_for_screen_recording();
bool is_authorized_for_accessibility();
AXUIElementRef get_frontmost_app();
///////////////////////////////////////////////////////////////////////
struct Vector *get_running_apps_v();
struct Vector *get_installed_apps_v();
///////////////////////////////////////////////////////////////////////
typedef void (^app_parser_b)(struct app_t *app, JSON_Object *app_object);
enum app_parser_type_t {
  APP_PARSER_TYPE_NAME = 1,
  APP_PARSER_TYPE_VERSION,
  APP_PARSER_TYPE_LAST_MODIFIED,
  APP_PARSER_TYPE_LAST_MODIFIED_TIME,
  APP_PARSER_TYPE_LAST_MODIFIED_TIMESTAMP,
  APP_PARSER_TYPE_PATH,
  APP_PARSER_TYPE_OBTAINED_FROM,
  APP_PARSER_TYPE_INFO,
  APP_PARSER_TYPE_ARCH,
  APP_PARSER_TYPES_QTY,
};
struct app_parser_t {
  bool enabled;
  void (^parser)(struct app_t *app, JSON_Object *app_object);
};
struct db_cache_t {
  IWDB      iwdb;
  IWKV      iwkv;
  IWKV_OPTS opts;
};
struct authorized_tests_t {
  enum {
    AUTHORIZED_SCREEN_RECORDING,
    AUTHORIZED_ACCESSIBILITY,
    AUTHORIZED_TEST_TYPE_IDS_QTY,
  } authorized_test_type_id_t;
  struct authorized_test_t {
    int                        id;
    char                       *name;
    bool                       authorized;
    unsigned long              ts, dur_ms;
    authorized_test_function_t test_function;
  }                 authorized_test_t;
  authorized_test_t tests[AUTHORIZED_TEST_TYPE_IDS_QTY + 1];
};
///////////////////////////////////////////////////////////////////////
authorized_test_t *execute_authorization_tests();
void debug_app(struct app_t *app);
hash_t *app_utils_get_closes_app_name_matches(char *name, size_t qty);
#endif
///////////////////////////////////////////////////////////////////////
