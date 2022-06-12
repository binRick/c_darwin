#pragma once
#include "app-utils.h"
#ifndef LOGLEVEL
#define LOGLEVEL    DEFAULT_LOGLEVEL
#endif
#include "submodules/log.h/log.h"


int CFDictionaryGetInt(CFDictionaryRef dict, const void *key) {
  log_debug("CFDictionaryGetInt.......");
  int isSuccess, value;

  isSuccess = CFNumberGetValue(
    CFDictionaryGetValue(dict, key), kCFNumberIntType, &value
    );

  return(isSuccess ? value : 0);
}


char *CFDictionaryCopyCString(CFDictionaryRef dict, const void *key) {
  log_debug("CFDictionaryCopyCString.......");
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
  isSuccess = CFStringGetCString(
    dictValue, value, maxSize, kCFStringEncodingUTF8
    );

  return(isSuccess ? value : NULL);
}


bool isAuthorizedForScreenRecording() {
  if (MAC_OS_X_VERSION_MIN_REQUIRED < 101500) {
    return(1);
  } else {
    CGDisplayStreamFrameAvailableHandler handler =
      ^ (CGDisplayStreamFrameStatus status,
         uint64_t display_time,
         IOSurfaceRef frame_surface,
         CGDisplayStreamUpdateRef updateRef) { return; };
    CGDisplayStreamRef stream =
      CGDisplayStreamCreate(CGMainDisplayID(), 1, 1, 'BGRA', NULL, handler);
    if (stream == NULL) {
      return(0);
    } else {
      CFRelease(stream);
      return(1);
    }
  }
}


bool isAuthorizedForAccessibility() {
#if MAC_OS_X_VERSION_MIN_REQUIRED < 1090
  return(AXAPIEnabled() || AXIsProcessTrusted());
#else
  return(AXIsProcessTrusted());
#endif
}

