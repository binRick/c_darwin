#pragma once
#include "app-utils.h"
#include "window-utils.h"
#ifndef LOGLEVEL
#define LOGLEVEL    DEFAULT_LOGLEVEL
#endif
#include "parson.h"
#include "submodules/log.h/log.h"
#ifndef APPLICATION_NAME
#define APPLICATION_NAME    "UNDEFINED"
#endif
static int emptyWindowNameAllowed(char *appName);
extern AXError _AXUIElementGetWindow(AXUIElementRef, CGWindowID *out);


uint32_t getWindowId(AXUIElementRef window) {
  CGWindowID _windowId;

  if (_AXUIElementGetWindow(window, &_windowId) == kAXErrorSuccess) {
    return(_windowId);
  }

  return(-1);
}


void MoveWindow(CFDictionaryRef window, void *ctxPtr) {
  MoveWinCtx     *ctx = (MoveWinCtx *)ctxPtr;
  int            windowId = CFDictionaryGetInt(window, kCGWindowNumber);
  CGPoint        newPosition, actualPosition;
  CGSize         newSize, actualSize;
  CGRect         displayBounds;
  AXUIElementRef appWindow = NULL;

  if (ctx->movedWindow) {
    return;
  }
  if (ctx->id != -1 && ctx->id != windowId) {
    return;
  }
  newPosition = ctx->position;
  actualSize  = CGWindowGetSize(window);
  newSize     = ctx->hasSize ? ctx->size : actualSize;
  if (ctx->fromRight || ctx->fromBottom) {
    displayBounds = CGDisplayBounds(CGMainDisplayID());
    if (ctx->fromRight) {
      newPosition.x = CGRectGetMaxX(displayBounds) -
                      (newSize.width + fabs(newPosition.x));
    }
    if (ctx->fromBottom) {
      newPosition.y = CGRectGetMaxY(displayBounds) -
                      (newSize.height + fabs(newPosition.y));
    }
  }
  actualPosition = CGWindowGetPosition(window);
  if (!CGPointEqualToPoint(newPosition, actualPosition)) {
    if (!appWindow) {
      appWindow = AXWindowFromCGWindow(window);
    }
    AXWindowSetPosition(appWindow, newPosition);
  }
  if (ctx->hasSize) {
    if (!CGSizeEqualToSize(newSize, actualSize)) {
      if (!appWindow) {
        appWindow = AXWindowFromCGWindow(window);
      }
      AXWindowSetSize(appWindow, newSize);
    }
  }
  ctx->movedWindow = 1;
} /* MoveWindow */


static int emptyWindowNameAllowed(char *appName) {
  log_debug("emptyWindowNameAllowed.......");
  return(0 == strcmp(appName, "Messages"));
}


CGPoint CGWindowGetPosition(CFDictionaryRef window) {
  log_debug("CGWindowGetPosition.......");
  CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
  int             x      = CFDictionaryGetInt(bounds, CFSTR("X"));
  int             y      = CFDictionaryGetInt(bounds, CFSTR("Y"));
  return(CGPointMake(x, y));
}


CGSize CGWindowGetSize(CFDictionaryRef window) {
  log_debug("CGWindowGetSize.......");
  CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
  int             width  = CFDictionaryGetInt(bounds, CFSTR("Width"));
  int             height = CFDictionaryGetInt(bounds, CFSTR("Height"));
  return(CGSizeMake(width, height));
}


AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window) {
  log_debug("AXWindowFromCGWindow.......");
  CGWindowID     targetWindowId, actualWindowId;
  CFStringRef    targetWindowName, actualWindowTitle;
  CGPoint        targetPosition, actualPosition;
  CGSize         targetSize, actualSize;
  pid_t          pid;
  AXUIElementRef app, appWindow, foundAppWindow;
  int            i;
  CFArrayRef     appWindowList;

  targetWindowId   = CFDictionaryGetInt(window, kCGWindowNumber);
  targetWindowName = CFDictionaryGetValue(window, kCGWindowName);
  targetPosition   = CGWindowGetPosition(window);
  targetSize       = CGWindowGetSize(window);

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
    } else {
      AXUIElementCopyAttributeValue(
        appWindow, kAXTitleAttribute, (CFTypeRef *)&actualWindowTitle
        );
      if (!actualWindowTitle
          || CFStringCompare(targetWindowName, actualWindowTitle, 0) != 0) {
        continue;
      }
      actualPosition = AXWindowGetPosition(appWindow);
      if (!CGPointEqualToPoint(targetPosition, actualPosition)) {
        continue;
      }
      actualSize = AXWindowGetSize(appWindow);
      if (!CGSizeEqualToSize(targetSize, actualSize)) {
        continue;
      }
      foundAppWindow = appWindow;
      break;
    }
  }
  CFRelease(app);

  return(foundAppWindow);
} /* AXWindowFromCGWindow */


void AXWindowGetValue(AXUIElementRef window,
                      CFStringRef    attrName,
                      void           *valuePtr) {
  AXValueRef attrValue;

  log_debug("AXWindowGetValue.......");

  AXUIElementCopyAttributeValue(window, attrName, (CFTypeRef *)&attrValue);
  AXValueGetValue(attrValue, AXValueGetType(attrValue), valuePtr);
  CFRelease(attrValue);
}


CGPoint AXWindowGetPosition(AXUIElementRef window) {
  CGPoint position;

  AXWindowGetValue(window, kAXPositionAttribute, &position);
  return(position);
}


void AXWindowSetPosition(AXUIElementRef window, CGPoint position) {
  log_debug("AXWindowSetPosition.......");
  AXValueRef attrValue = AXValueCreate(kAXValueCGPointType, &position);

  AXUIElementSetAttributeValue(window, kAXPositionAttribute, attrValue);
  CFRelease(attrValue);
}


CGSize AXWindowGetSize(AXUIElementRef window) {
  log_debug("AXWindowGetSize.......");
  CGSize size;

  AXWindowGetValue(window, kAXSizeAttribute, &size);
  return(size);
}


void AXWindowSetSize(AXUIElementRef window, CGSize size) {
  log_debug("AXWindowSetSize.......");
  AXValueRef attrValue = AXValueCreate(kAXValueCGSizeType, &size);

  AXUIElementSetAttributeValue(window, kAXSizeAttribute, attrValue);
  CFRelease(attrValue);
}


int GetWindowsQty(void){
  int        qty        = -1;
  CFArrayRef windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );

  qty = CFArrayGetCount(windowList);

  CFRelease(windowList);
  return(qty);
}


int EnumerateWindows(char *pattern,
                     void ( *callback )(CFDictionaryRef window, void *callback_data),
                     void *callback_data) {
  log_debug("EnumerateWindows.......");
  int             patternLen, subPatternLen, count, i, layer;
  char            *subPattern, *starL, *starR, *appName, *windowName, *title;
  CFArrayRef      windowList;
  CFDictionaryRef window;

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

  windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );
  count = 0;
  for (i = 0; i < CFArrayGetCount(windowList); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);

    layer = CFDictionaryGetInt(window, kCGWindowLayer);
    if (layer > 0) {
      continue;
    }

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


char *windowTitle(char *appName, char *windowName) {
  size_t titleSize;
  char   *title;

  log_info("%s> windowTitle: |app:%s|window:%s", APPLICATION_NAME, appName, windowName);

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


void PrintWindow(CFDictionaryRef window, void *ctxPtr) {
  LsWinCtx    *ctx = (LsWinCtx *)ctxPtr;
  int         windowId = CFDictionaryGetInt(window, kCGWindowNumber);
  char        *appName = CFDictionaryCopyCString(window, kCGWindowOwnerName);
  char        *windowName = CFDictionaryCopyCString(window, kCGWindowName);
  char        *title = windowTitle(appName, windowName);
  CGPoint     position = CGWindowGetPosition(window);
  CGSize      size = CGWindowGetSize(window);
  JSON_Value  *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);
  char        *serialized_string = NULL, *pretty_serialized_string = NULL;

  if (ctx->id == -1 || ctx->id == windowId) {
    if (ctx->jsonMode) {
      json_object_set_string(root_object, "appName", appName);
      json_object_set_string(root_object, "windowName", windowName);
      json_object_set_string(root_object, "title", title);
      json_object_set_number(root_object, "windowId", windowId);
      json_object_dotset_number(root_object, "size.height", (int)size.height);
      json_object_dotset_number(root_object, "size.width", (int)size.width);
      json_object_dotset_number(root_object, "position.x", (int)position.x);
      json_object_dotset_number(root_object, "position.y", (int)position.y);
      pretty_serialized_string = json_serialize_to_string_pretty(root_value);
      serialized_string        = json_serialize_to_string(root_value);
      log_info(AC_RESETALL AC_REVERSED AC_YELLOW_BLACK "%s" AC_RESETALL, pretty_serialized_string);
      fprintf(stdout, "%s", serialized_string);
    }else if (ctx->longDisplay) {
      fprintf(stdout,
              "%s - %s %d %d %d %d %d" "\n",
              title,
              appName,
              (int)windowId,
              (int)position.x, (int)position.y,
              (int)size.width, (int)size.height
              );
    }else {
      fprintf(stdout,
              "%d" "\n",
              windowId
              );
    }
    ctx->numFound++;
  }
  json_free_serialized_string(serialized_string);
  json_value_free(root_value);
  free(title);
  free(windowName);
  free(appName);
} /* PrintWindow */


int get_front_window_pid(void){
  int        pid;
  CFArrayRef windows = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
  CFIndex    i, n;

  for (i = 0, n = CFArrayGetCount(windows); i < n; i++) {
    CFDictionaryRef windict  = CFArrayGetValueAtIndex(windows, i);
    CFNumberRef     layernum = CFDictionaryGetValue(windict, kCGWindowLayer);
    CFNumberRef     pidnum   = CFDictionaryGetValue(windict, kCGWindowOwnerPID);
    if (layernum && pidnum) {
      int layer;
      CFNumberGetValue(layernum, kCFNumberIntType, &layer);
      if (layer == 0) {
        CFNumberGetValue(pidnum, kCFNumberIntType, &pid);
        CFRelease(windows);
        return(pid);
      }
    }
  }
  CFRelease(windows);
  return(-1);
}


char *cstring_from_CFString(CFStringRef cf_string) {
  CFIndex length = CFStringGetLength(cf_string);
  CFIndex size   = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
  CFIndex used_size;
  CFRange range   = CFRangeMake(0, length);
  char    *string = malloc(size);

  CFStringGetBytes(
    cf_string, range, kCFStringEncodingUTF8, '?', false, (unsigned char *)string,
    size - 1, &used_size);
  string[used_size] = '\0';
  return(string);
}


char *front_window_owner(void) {
  CFArrayRef windows = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);

  for (CFIndex idx = 0; idx < CFArrayGetCount(windows); idx++) {
    CFDictionaryRef window = CFArrayGetValueAtIndex(windows, idx);

    int64_t         window_layer;
    CFNumberRef     window_layer_ref = CFDictionaryGetValue(window, kCGWindowLayer);
    CFNumberGetValue(window_layer_ref, kCFNumberSInt64Type, &window_layer);
    CFRelease(window_layer_ref);
    if (window_layer == 0) {
      char        *window_owner;
      CFStringRef window_owner_ref = CFDictionaryGetValue(window, kCGWindowOwnerName);
      //window_owner = CFStringCopyUTF8String(window_owner_ref);
      CFRelease(window_owner_ref);
      return(window_owner);
    }
  }
  CFRelease(windows);
  return(NULL);
}
