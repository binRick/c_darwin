#pragma once
#include "app-utils/app-utils.h"
#include "timestamp/timestamp.h"
#include <ApplicationServices/ApplicationServices.h>
#include <CoreVideo/CVPixelBuffer.h>
#include <stdbool.h>
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
authorized_tests_t authorized_tests = {
  .tests             = {
    [AUTHORIZED_ACCESSIBILITY] =     {
      .id            = AUTHORIZED_ACCESSIBILITY,
      .name          = "accessibility",
      .test_function = (*is_authorized_for_accessibility),
      .authorized    = false,                             .ts= 0, .dur_ms = 0,
    },
    [AUTHORIZED_SCREEN_RECORDING] =  {
      .id            = AUTHORIZED_SCREEN_RECORDING,
      .name          = "screen_recording",
      .test_function = (*is_authorized_for_screen_recording),
      .authorized    = false,                                .ts= 0, .dur_ms = 0,
    },
    [AUTHORIZED_TEST_TYPE_IDS_QTY] = { 0 },
  },
};
///////////////////////////////////////////////////////////////////////

bool request_accessibility_permissions() {
  bool                       result          = false;
  CFDictionaryKeyCallBacks   key_callbacks   = kCFTypeDictionaryKeyCallBacks;
  CFDictionaryValueCallBacks value_callbacks = kCFTypeDictionaryValueCallBacks;
  CFStringRef                key             = kAXTrustedCheckOptionPrompt;
  CFBooleanRef               value           = kCFBooleanTrue;
  CFDictionaryRef            options         = CFDictionaryCreate(
    kCFAllocatorDefault, (CFTypeRef *)&key, (CFTypeRef *)&value, 1,
    &key_callbacks, &value_callbacks);

  result = AXIsProcessTrustedWithOptions(options);
  CFRelease(key);
  CFRelease(value);
  CFRelease(options);
  return(result);
}

///////////////////////////////////////////////////////////////////////

authorized_test_t execute_authorization_test(int authorized_test_type_id){
  authorized_test_t authorized_test = authorized_tests.tests[authorized_test_type_id];

  authorized_test.ts            = timestamp();
  authorized_test.authorized    = authorized_test.test_function();
  authorized_test.test_function = NULL;
  return(authorized_test);
}

authorized_test_t *execute_authorization_tests(){
  authorized_test_t *authorized_test_results = calloc(AUTHORIZED_TEST_TYPE_IDS_QTY, sizeof(authorized_test_t));

  for (int i = 0; i < AUTHORIZED_TEST_TYPE_IDS_QTY; i++) {
    unsigned long started = timestamp();
    authorized_test_results[i]        = (execute_authorization_test(i));
    authorized_test_results[i].dur_ms = timestamp() - started;
  }
  return(authorized_test_results);
}

bool request_screen_recording_permissions() {
  CGDisplayStreamRef stream = NULL;

  stream = CGDisplayStreamCreate(CGMainDisplayID(), 1, 1, kCVPixelFormatType_32BGRA, nil,
                                 ^ (CGDisplayStreamFrameStatus status, uint64_t displayTime, IOSurfaceRef frameSurface,
                                    CGDisplayStreamUpdateRef updateRef){});
  if (stream == NULL) {
    return(false);
  }
  CFRelease(stream);
  return(true);
}

bool is_authorized_for_screen_recording() {
  CGDisplayStreamFrameAvailableHandler handler = ^ (CGDisplayStreamFrameStatus status, uint64_t display_time, IOSurfaceRef frame_surface, CGDisplayStreamUpdateRef updateRef){ return; };
  CGDisplayStreamRef                   stream = CGDisplayStreamCreate(CGMainDisplayID(), 1, 1, 'BGRA', NULL, handler);

  if (stream == NULL) {
    return(false);
  }
  CFRelease(stream);
  return(true);
}

bool is_authorized_for_accessibility() {
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

  dictValue = CFDictionaryGetValue(dict, key);
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
  isSuccess = CFStringGetCString(dictValue, value, maxSize, kCFStringEncodingUTF8);

  return((isSuccess ? value : NULL));
}
