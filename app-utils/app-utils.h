#pragma once
#ifndef DEFAULT_LOGLEVEL
#define DEFAULT_LOGLEVEL    2
#endif
#include "stringfn.h"
#include "window-utils.h"
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
int CFDictionaryGetInt(CFDictionaryRef dict, const void *key);
char *CFDictionaryCopyCString(CFDictionaryRef dict, const void *key);
bool isAuthorizedForScreenRecording();
bool isAuthorizedForAccessibility();

///////////////////////////////////////////////////////////////////////
#define APP_AUTHORIZATION_TESTS                                                 \
  TEST t_authorized_screen_recording(void) {                                    \
    bool OK = isAuthorizedForScreenRecording();                                 \
    log_debug("%s> isAuthorizedForScreenRecording:%d\n", APPLICATION_NAME, OK); \
    ASSERT_EQm("App not authorized for screen recording", 1, OK);               \
    /*PASS();*/                                                                 \
  }                                                                             \
  TEST t_authorized_accessibiliiy(void) {                                       \
    bool OK = isAuthorizedForAccessibility();                                   \
    log_debug("%s> isAuthorizedForAccessibility:%d\n", APPLICATION_NAME, OK);   \
    ASSERT_EQm("App not authorized for accessibility", 1, OK);                  \
    /*PASS();*/                                                                 \
  }                                                                             \
  SUITE(s_authorized) {                                                         \
    RUN_TEST(t_authorized_screen_recording);                                    \
    RUN_TEST(t_authorized_accessibiliiy);                                       \
    /*PASS();*/                                                                 \
  }                                                                             \
