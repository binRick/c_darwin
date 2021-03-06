#pragma once
#ifndef DEFAULT_LOGLEVEL
#define DEFAULT_LOGLEVEL    2
#endif
#include "stringfn.h"
#include "window-utils/window-utils.h"
#include <assert.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <fnmatch.h>
#include <fnmatch.h>
#include <libproc.h>
#include <mach/mach_time.h>
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
typedef bool (*authorized_test_function_t)(void);
typedef struct authorized_test_t    authorized_test_t;
typedef struct authorized_tests_t   authorized_tests_t;
///////////////////////////////////////////////////////////////////////
int CFDictionaryGetInt(CFDictionaryRef dict, const void *key);
char *CFDictionaryCopyCString(CFDictionaryRef dict, const void *key);

///////////////////////////////////////////////////////////////////////
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
    unsigned long              ts;
    authorized_test_function_t test_function;
  }                 authorized_test_t;
  authorized_test_t tests[AUTHORIZED_TEST_TYPE_IDS_QTY];
};
///////////////////////////////////////////////////////////////////////
authorized_test_t *execute_authorization_tests();

///////////////////////////////////////////////////////////////////////

/*is_authorized_for_screen_recording();                                 \
 *  log_debug("%s> is_authorized_for_screen_recording:%d\n", APPLICATION_NAME, OK); \
 *  ASSERT_EQm("App not authorized for screen recording", 1, OK);               \
 */
//bool OK = true;
/*is_authorized_for_accessibility();                                   \
 * log_debug("%s> is_authorized_for_accessibility:%d\n", APPLICATION_NAME, OK);   \
 * ASSERT_EQm("App not authorized for accessibility", 1, OK);                  \
 */
///////////////////////////////////////////////////////////////////////
#define APP_AUTHORIZATION_TESTS                        \
  TEST t_authorized_screen_recording(void) {           \
    bool OK = true;                                    \
    /*PASS("OK- Authorized for Screen Recording");*/   \
  }                                                    \
  TEST t_authorized_accessibiliiy(void) {              \
    bool OK = true;                                    \
    /*PASS("OK- Authorized for Accessibility");     */ \
  }                                                    \
  SUITE(s_authorized) {                                \
    RUN_TEST(t_authorized_screen_recording);           \
    RUN_TEST(t_authorized_accessibiliiy);              \
    /*PASS();*/                                        \
  }                                                    \
