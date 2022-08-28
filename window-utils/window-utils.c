#pragma once
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "core-utils/core-utils.h"
#include "parson/parson.h"
#include "process/process.h"
#include "submodules/log.h/log.h"
#include "system-utils/system-utils.h"
#include "window-utils/window-utils.h"
///////////////////////////////////////////////////////////////////////////////
#define DEBUG_MODE      false
///////////////////////////////////////////////////////////////////////////////
static bool window_is_focused(struct window_t *w);
bool get_window_is_visible(struct window_t *window);
ProcessSerialNumber get_window_ProcessSerialNumber(struct window_t *w);
int get_window_id_pid(int window_id);

  int get_window_id_space_id(int window_id){
    int space_id = -1;
    int space_cnt = total_spaces() + 1;
      for(int i=1;i<space_cnt && space_id == -1;i++){
        int window_count;
      uint32_t *window_list         = space_window_list(i, &window_count, true);
        for(int ii=0;ii<window_count && space_id == -1;ii++){
          if(window_list[ii] == (uint32_t)window_id){
            space_id = i;
          }
        }
      }
      return(space_id);
    }

window_t *get_window_id(const int WINDOW_ID){
  int      focused_pid = get_frontmost_application();
  window_t *w          = malloc(sizeof(window_t));
  w->window_id = WINDOW_ID;
  w->window    = window_id_to_window(w->window_id);
  w->pid = CFDictionaryGetInt(w->window, kCGWindowOwnerPID);
  w->app = AXUIElementCreateApplication(w->pid);
  w->psn = PID2PSN(w->pid);
  SLSGetConnectionIDForPSN(g_connection, &w->psn, &w->connection_id);
  w->app_name     = stringfn_trim(CFDictionaryCopyCString(w->window, kCGWindowOwnerName));
  w->window_name  = stringfn_trim(CFDictionaryCopyCString(w->window, kCGWindowName));
  w->display_id   = get_window_display_id(w);
  w->display_uuid = get_window_display_uuid(w);
  w->space_id     = get_window_id_space_id(w->window_id);
  w->window_title = stringfn_trim(windowTitle(w->app_name, w->window_name));
  w->layer        = CFDictionaryGetInt(w->window, kCGWindowLayer);
  w->position     = CGWindowGetPosition(w->window);
  w->size         = CGWindowGetSize(w->window);
  // w->connection_id = get_window_connection_id(w);
  w->rect         = CGRectMake(w->position.x, w->position.y, w->size.width, w->size.height);
  w->width        = (int)(w->size.width);
  w->height       = (int)(w->size.height);
  w->is_minimized = get_window_is_minimized(w);
  w->is_focused   = (focused_pid == w->pid) ? true : false;
  w->is_visible   = get_window_is_visible(w);
  get_kinfo_proc(w->pid, &w->pid_info);

  return(w);
}
bool get_window_is_visible(struct window_t *window){
  bool result = true;

  if (window->position.x == 0 && window->position.y == 25 && window->layer == 0) {
    result = true;
  }

  return(result);
}

bool get_window_is_minimized(struct window_t *window){
  bool      result = false;
  CFTypeRef value;

  if (AXUIElementCopyAttributeValue(window->window, kAXMinimizedAttribute, &value) == kAXErrorSuccess) {
    result = CFBooleanGetValue(value);
    fprintf(stderr, "checking if window #%d is min- %d\n", window->window_id, result);
    CFRelease(value);
  }

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

window_t *get_focused_window(){
  int             pid       = get_focused_pid();
  int             window_id = get_pid_window_id(pid);
  struct window_t *w        = get_window_id(window_id);

  return(w);
}

window_t *get_pid_window(const int PID){
  struct Vector   *windows = get_windows();
  struct window_t *w = NULL, *_w = NULL;

  for (size_t i = 0; i < vector_size(windows); i++) {
    window_t *w = vector_get(windows, i);
    _w = (struct window_t *)vector_get(windows, i);
    if (w->pid == PID) {
      w = _w;
    }else{
      free(w);
    }
  }
  return(w);
}

int get_display_width(){
  int    w             = -1;
  CGRect displayBounds = CGDisplayBounds(CGMainDisplayID());

  w = displayBounds.size.width;
  return(w);
}

int get_window_connection_id(struct window_t *w){
  int conn;

  SLSGetConnectionIDForPSN(g_connection, &w->psn, &conn);
  return(conn);
}

CFStringRef display_uuid(uint32_t did){
  CFUUIDRef uuid_ref = CGDisplayCreateUUIDFromDisplayID(did);

  if (!uuid_ref) {
    return(NULL);
  }

  CFStringRef uuid_str = CFUUIDCreateString(NULL, uuid_ref);

  CFRelease(uuid_ref);

  return(uuid_str);
}

uint32_t display_id(CFStringRef uuid){
  CFUUIDRef uuid_ref = CFUUIDCreateFromString(NULL, uuid);

  if (!uuid_ref) {
    return(0);
  }

  uint32_t did = CGDisplayGetDisplayIDFromUUID(uuid_ref);

  CFRelease(uuid_ref);

  return(did);
}

struct Vector *get_space_window_ids_v(uint64_t space_id){
  struct Vector *ids       = vector_new();
  uint64_t      set_tags   = 0;
  uint64_t      clear_tags = 0;
  uint64_t      _space_id  = (uint64_t)space_id;
  uint64_t      sid        = &_space_id;

//    CFArrayRef space_list_ref = cfarray_from_numbers(&sid, sizeof(uint64_t), 1, kCFNumberSInt32Type);
//    CFArrayRef window_list_ref = SLSCopyWindowsWithOptionsAndTags(g_connection, 0, space_list_ref, 0x2, &set_tags, &clear_tags);

  //  CFRelease(space_list_ref);
  return(ids);
}

struct Vector *get_display_id_space_ids_v(uint32_t did){
  struct Vector *display_space_ids = vector_new();
  CFStringRef   uuid               = display_uuid(did);

  if (!uuid) {
    goto out;
  }
  CFArrayRef display_spaces_ref = SLSCopyManagedDisplaySpaces(g_connection);

  if (!display_spaces_ref) {
    goto err;
  }

  int display_spaces_count = CFArrayGetCount(display_spaces_ref);

  for (int i = 0; i < display_spaces_count; ++i) {
    CFDictionaryRef display_ref = CFArrayGetValueAtIndex(display_spaces_ref, i);
    CFStringRef     identifier  = CFDictionaryGetValue(display_ref, CFSTR("Display Identifier"));
    if (!CFEqual(uuid, identifier)) {
      continue;
    }

    CFArrayRef spaces_ref = CFDictionaryGetValue(display_ref, CFSTR("Spaces"));
    int        qty        = CFArrayGetCount(spaces_ref);
    for (int j = 0; j < qty; ++j) {
      CFDictionaryRef space_ref = CFArrayGetValueAtIndex(spaces_ref, j);
      CFNumberRef     sid_ref   = CFDictionaryGetValue(space_ref, CFSTR("id64"));
      int             ij        = 0;
      CFNumberGetValue(sid_ref, CFNumberGetType(CFDictionaryGetValue(space_ref, CFSTR("id64"))), &ij);
      vector_push(display_space_ids, (void *)(uint64_t)ij);
    }
  }

  CFRelease(display_spaces_ref);
err:
  CFRelease(uuid);
out:
  return(display_space_ids);
} /* display_space_list */

char *get_window_display_uuid(struct window_t *window){
  CFStringRef _uuid = SLSCopyManagedDisplayForWindow(CGSMainConnectionID(), window->window_id);
  char        *uuid = cstring_from_CFString(_uuid);

  CFRelease(_uuid);
  return(uuid);
}

int get_window_display_id(struct window_t *window){
  CFStringRef _uuid = SLSCopyManagedDisplayForWindow(CGSMainConnectionID(), window->window_id);
  CFUUIDRef   uuid  = CFUUIDCreateFromString(NULL, _uuid);

  CFRelease(_uuid);
  int id = CGDisplayGetDisplayIDFromUUID(uuid);

  CFRelease(uuid);

  return(id);
}

struct Vector *get_connection_window_ids(int conn){
  struct Vector *ids       = vector_new();
  CFArrayRef    spaces_ref = SLSCopyManagedDisplaySpaces(conn);
  int           spaces_qty = CFArrayGetCount(spaces_ref);

  //log_info("Conn #%d has %d spaces", conn, spaces_qty);

  return(ids);
}
//CFStringRef string = CFStringCreateWithCString(NULL, cstring, kCFStringEncodingUTF8);

struct Vector *get_window_space_ids_v(struct window_t *w){
  struct Vector *window_space_ids = vector_new();

  return(window_space_ids);
}

struct Vector *get_windows(){
  struct Vector *windows    = vector_new();
  struct Vector *window_ids = get_window_ids();

  for (size_t i = 0; i < vector_size(window_ids); i++) {
    size_t   window_id = (size_t)vector_get(window_ids, i);
    window_t *w        = get_window_id(window_id);
    if (w == NULL) {
      continue;
    }
//  struct Vector *display_space_ids_v = get_display_id_space_ids_v(w->display_id);
//  fprintf(stderr,"display #%d  has %lu spaces spaces:\n",w->display_id,vector_size(display_space_ids_v));
/*
 *  w->space_ids_v = get_window_space_ids_v(w);
 *  for(size_t ii=0;ii<vector_size(w->space_ids_v);ii++){
 *    fprintf(stderr,"      #%lu\n",(size_t)vector_get(w->space_ids_v,ii));
 *  }*/
    vector_push(windows, (window_t *)w);
  }

  return(windows);
}

CGRect display_bounds(uint32_t did){
  return(CGDisplayBounds(did));
}

uint32_t display_manager_main_display_id(void){
  return(CGMainDisplayID());
}

CFStringRef display_manager_active_display_uuid(void){
  return(SLSCopyActiveMenuBarDisplayIdentifier(g_connection));
}

uint32_t display_manager_active_display_id(void){
  CFStringRef uuid = display_manager_active_display_uuid();

  assert(uuid);

  uint32_t result = display_id(uuid);

  CFRelease(uuid);

  return(result);
}

static bool window_is_focused(struct window_t *w){
  return(true);
}

bool display_manager_dock_hidden(void){
  return(CoreDockGetAutoHideEnabled());
}

int display_manager_dock_orientation(void){
  int pinning     = 0;
  int orientation = 0;

  CoreDockGetOrientationAndPinning(&orientation, &pinning);
  return(orientation);
}

CGRect display_manager_dock_rect(void){
  int    reason = 0;
  CGRect bounds = {};

  SLSGetDockRectWithReason(g_connection, &bounds, &reason);
  return(bounds);
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

uint32_t *display_manager_active_display_list(uint32_t *count){
  int      display_count = display_manager_active_display_count();
  uint32_t *result       = ts_alloc_aligned(sizeof(uint32_t), display_count);

  CGGetActiveDisplayList(display_count, result, count);
  return(result);
}


bool move_window(CFDictionaryRef w, const int X, const int Y){
  AXUIElementRef app = AXWindowFromCGWindow(w);
  CGPoint        newPosition;

  newPosition.x = X;
  newPosition.y = Y;
  AXWindowSetPosition(app, newPosition);
  return(true);
}

bool resize_window(CFDictionaryRef w, const int WIDTH, const int HEIGHT){
  AXUIElementRef app       = AXWindowFromCGWindow(w);
  CGSize         size      = CGSizeMake(WIDTH, HEIGHT);
  AXValueRef     attrValue = AXValueCreate(kAXValueCGSizeType, &size);

  AXUIElementSetAttributeValue(app, kAXSizeAttribute, attrValue);
  return(true);
} /* resize_window_id */

CFDictionaryRef window_id_to_window(const int WINDOW_ID){
  CFArrayRef      windowList;
  CFDictionaryRef window;

  windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );

  for (int i = 0; i < CFArrayGetCount(windowList); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);
    int windowId = CFDictionaryGetInt(window, kCGWindowNumber);
    if (windowId != WINDOW_ID) {
      continue;
    }
    return(window);
  }
  printf("window %d not found\n", WINDOW_ID);
  return(NULL);
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

int get_pid_window_id(const int PID){
  int             WINDOW_ID = -1;
  CFArrayRef      windowList;
  CFDictionaryRef window;

  windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );
  for (int i = 0; i < CFArrayGetCount(windowList) && (WINDOW_ID == -1); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);
    if (CFDictionaryGetInt(window, kCGWindowOwnerPID) == PID) {
      WINDOW_ID = CFDictionaryGetInt(window, kCGWindowNumber);
      break;
    }
  }
  CFRelease(windowList);
  return(WINDOW_ID);
}

struct Vector *get_window_ids(void){
  struct Vector   *ids_v = vector_new();
  CFArrayRef      windowList;
  CFDictionaryRef window;

  windowList = CGWindowListCopyWindowInfo(
    (
      kCGWindowListExcludeDesktopElements
      | kCGWindowListOptionOnScreenOnly
    ),
    kCGNullWindowID
    );

  for (int i = 0; i < CFArrayGetCount(windowList); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);
    int id = CFDictionaryGetInt(window, kCGWindowNumber);
    if (id > 0) {
      int layer = CFDictionaryGetInt(window, kCGWindowLayer);
      if (layer > 0) {
        continue;
      }

      vector_push(ids_v, (void *)(size_t)id);
    }
  }
  CFRelease(windowList);
  return(ids_v);
}

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
  return(0 == strcmp(appName, "Messages"));
}

CGPoint CGWindowGetPosition(CFDictionaryRef window) {
  CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
  int             x      = CFDictionaryGetInt(bounds, CFSTR("X"));
  int             y      = CFDictionaryGetInt(bounds, CFSTR("Y"));

  return(CGPointMake(x, y));
}

CGSize CGWindowGetSize(CFDictionaryRef window) {
  CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
  int             width  = CFDictionaryGetInt(bounds, CFSTR("Width"));
  int             height = CFDictionaryGetInt(bounds, CFSTR("Height"));

  return(CGSizeMake(width, height));
}

AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window) {
  CGWindowID     targetWindowId, actualWindowId;
  pid_t          pid;
  AXUIElementRef app, appWindow, foundAppWindow;
  int            i;
  CFArrayRef     appWindowList;

  targetWindowId = CFDictionaryGetInt(window, kCGWindowNumber);

  pid = CFDictionaryGetInt(window, kCGWindowOwnerPID);
  app = AXUIElementCreateApplication(pid);
  AXUIElementCopyAttributeValue(
    app, kAXWindowsAttribute, (CFTypeRef *)&appWindowList
    );

  foundAppWindow = NULL;
  for (i = 0; i < CFArrayGetCount(appWindowList); i++) {
    appWindow = CFArrayGetValueAtIndex(appWindowList, i);
    _AXUIElementGetWindow(appWindow, &actualWindowId);
    if (actualWindowId == targetWindowId) {
      foundAppWindow = appWindow;
      break;
    } else {
      continue;
    }
  }
  CFRelease(app);

  return(foundAppWindow);
} /* AXWindowFromCGWindow */

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

void AXWindowGetValue(AXUIElementRef window,
                      CFStringRef    attrName,
                      void           *valuePtr) {
  AXValueRef attrValue;

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
  AXValueRef attrValue = AXValueCreate(kAXValueCGPointType, &position);

  AXUIElementSetAttributeValue(window, kAXPositionAttribute, attrValue);
  CFRelease(attrValue);
}

CGSize AXWindowGetSize(AXUIElementRef window) {
  CGSize size;

  AXWindowGetValue(window, kAXSizeAttribute, &size);
  return(size);
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
  char   *windowTitle;

  if (!appName || !*appName) {
    windowTitle = NULL;
  } else if (!windowName || !*windowName) {
    titleSize   = strlen(appName) + 1;
    windowTitle = (char *)malloc(titleSize);
    strncpy(windowTitle, appName, titleSize);
  } else {
    titleSize   = strlen(appName) + strlen(" - ") + strlen(windowName) + 1;
    windowTitle = (char *)malloc(titleSize);
    snprintf(windowTitle, titleSize, "%s - %s", appName, windowName);
  }
  if (DEBUG_MODE == true) {
    log_info("     |appName:%s|windowName:%s|windowTitle:%s|", appName, windowName, windowTitle);
  }

  return(windowTitle);
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

uint64_t display_space_id(uint32_t did){
  CFStringRef uuid = display_uuid(did);

  if (!uuid) {
    return(0);
  }

  uint64_t sid = SLSManagedDisplayGetCurrentSpace(g_connection, uuid);

  CFRelease(uuid);

  return(sid);
}

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

int get_window_id_pid(int window_id){
  AXUIElementRef window;
  _AXUIElementGetWindow(window, &window_id);
  int _window_id = CFDictionaryGetInt(window, kCGWindowNumber);
  pid_t pid = CFDictionaryGetInt(window, kCGWindowOwnerPID);
  if(_window_id == window_id)
    return((int)pid);

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
  CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements,
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
