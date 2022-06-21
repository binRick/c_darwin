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

  log_debug("%s> windowTitle: |app:%s|window:%s", APPLICATION_NAME, appName, windowName);

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
  log_debug("PrintWindow..........");
  LsWinCtx    *ctx                      = (LsWinCtx *)ctxPtr;
  int         windowId                  = CFDictionaryGetInt(window, kCGWindowNumber);
  char        *appName                  = CFDictionaryCopyCString(window, kCGWindowOwnerName);
  char        *windowName               = CFDictionaryCopyCString(window, kCGWindowName);
  char        *title                    = windowTitle(appName, windowName);
  CGPoint     position                  = CGWindowGetPosition(window);
  CGSize      size                      = CGWindowGetSize(window);
  JSON_Value  *root_value               = json_value_init_object();
  JSON_Object *root_object              = json_value_get_object(root_value);
  char        *serialized_string        = NULL;
  char        *pretty_serialized_string = NULL;

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
  log_debug(
    AC_RESETALL AC_REVERSED AC_YELLOW_BLACK "%s" AC_RESETALL,
    pretty_serialized_string
    );
  if (ctx->id == -1 || ctx->id == windowId) {
    if (ctx->jsonMode) {
      log_debug("%s", serialized_string);
    }else if (ctx->longDisplay) {
      fprintf(stderr,
              "%s - %s %d %d %d %d %d\n", title,
              appName,
              (int)windowId,
              (int)position.x, (int)position.y,
              (int)size.width, (int)size.height
              );
    }else {
      printf("%d\n", windowId);
    }
    ctx->numFound++;
  }
  json_free_serialized_string(serialized_string);
  json_value_free(root_value);
  free(title);
  free(windowName);
  free(appName);
} /* PrintWindow */
