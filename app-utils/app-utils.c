#pragma once
#define DEBUG_MODE_ENABLED    false
#include "app-utils/app-utils.h"
#include "timestamp/timestamp.h"
///////////////////////////////////////////////////////////////////////
static bool is_authorized_for_screen_recording();
static bool is_authorized_for_accessibility();

///////////////////////////////////////////////////////////////////////
static authorized_tests_t authorized_tests = {
  .tests             = {
    [AUTHORIZED_ACCESSIBILITY] =    {
      .id            = AUTHORIZED_ACCESSIBILITY,
      .name          = "accessibility",
      .test_function = (is_authorized_for_accessibility),
      .authorized    = false,
      .ts            = false,
    },
    [AUTHORIZED_SCREEN_RECORDING] = {
      .id            = AUTHORIZED_SCREEN_RECORDING,
      .name          = "screen_recording",
      .test_function = (is_authorized_for_screen_recording),
      .authorized    = false,
      .ts            = false,
    },
  },
};


///////////////////////////////////////////////////////////////////////


static authorized_test_t execute_authorization_test(int authorized_test_type_id){
  authorized_test_t authorized_test = authorized_tests.tests[authorized_test_type_id];

  authorized_test.ts            = timestamp();
  authorized_test.authorized    = authorized_test.test_function();
  authorized_test.test_function = NULL;
  return(authorized_test);
}


authorized_test_t *execute_authorization_tests(){
  authorized_test_t *authorized_test_results = calloc(AUTHORIZED_TEST_TYPE_IDS_QTY, sizeof(authorized_test_t));

  for (int i = 0; i < AUTHORIZED_TEST_TYPE_IDS_QTY; i++) {
    if (DEBUG_MODE_ENABLED) {
      fprintf(stderr, "Running Test #%d\n", i);
    }
    authorized_test_results[i] = (execute_authorization_test(i));
    if (DEBUG_MODE_ENABLED) {
      fprintf(stderr, "Ran Test #%d (%s) with result '%d' @ %lu.\n",
              i,
              authorized_test_results[i].name,
              authorized_test_results[i].authorized,
              authorized_test_results[i].ts
              );
    }
  }
  return(authorized_test_results);
}


static bool is_authorized_for_screen_recording() {
  CGDisplayStreamFrameAvailableHandler handler = ^ (CGDisplayStreamFrameStatus status, uint64_t display_time, IOSurfaceRef frame_surface, CGDisplayStreamUpdateRef updateRef){
    return;
  };
  CGDisplayStreamRef stream = CGDisplayStreamCreate(CGMainDisplayID(), 1, 1, 'BGRA', NULL, handler);

  if (stream == NULL) {
    return(false);
  } else {
    CFRelease(stream);
    return(true);
  }
}


static bool is_authorized_for_accessibility() {
#if MAC_OS_X_VERSION_MIN_REQUIRED < 1090
  return(AXAPIEnabled() || AXIsProcessTrusted());
#else
  return(AXIsProcessTrusted());
#endif
}


int CFDictionaryGetInt(CFDictionaryRef dict, const void *key) {
  int value;

  return(CFNumberGetValue(CFDictionaryGetValue(dict, key), kCFNumberIntType, &value) ? value : 0);
}


char *CFDictionaryCopyCString(CFDictionaryRef dict, const void *key) {
  const void *dictValue;
  CFIndex    length;
  int        maxSize, isSuccess;
  char       *value;

  if (DEBUG_MODE_ENABLED) {
    fprintf(stderr, "CFDictionaryCopyCString.......|key='%s'|\n", (char *)key);
  }
  dictValue = CFDictionaryGetValue(dict, key);
  if (DEBUG_MODE_ENABLED) {
    fprintf(stderr, "CFDictionaryCopyCString.......|value null? '%s'|\n", value == NULL ? "yes" : "no");
  }
  if (dictValue == NULL) {
    return(NULL);
  }
  length  = CFStringGetLength(dictValue);
  maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
  if (length == 0 || maxSize == 0) {
    value  = (char *)malloc(1);
    *value = '\0';
    return(value);
  }

  value     = (char *)malloc(maxSize);
  isSuccess = CFStringGetCString(
    dictValue, value, maxSize, kCFStringEncodingUTF8
    );

  if (DEBUG_MODE_ENABLED) {
    fprintf(stderr, "CFDictionaryCopyCString.......|'%s'->'%s'|\n", (char *)key, (char *)value);
  }
  return((isSuccess) ? (value) : (NULL));
}

