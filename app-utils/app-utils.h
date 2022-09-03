#pragma once
#ifndef APP_UTILS_H
#define APP_UTILS_H
#include <assert.h>
#include <stdbool.h>
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
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreVideo/CVPixelBuffer.h>
///////////////////////////////////////////////////////////////////////
#include "window-utils/window-utils.h"
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
#include "iowow/src/fs/iwfile.h"
#include "iowow/src/iowow.h"
#include "iowow/src/kv/iwkv.h"
#include "iowow/src/log/iwlog.h"
#include "iowow/src/platform/iwp.h"
///////////////////////////////////////////////////////////////////////
#define APP_UTILS_STDOUT_READ_BUFFER_SIZE                   1024 * 16
#define APP_UTILS_CACHE_FILE                                ".app-utils-cache.db"
#define APP_UTILS_SYSTEM_PROFILER_JSON_CACHE_KEY            "system_profiler_json"
#define APP_UTILS_SYSTEM_PROFILER_TIMEOUT                   30
#define APP_UTILS_SYSTEM_PROFILER_DB_LOCK_MODE              IWP_RLOCK
#define APP_UTILS_SYSTEM_PROFILER_DATABASE_ID               1
#define APP_UTILS_SYSTEM_PROFILER_DETAIL_LEVEL              "mini"
#define APP_UTILS_SYSTEM_PROFILER_APPLICATIONS_DATA_TYPE    "SPApplicationsDataType"
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
char *run_system_profiler_subprocess();
///////////////////////////////////////////////////////////////////////
struct db_cache_t {
  IWDB      iwdb;
  IWKV      iwkv;
  IWKV_OPTS opts;
  iwrc      rc;
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
