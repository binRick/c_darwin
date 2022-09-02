#pragma once
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "parson/parson.h"
#include "process/process.h"
#include "process/process.h"
#include "space-utils/space-utils.h"
#include "submodules/log.h/log.h"
#include "system-utils/system-utils.h"
#include "window-utils/window-utils.h"
///////////////////////////////////////////////////////////////////////////////
#define DEBUG_MODE    false

///////////////////////////////////////////////////////////////////////////////

void set_window_active_on_all_spaces(struct window_t *w){
  SLSProcessAssignToAllSpaces(g_connection, w->pid);
}

int window_layer(struct window_t *window){
  int layer = 0;

  SLSGetWindowLevel(g_connection, window->window_id, &layer);
  return(layer);
}

void window_set_layer(struct window_t *window, uint32_t layer) {
  SLSSetWindowLevel(g_connection, window->window_id, layer);
}

void window_id_send_to_space(size_t window_id, uint64_t dsid) {
  uint32_t   wid         = (uint32_t)window_id;
  CFArrayRef window_list = cfarray_of_cfnumbers(&wid, sizeof(uint32_t), 1, kCFNumberSInt32Type);

  SLSMoveWindowsToManagedSpace(g_connection, window_list, dsid);
}

void window_send_to_space(struct window_t *window, uint64_t dsid) {
  uint32_t   wid         = (uint32_t)window->window_id;
  CFArrayRef window_list = cfarray_of_cfnumbers(&wid, sizeof(uint32_t), 1, kCFNumberSInt32Type);

  SLSMoveWindowsToManagedSpace(g_connection, window_list, dsid);
  if (CGPointEqualToPoint(window->position, g_nirvana)) {
    SLSMoveWindow(g_connection, window->window_id, &g_nirvana);
  }
}

bool get_window_is_minimized(struct window_t *w){
  bool is_min = false;
  int  space_minimized_window_qty;
  int  spaces_qty = total_spaces();

  for (int s = 1; s < spaces_qty && is_min == false; s++) {
    uint32_t *minimized_window_list = space_minimized_window_list(s, &space_minimized_window_qty);
    for (int i = 0; i < space_minimized_window_qty && is_min == false; i++) {
      if (minimized_window_list[i] == (uint32_t)w->window_id) {
        is_min = true;
      }
    }
    if (minimized_window_list) {
      free(minimized_window_list);
    }
  }
  return(is_min);
}

CFStringRef get_window_role_ref(struct window_t *w){
  const void *role = NULL;

  AXUIElementCopyAttributeValue(w->window, kAXRoleAttribute, &role);
  return(role);
}

int get_window_connection_id(struct window_t *w){
  int conn;

  SLSGetConnectionIDForPSN(g_connection, &w->psn, &conn);
  return(conn);
}

bool get_window_is_popover(struct window_t *w){
  CFStringRef role = get_window_role_ref(w->window);

  if (!role) {
    return(false);
  }

  bool result = CFEqual(role, kAXPopoverRole);

  CFRelease(role);

  return(result);
}

pid_t ax_window_pid(AXUIElementRef ref){
  return(*(pid_t *)((void *)ref + 0x10));
}

uint32_t ax_window_id(AXUIElementRef ref){
  uint32_t wid = 0;

  _AXUIElementGetWindow(ref, &wid);
  return(wid);
}

void move_current_window(int center, int x, int y, int w, int h){
  AXValueRef     temp;
  AXUIElementRef current_app = get_frontmost_app();
  AXUIElementRef current_win;
  AXValueRef     tempforsize;
  CGSize         winsiz;
  CGPoint        winpos;
  int            _, dw, dh;

  get_display_bounds(&_, &_, &dw, &dh);

  AXUIElementCopyAttributeValue(current_app,
                                kAXFocusedWindowAttribute,
                                (CFTypeRef *)&current_win);

  if (center) {
    AXUIElementCopyAttributeValue(current_win,
                                  kAXSizeAttribute,
                                  (CFTypeRef *)&tempforsize);
    AXValueGetValue(tempforsize, kAXValueCGSizeType, &winsiz);
    winpos.x = (dw - winsiz.width) / 2;
    winpos.y = (dh - winsiz.height) / 2;
  } else {
    winpos.x      = x;
    winpos.y      = y;
    winsiz.width  = w;
    winsiz.height = h;
  }

  temp = AXValueCreate(kAXValueCGPointType, &winpos);
  AXUIElementSetAttributeValue(current_win, kAXPositionAttribute, temp);
  CFRelease(temp);

  temp = AXValueCreate(kAXValueCGSizeType, &winsiz);
  AXUIElementSetAttributeValue(current_win, kAXSizeAttribute, temp);
  CFRelease(temp);
}

void print_all_menu_items(FILE *rsp) {
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 110000
  if (__builtin_available(macOS 11.0, *)) {
    if (!CGRequestScreenCaptureAccess()) {
      printf("[!] Query (default_menu_items): Screen Recording "
             "Permissions not given\n");
      return;
    }
  }
#endif
  CFArrayRef window_list = CGWindowListCopyWindowInfo(kCGWindowListOptionAll,
                                                      kCGNullWindowID);
  int        window_count = CFArrayGetCount(window_list);

  fprintf(rsp, "[\n");
  int counter = 0;
  for (int i = 0; i < window_count; ++i) {
    CFDictionaryRef dictionary = CFArrayGetValueAtIndex(window_list, i);
    if (!dictionary) {
      continue;
    }

    CFStringRef owner_ref = CFDictionaryGetValue(dictionary,
                                                 kCGWindowOwnerName);

    CFNumberRef owner_pid_ref = CFDictionaryGetValue(dictionary,
                                                     kCGWindowOwnerPID);

    CFStringRef name_ref  = CFDictionaryGetValue(dictionary, kCGWindowName);
    CFNumberRef layer_ref = CFDictionaryGetValue(dictionary, kCGWindowLayer);
    if (!name_ref || !owner_ref || !owner_pid_ref || !layer_ref) {
      continue;
    }

    long long int layer = 0;
    CFNumberGetValue(layer_ref, CFNumberGetType(layer_ref), &layer);
    uint64_t      owner_pid = 0;
    CFNumberGetValue(owner_pid_ref,
                     CFNumberGetType(owner_pid_ref),
                     &owner_pid);

    char *owner = cfstring_copy(owner_ref);
    char *name  = cfstring_copy(name_ref);

    if (strcmp(name, "") != 0) {
      if (counter++ > 0) {
        fprintf(rsp, ", \n");
      }
      fprintf(rsp, "\t\"%s,%s\"", owner, name);
    }

    free(owner);
    free(name);
  }
  fprintf(rsp, "\n]\n");
  CFRelease(window_list);
} /* print_all_menu_items */

CGImageRef window_capture(struct window_t *window) {
  CGImageRef image_ref = NULL;
  uint64_t   wid       = window->window_id;

  SLSCaptureWindowsContentsToRectWithOptions(g_connection,
                                             &wid,
                                             true,
                                             CGRectNull,
                                             1 << 8,
                                             &image_ref);

  CGRect bounds;

  SLSGetScreenRectForWindow(g_connection, wid, &bounds);
  bounds.size.width = (uint32_t)(bounds.size.width + 0.5);
  window->rect.size = bounds.size;

  return(image_ref);
}

void window_move(struct window_t *window, CGPoint point) {
  window->position = point;
  SLSMoveWindow(g_connection, window->window_id, &point);

  if (__builtin_available(macOS 12.0, *)) {
  } else {
    CFNumberRef number = CFNumberCreate(NULL,
                                        kCFNumberSInt32Type,
                                        &window->window_id);

    const void *values[1] = { number };
    CFArrayRef array      = CFArrayCreate(NULL, values, 1, &kCFTypeArrayCallBacks);
    SLSReassociateWindowsSpacesByGeometry(g_connection, array);
    CFRelease(array);
    CFRelease(number);
  }
}

CFStringRef display_active_display_uuid(void) {
  return(SLSCopyActiveMenuBarDisplayIdentifier(g_connection));
}

uint32_t display_active_display_id(void) {
  uint32_t    result   = 0;
  CFStringRef uuid     = display_active_display_uuid();
  CFUUIDRef   uuid_ref = CFUUIDCreateFromString(NULL, uuid);

  result = CGDisplayGetDisplayIDFromUUID(uuid_ref);
  CFRelease(uuid_ref);
  CFRelease(uuid);
  return(result);
}

ProcessSerialNumber get_window_ProcessSerialNumber(struct window_t *w){
  ProcessSerialNumber psn = {};

  _SLPSGetFrontProcess(&psn);
  return(psn);
}

char *window_title(char *windowRef){
  char      *title = NULL;
  CFTypeRef value  = NULL;

  AXUIElementCopyAttributeValue(windowRef, kAXTitleAttribute, &value);

  if (value) {
    title = cstring_from_CFString(value);
    CFRelease(value);
  } else {
    title = cstring_from_CFString("");
  }

  return(title);
}

char *get_focused_window_title(){
  ProcessSerialNumber psn = {};

  _SLPSGetFrontProcess(&psn);
  pid_t pid;

  GetProcessPID(&psn, &pid);
  AXUIElementRef application_ref = AXUIElementCreateApplication(pid);

  if (!application_ref) {
    return(NULL);
  }
  CFTypeRef window_ref = NULL;

  AXUIElementCopyAttributeValue(application_ref, kAXFocusedWindowAttribute, &window_ref);
  if (!window_ref) {
    CFRelease(application_ref);
    return(NULL);
  }

  char      *title = NULL;
  CFTypeRef value  = NULL;

  AXUIElementCopyAttributeValue(window_ref, kAXTitleAttribute, &value);
  if (value) {
    title = cstring_from_CFString(value);
    CFRelease(value);
  }

  CFRelease(window_ref);
  CFRelease(application_ref);

  return(title);
}

CGPoint window_ax_origin(char *windowRef){
  CGPoint   origin       = {};
  CFTypeRef position_ref = NULL;

  AXUIElementCopyAttributeValue(windowRef, kAXPositionAttribute, &position_ref);

  if (position_ref) {
    AXValueGetValue(position_ref, kAXValueTypeCGPoint, &origin);
    CFRelease(position_ref);
  }

  return(origin);
}

/*
 * uint64_t display_space_id(uint32_t did){
 * CFStringRef uuid = display_uuid(did);
 *
 * if (!uuid) {
 *  return(0);
 * }
 *
 * uint64_t sid = SLSManagedDisplayGetCurrentSpace(g_connection, uuid);
 *
 * CFRelease(uuid);
 *
 * return(sid);
 * }*/

uint32_t display_manager_active_display_count(void){
  uint32_t count;

  CGGetActiveDisplayList(0, NULL, &count);
  return(count);
}

bool move_window(struct window_t *w, const int X, const int Y){
  CGPoint newPosition;

  newPosition.x = X;
  newPosition.y = Y;
  AXUIElementRef app = AXWindowFromCGWindow(w->window);

  AXWindowSetPosition(app, newPosition);
  return(true);
}

bool resize_window(struct window_t *w, const int WIDTH, const int HEIGHT){
  AXUIElementRef app       = AXWindowFromCGWindow(w->window);
  CGSize         size      = CGSizeMake(WIDTH, HEIGHT);
  AXValueRef     attrValue = AXValueCreate(kAXValueCGSizeType, &size);

  AXUIElementSetAttributeValue(app, kAXSizeAttribute, attrValue);
  return(true);
}

char *get_window_id_title(const int WINDOW_ID){
  char *window_title = malloc(128);

  sprintf(window_title, "Window #%d", WINDOW_ID);
  return(window_title);
}

void move_window_id(const int WINDOW_ID, const int X, const int Y){
  CGPoint position = CGPointMake(X, Y);

  fprintf(stderr, "moving window #%d to %fx%f\n", WINDOW_ID, position.x, position.y);
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
  return(0 == strcmp(appName, "Messages"));
}

int is_full_screen(void){
  CFArrayRef windows = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
  CFIndex    i, n;

  for (i = 0, n = CFArrayGetCount(windows); i < n; i++) {
    CFDictionaryRef windict  = CFArrayGetValueAtIndex(windows, i);
    CFNumberRef     layernum = CFDictionaryGetValue(windict, kCGWindowLayer);
    if (layernum) {
      int layer;
      CFNumberGetValue(layernum, kCFNumberIntType, &layer);
      if (layer == -1) {
        CFRelease(windows);
        return(1);
      }
    }
  }
  CFRelease(windows);
  return(0);
}

void AXWindowSetSize(AXUIElementRef window, CGSize size) {
  AXValueRef attrValue = AXValueCreate(kAXValueCGSizeType, &size);

  AXUIElementSetAttributeValue(window, kAXSizeAttribute, attrValue);
  CFRelease(attrValue);
}

int get_windows_qty(void){
  int        qty        = -1;
  CFArrayRef windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );

  qty = CFArrayGetCount(windowList);

  CFRelease(windowList);
  return(qty);
}

int EnumerateWindows(char *pattern,
                     void ( *callback )(CFDictionaryRef window, void *callback_data),
                     void *callback_data) {
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
    (kCGWindowListExcludeDesktopElements),
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
      fprintf(stderr, AC_RESETALL AC_REVERSED AC_YELLOW_BLACK "%s" AC_RESETALL, pretty_serialized_string);
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

int get_window_id_pid(int window_id){
  AXUIElementRef window;

  _AXUIElementGetWindow(window, &window_id);
  int   _window_id = CFDictionaryGetInt(window, kCGWindowNumber);
  pid_t pid        = CFDictionaryGetInt(window, kCGWindowOwnerPID);

  if (_window_id == window_id) {
    return((int)pid);
  }

  return(-1);
}

CGWindowID CGWindowWithInfo(AXUIElementRef window, CGPoint location) {
  AXError    error;
  CGWindowID windowID = kCGNullWindowID;

#ifndef APP_STORE
  error = _AXUIElementGetWindow(window, &windowID);
#else
  pid_t pid;
  error = AXUIElementGetPid(window, &pid);
  if (error != kAXErrorSuccess) {
    return(kCGNullWindowID);
  }

  CFStringRef title = NULL;
  error = AXUIElementGetTitle(window, &title);

  CGRect frame;
  error = AXUIElementGetFrame(window, &frame);
  if (error != kAXErrorSuccess) {
    return(kCGNullWindowID);
  }

  CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListExcludeDesktopElements,
                                                     kCGNullWindowID);
  if (windowList == NULL) {
    return(kCGNullWindowID);
  }

  for (CFIndex i = 0; i < CFArrayGetCount(windowList); i++) {
    CFDictionaryRef window = CFArrayGetValueAtIndex(windowList, i);

    int             windowPID;
    CFNumberGetValue(CFDictionaryGetValue(window, kCGWindowOwnerPID),
                     kCFNumberIntType,
                     &windowPID);

    if (windowPID != pid) {
      continue;
    }

    CGRect windowBounds;
    CGRectMakeWithDictionaryRepresentation(CFDictionaryGetValue(window, kCGWindowBounds),
                                           &windowBounds);

    if (!CGSizeEqualToSize(frame.size, windowBounds.size)) {
      continue;
    }

    if (!CGRectContainsPoint(windowBounds, location)) {
      continue;
    }

    CFStringRef windowName = CFDictionaryGetValue(window, kCGWindowName);
    if (windowName == NULL || title == NULL || CFStringCompare(windowName, title, 0) == kCFCompareEqualTo) {
      CFNumberGetValue(CFDictionaryGetValue(window, kCGWindowNumber),
                       kCGWindowIDCFNumberType,
                       &windowID);
      break;
    }
  }

  CFRelease(windowList);
#endif

  return(windowID);
} /* CGWindowWithInfo */

CGRect CGWindowGetBounds(CGWindowID windowID) {
  CGRect     bounds;

  CFArrayRef array = CGWindowListCopyWindowInfo(kCGWindowListOptionIncludingWindow,
                                                windowID);

  if (CFArrayGetCount(array) != 1) {
    return(CGRectMake(NAN, NAN, NAN, NAN));
  }

  CFDictionaryRef dict    = CFArrayGetValueAtIndex(array, 0);
  CFDictionaryRef _bounds = CFDictionaryGetValue(dict, kCGWindowBounds);

  CGRectMakeWithDictionaryRepresentation(_bounds, &bounds);

  CFRelease(array);

  return(bounds);
}

void _set_window_size(AXUIElementRef ax_window, CGSize *size) {
  AXValueRef ax_size = AXValueCreate(kAXValueCGSizeType, size);

  AXUIElementSetAttributeValue(ax_window, kAXSizeAttribute, ax_size);
}

void _set_window_position(AXUIElementRef ax_window, CGPoint *position) {
  AXValueRef ax_position = AXValueCreate(kAXValueCGPointType, position);
  AXError    error       = AXUIElementSetAttributeValue(ax_window, kAXPositionAttribute, ax_position);
}

CGWindowID CGWindowAtPosition(CGPoint position) {
  CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListExcludeDesktopElements,
                                                     kCGNullWindowID);

  if (windowList == NULL) {
    return(kCGNullWindowID);
  }

  CGWindowID windowID = kCGNullWindowID;

  for (CFIndex i = 0; i < CFArrayGetCount(windowList); i++) {
    CFDictionaryRef window = CFArrayGetValueAtIndex(windowList, i);

    int             layer;
    CFNumberGetValue(CFDictionaryGetValue(window, kCGWindowLayer),
                     kCFNumberIntType,
                     &layer);
    if (layer != 0) {
      continue;
    }

    CGRect windowBounds;
    CGRectMakeWithDictionaryRepresentation(CFDictionaryGetValue(window, kCGWindowBounds),
                                           &windowBounds);

    if (CGRectContainsPoint(windowBounds, position)) {
      CFNumberGetValue(CFDictionaryGetValue(window, kCGWindowNumber),
                       kCGWindowIDCFNumberType,
                       &windowID);
      break;
    }
  }

  CFRelease(windowList);

  return(windowID);
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
      window_owner = cstring_from_CFString(window_owner_ref);
      CFRelease(window_owner_ref);
      return(window_owner);
    }
  }
  CFRelease(windows);
  return(NULL);
}
