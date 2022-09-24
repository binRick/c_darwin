#pragma once
#ifndef APP_UTILS_H
#define APP_UTILS_H
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
///////////////////////////////////////////////////////////////////////
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreVideo/CVPixelBuffer.h>
///////////////////////////////////////////////////////////////////////
#include "window/utils/utils.h"
///////////////////////////////////////////////////////////////////////
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "subprocess.h/subprocess.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "which/src/which.h"
///////////////////////////////////////////////////////////////////////
#include "app/app.h"
#include "iowow/src/fs/iwfile.h"
#include "iowow/src/iowow.h"
#include "iowow/src/kv/iwkv.h"
#include "iowow/src/log/iwlog.h"
#include "iowow/src/platform/iwp.h"
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
#endif
///////////////////////////////////////////////////////////////////////
