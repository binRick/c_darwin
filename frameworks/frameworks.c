#pragma once
#ifndef FRAMEWORKS_C
#define FRAMEWORKS_C
////////////////////////////////////////////

////////////////////////////////////////////
#include "frameworks/frameworks.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include <fnmatch.h>
////////////////////////////////////////////
static bool FRAMEWORKS_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__frameworks(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_FRAMEWORKS") != NULL) {
    log_debug("Enabling frameworks Debug Mode");
    FRAMEWORKS_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////

static int emptyWindowNameAllowed(char *appName);
int CFDictionaryGetInt(CFDictionaryRef dict, const void *key);
char *CFDictionaryCopyCString(CFDictionaryRef dict, const void *key);
char *windowTitle(char *appName, char *windowName);
bool isAuthorizedForScreenRecording();
bool isAuthorizedForAccessibility();
void AXWindowSetPosition(AXUIElementRef window, CGPoint position);
void AXWindowSetSize(AXUIElementRef window, CGSize size) ;
static int emptyWindowNameAllowed(char *appName) {
  return(0 == strcmp(appName, "Messages"));
}

/* Undocumented accessibility API to get window ID:
 * http://stackoverflow.com/a/10134254
 * https://github.com/jmgao/metamove/blob/master/src/window.mm
 */
extern AXError _AXUIElementGetWindow(AXUIElementRef, CGWindowID *out);


/* Search windows for match (NULL for all), run function (NULL for none) */
int EnumerateWindows(char *pattern,
                     void ( *callback )(CFDictionaryRef window, void *callback_data),
                     void *callback_data) {
  int             patternLen, subPatternLen, count, i, layer;
  char            *subPattern, *starL, *starR, *appName, *windowName, *title;
  CFArrayRef      windowList;
  CFDictionaryRef window;

  /* Add asterisks to left/right of pattern, if they are not already there */
  if (pattern && *pattern) {
    patternLen    = strlen(pattern);
    starL         = (*pattern == '*') ? "" : "*";
    starR         = (*pattern + (patternLen - 1) == '*') ? "" : "*";
    subPatternLen = patternLen + strlen(starL) + strlen(starR) + 1;
    subPattern    = (char *)malloc(subPatternLen);
    snprintf(subPattern, subPatternLen, "%s%s%s", starL, pattern, starR);
  } else {
    subPattern = pattern;
  }

  /* Iterate through list of all windows, run callback on pattern matches */
  windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );
  count = 0;
  for (i = 0; i < CFArrayGetCount(windowList); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);

    /* Skip windows that are not on the desktop layer */
    layer = CFDictionaryGetInt(window, kCGWindowLayer);
    if (layer > 0) {
      continue;
    }

    /* Turn application name and title into string to match against */
    appName = windowName = title = NULL;
    appName = CFDictionaryCopyCString(window, kCGWindowOwnerName);
    if (!appName || !*appName) {
      goto skip;
    }
    windowName = CFDictionaryCopyCString(window, kCGWindowName);
    if (!windowName || (!*windowName && !emptyWindowNameAllowed(appName))) {
      goto skip;
    }
    title = windowTitle(appName, windowName);

    /* If no pattern, or pattern matches, run callback */
    if (!pattern || fnmatch(subPattern, title, 0) == 0) {
      if (callback) {
        (*callback)(window, callback_data);
      }
      count++;
    }

skip:
    if (title) {
      free(title);
    }
    if (windowName) {
      free(windowName);
    }
    if (appName) {
      free(appName);
    }
  }
  CFRelease(windowList);
  if (subPattern != pattern) {
    free(subPattern);
  }

  return(count);
} /* EnumerateWindows */


/* Fetch an integer value from a CFDictionary */
int CFDictionaryGetInt(CFDictionaryRef dict, const void *key) {
  int isSuccess, value;

  isSuccess = CFNumberGetValue(
    CFDictionaryGetValue(dict, key), kCFNumberIntType, &value
    );

  return(isSuccess ? value : 0);
}


/* Copy a string value from a CFDictionary into a newly allocated string */
char *CFDictionaryCopyCString(CFDictionaryRef dict, const void *key) {
  const void *dictValue;
  CFIndex    length;
  int        maxSize, isSuccess;
  char       *value;

  dictValue = CFDictionaryGetValue(dict, key);
  if (dictValue == NULL) {
    return(NULL);
  }

  /* If empty value, allocate and return empty string */
  length  = CFStringGetLength(dictValue);
  maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
  if (length == 0 || maxSize == 0) {
    value  = (char *)malloc(1);
    *value = '\0';
    return(value);
  }

  /* Otherwise, allocate string and copy value into it */
  value     = (char *)malloc(maxSize);
  isSuccess = CFStringGetCString(
    dictValue, value, maxSize, kCFStringEncodingUTF8
    );

  return(isSuccess ? value : NULL);
}


/* Return newly allocated window title like "appName - windowName" */
char *windowTitle(char *appName, char *windowName) {
  size_t titleSize;
  char   *title;

  if (!appName || !*appName) {
    title  = (char *)malloc(1);
    *title = '\0';
  } else if (!windowName || !*windowName) {
    titleSize = strlen(appName) + 1;
    title     = (char *)malloc(titleSize);
    strncpy(title, appName, titleSize);
  } else {
    titleSize = strlen(appName) + strlen(" - ") + strlen(windowName) + 1;
    title     = (char *)malloc(titleSize);
    snprintf(title, titleSize, "%s - %s", appName, windowName);
  }

  return(title);
}


/* Given window dictionary from CGWindowList, return position */
CGPoint CGWindowGetPosition(CFDictionaryRef window) {
  CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
  int             x      = CFDictionaryGetInt(bounds, CFSTR("X"));
  int             y      = CFDictionaryGetInt(bounds, CFSTR("Y"));

  return(CGPointMake(x, y));
}


/* Given window dictionary from CGWindowList, return size */
CGSize CGWindowGetSize(CFDictionaryRef window) {
  CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
  int             width  = CFDictionaryGetInt(bounds, CFSTR("Width"));
  int             height = CFDictionaryGetInt(bounds, CFSTR("Height"));

  return(CGSizeMake(width, height));
}


/* Return true if and only if we are authorized to do screen recording */
bool isAuthorizedForScreenRecording() {
  if (MAC_OS_X_VERSION_MIN_REQUIRED < 101500) {
    /* OS X prior to Catalina does not require separate permissions */
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


/* Return true if and only if we are authorized to call accessibility APIs */
bool isAuthorizedForAccessibility() {
#if MAC_OS_X_VERSION_MIN_REQUIRED < 1090
  return(AXAPIEnabled() || AXIsProcessTrusted());
#else
  /* Mavericks and later have only per-process accessibility permissions */
  return(AXIsProcessTrusted());
#endif
}

/* Silence warning that address of _AXUIElementGetWindow is always true */
#pragma GCC diagnostic ignored "-Waddress"


/* Given window dictionary from CGWindowList, return accessibility object */
AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window) {
  CGWindowID     targetWindowId, actualWindowId;
  CFStringRef    targetWindowName, actualWindowTitle;
  pid_t          pid;
  AXUIElementRef app, appWindow, foundAppWindow = NULL;
  int            i;
  CFArrayRef     appWindowList;

  targetWindowId   = CFDictionaryGetInt(window, kCGWindowNumber);
  targetWindowName = CFDictionaryGetValue(window, kCGWindowName);

  pid = CFDictionaryGetInt(window, kCGWindowOwnerPID);
  app = AXUIElementCreateApplication(pid);
  AXUIElementCopyAttributeValue(
    app, kAXWindowsAttribute, (CFTypeRef *)&appWindowList
    );

  foundAppWindow = NULL;
  for (i = 0; i < CFArrayGetCount(appWindowList); i++) {
    appWindow = CFArrayGetValueAtIndex(appWindowList, i);
    if (_AXUIElementGetWindow) {
      _AXUIElementGetWindow(appWindow, &actualWindowId);
      if (actualWindowId == targetWindowId) {
        foundAppWindow = appWindow;
        break;
      } else {
        continue;
      }
    }
  }
  CFRelease(app);

  return(foundAppWindow);
} /* AXWindowFromCGWindow */


/* Get a value from an accessibility object */
void AXWindowGetValue(AXUIElementRef window,
                      CFStringRef    attrName,
                      void           *valuePtr) {
  AXValueRef attrValue;

  AXUIElementCopyAttributeValue(window, attrName, (CFTypeRef *)&attrValue);
  AXValueGetValue(attrValue, AXValueGetType(attrValue), valuePtr);
  CFRelease(attrValue);
}


/* Get position of window via accessibility object */

/* Set position of window via accessibility object */
void AXWindowSetPosition(AXUIElementRef window, CGPoint position) {
  AXValueRef attrValue = AXValueCreate(kAXValueCGPointType, &position);

  AXUIElementSetAttributeValue(window, kAXPositionAttribute, attrValue);
  CFRelease(attrValue);
}



/* Set size of window via accessibility object */
void AXWindowSetSize(AXUIElementRef window, CGSize size) {
  AXValueRef attrValue = AXValueCreate(kAXValueCGSizeType, &size);

  AXUIElementSetAttributeValue(window, kAXSizeAttribute, attrValue);
  CFRelease(attrValue);
}


#endif
