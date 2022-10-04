#pragma once
#include "active-app/active-app.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "core/core.h"
#include "frameworks/frameworks.h"
#include "image/utils/utils.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "space/utils/utils.h"
#include "string-utils/string-utils.h"
#include "submodules/log/log.h"
#include "timestamp/timestamp.h"
#include "wildcardcmp/wildcardcmp.h"
#include "window/info/info.h"
#include "window/utils/utils.h"
#include <errno.h>
#include <mach/clock.h>
#include <mach/mach.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
///////////////////////////////////////////////////////////////////////////////
#define MIN_VALID_WINDOW_WIDTH     200
#define MIN_VALID_WINDOW_HEIGHT    100
static bool WINDOW_UTILS_DEBUG_MODE = false, WINDOW_UTILS_VERBOSE_DEBUG_MODE = false;
static char *get_axerror_name(AXError err);
///////////////////////////////////////////////////////////////////////////////

bool minimize_window_id(size_t window_id){
  if (window_id < 0) {
    errno = 0;
    log_error("Invalid Window ID %lu", window_id);
    return(false);
  }
  struct window_info_t *W = get_window_id_info(window_id);
  assert(W->window_id == window_id);
  AXUIElementRef       app = AXWindowFromCGWindow(W->window);
  CFBooleanRef         is_min_ref;
  errno = 0;
  if (AXUIElementCopyAttributeValue(app, kAXMinimizedAttribute, &is_min_ref) != kAXErrorSuccess) {
    log_error("Failed to copy attrs");
    return(false);
  }
  bool is_min = CFBooleanGetValue(is_min_ref);
  if (is_min == true) {
    return(true);
  }
  errno = 0;
  if (AXUIElementSetAttributeValue(app, kAXMinimizedAttribute, kCFBooleanTrue) == kAXErrorSuccess) {
    if (WINDOW_UTILS_DEBUG_MODE) {
      log_info("Set minimized property on window id");
    }
  }else{
    log_error("Failed to set minimized property");
    errno = 0;
    return(false);
  }
  return(true);
}

void set_window_id_flags(size_t window_id, enum window_info_flag_t flags){
  struct window_info_t *w = get_window_id_info(window_id);

  if (!w) {
    return;
  }
  return(set_window_flags(w, flags));
}

void set_window_flags(struct window_info_t *w, enum window_info_flag_t flags){
  log_info("Setting flags %d on window #%lu", flags, w->window_id);
  switch (flags) {
  case WINDOW_FLAG_MINIMIZE:
    log_debug("Minimizing window");
    AXUIElementSetAttributeValue(w->app, kAXMinimizedAttribute, kCFBooleanTrue);
    break;
  default:
    log_error("Unhandled flag %d", flags);
    break;
  }
}

AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window) {
  char           *pid_s;
  CGWindowID     targetWindowId, actualWindowId;
  pid_t          pid;
  AXUIElementRef app, appWindow, foundAppWindow;
  int            i;
  CFArrayRef     appWindowList;

  targetWindowId = CFDictionaryGetInt(window, kCGWindowNumber);
  pid            = CFDictionaryGetInt(window, kCGWindowOwnerPID);
  asprintf(&pid_s, "%d", pid);
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
  free(pid_s);

  return(foundAppWindow);
}

int get_window_display_id(struct window_info_t *window){
  CFStringRef _uuid = SLSCopyManagedDisplayForWindow(CGSMainConnectionID(), window->window_id);
  CFUUIDRef   uuid  = CFUUIDCreateFromString(NULL, _uuid);

  CFRelease(_uuid);
  int id = CGDisplayGetDisplayIDFromUUID(uuid);

  CFRelease(uuid);

  return(id);
}

struct window_info_t *get_focused_window(){
  int pid = get_focused_pid();

  if (pid < 1) {
    return(NULL);
  }
  size_t window_id = get_pid_window_id(pid);

  if (window_id < 1) {
    return(NULL);
  }
  return(get_window_id_info(window_id));
}

struct Vector *get_window_ids(void){
  struct Vector   *ids_v = vector_new();
  CFArrayRef      windowList;
  CFDictionaryRef window;

  windowList = CGWindowListCopyWindowInfo(
    (
      kCGWindowListExcludeDesktopElements
    ),
    kCGNullWindowID
    );

  for (int i = 0; i < CFArrayGetCount(windowList); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);
    int id = CFDictionaryGetInt(window, kCGWindowNumber);
    if (id > 0) {
      vector_push(ids_v, (void *)(size_t)id);
    }
  }
  CFRelease(windowList);
  return(ids_v);
}

char *get_window_display_uuid(struct window_info_t *window){
  CFStringRef _uuid = SLSCopyManagedDisplayForWindow(CGSMainConnectionID(), window->window_id);
  char        *uuid = cstring_from_CFString(_uuid);

  CFRelease(_uuid);
  return(uuid);
}

size_t get_pid_window_id(int PID){
  size_t               WINDOW_ID = 0, tmp_WINDOW_ID = 0;
  CFArrayRef           windowList;
  CFDictionaryRef      window;
  struct window_info_t *w = NULL;

  windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );
  for (int i = 0; i < CFArrayGetCount(windowList) && (WINDOW_ID <= 0); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);
    if (CFDictionaryGetInt(window, kCGWindowOwnerPID) == PID) {
      tmp_WINDOW_ID = (size_t)CFDictionaryGetInt(window, kCGWindowNumber);
      w             = get_window_id_info(tmp_WINDOW_ID);
      if (w != NULL && w->pid == PID && w->window != NULL) {
        WINDOW_ID = tmp_WINDOW_ID;
      }
      if (w) {
        free(w);
      }
    }
  }
  CFRelease(windowList);
  return(WINDOW_ID);
}

int get_window_id_space_id(__attribute__((unused)) size_t window_id){
  int space_id = -1;

  return(space_id);
}

struct Vector *get_window_ids_below_window(struct window_info_t *w){
  struct Vector *ids        = vector_new();
  CFArrayRef    window_list = CGWindowListCopyWindowInfo(
    (kCGWindowListExcludeDesktopElements | kCGWindowListOptionOnScreenOnly | kCGWindowListOptionOnScreenBelowWindow),
    w->window_id);
  int             num_windows = CFArrayGetCount(window_list);
  CFDictionaryRef dict;

  for (int i = 0; i < num_windows; i++) {
    dict = CFArrayGetValueAtIndex(window_list, i);
    int windowId = CFDictionaryGetInt(dict, kCGWindowNumber);
    vector_push(ids, (void *)(size_t)windowId);
  }
  return(ids);
}

struct Vector *get_window_ids_above_window(struct window_info_t *w){
  struct Vector *ids        = vector_new();
  CFArrayRef    window_list = CGWindowListCopyWindowInfo(
    (kCGWindowListExcludeDesktopElements | kCGWindowListOptionOnScreenOnly | kCGWindowListOptionOnScreenAboveWindow),
    w->window_id);
  int             num_windows = CFArrayGetCount(window_list);
  CFDictionaryRef dict;

  for (int i = 0; i < num_windows; i++) {
    dict = CFArrayGetValueAtIndex(window_list, i);
    int windowId = CFDictionaryGetInt(dict, kCGWindowNumber);
    vector_push(ids, (void *)(size_t)windowId);
  }
  return(ids);
}

struct window_info_t *get_pid_window(const int PID){
  struct Vector        *windows = get_window_infos_v();
  struct window_info_t *w = NULL, *_w = NULL;

  for (size_t i = 0; i < vector_size(windows); i++) {
    struct window_info_t *w = vector_get(windows, i);
    _w = (struct window_info_t *)vector_get(windows, i);
    if (w->pid == PID) {
      w = _w;
    }else{
      free(w);
    }
  }
  return(w);
}

CFDictionaryRef window_id_to_window(const int WINDOW_ID){
  CFArrayRef      windowList;
  CFDictionaryRef window;

  windowList = CGWindowListCopyWindowInfo((kCGWindowListExcludeDesktopElements | kCGWindowListOptionAll | kCGWindowListOptionIncludingWindow), WINDOW_ID);

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

int get_window_id_level(size_t window_id){
  int level = 0;

  //CGSGetWindowLevel(w->connection_id, (CGWindowID)(w->window_id), &level);
  // CGSGetWindowLevel(CGSMainConnectionID(), (CGWindowID)(window_id), &level);
  SLSGetWindowLevel(g_connection, (uint32_t)(window_id), &level);
  return(level);
}

int get_window_level(struct window_info_t *w){
  int level = 0;

  //CGSGetWindowLevel(w->connection_id, (CGWindowID)(w->window_id), &level);
  //CGSGetWindowLevel(CGSMainConnectionID(), (CGWindowID)(w->window_id), &level);
  SLSGetWindowLevel(w->connection_id, (uint32_t)(w->window_id), &level);
  return(level);
}

static void get_current_utc_time(struct timespec *ts) {
  clock_serv_t    cclock;
  mach_timespec_t mts;

  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  ts->tv_sec  = mts.tv_sec;
  ts->tv_nsec = mts.tv_nsec;
}

struct window_info_t *get_random_window_info(){
  struct Vector        *windows_v = get_window_infos_v();
  bool                 ok = false;
  int                  qty = 0;
  struct window_info_t *w; size_t id;

  for (size_t i = 0; i < vector_size(windows_v); i++) {
    w = (struct window_info_t *)vector_get(windows_v, i);
    if (w && w->window_id > 0 && (int)w->rect.size.width > MIN_VALID_WINDOW_WIDTH && (int)w->rect.size.height > MIN_VALID_WINDOW_HEIGHT) {
      vector_remove(windows_v, i);
    }
  }
  if (WINDOW_UTILS_DEBUG_MODE) {
    log_debug("%lu windows to choose from", vector_size(windows_v));
  }
  struct timespec ts;

  get_current_utc_time(&ts);
  srand(ts.tv_nsec);

  while (ok == false && qty < 100) {
    id = (rand()) % (vector_size(windows_v) - 1);
    if (WINDOW_UTILS_DEBUG_MODE) {
      log_debug("random : %lu", id);
    }
    qty++;
    w = (struct window_info_t *)vector_get(windows_v, id);
    if (w && w->window_id > 0 && (int)w->rect.size.width > MIN_VALID_WINDOW_WIDTH && (int)w->rect.size.height > MIN_VALID_WINDOW_HEIGHT) {
      ok = true;
    }
  }
  for (size_t i = 0; i < vector_size(windows_v); i++) {
    if (i != id) {
      vector_remove(windows_v, i);
    }
  }
  if (WINDOW_UTILS_DEBUG_MODE) {
    log_debug("random win id: %lu", w->window_id);
  }
  return(w);
} /* get_random_window_info */

size_t get_random_window_info_id(){
  return(get_random_window_info()->window_id);
}

size_t get_first_window_id_by_name(char *name){
  size_t               window_id = 0;
  struct window_info_t *w;
  struct Vector        *windows_v;

  windows_v = get_window_infos_v();

  for (size_t i = 0; i < vector_size(windows_v) && window_id == 0; i++) {
    w = (struct window_info_t *)vector_get(windows_v, i);
    if (!w || !w->name || w->window_id < 1) {
      continue;
    }
    if (WINDOW_UTILS_DEBUG_MODE) {
      log_debug("Checking app %s", w->name);
    }
    if (WINDOW_UTILS_DEBUG_MODE) {
      log_debug("Comparing %s|%s", w->name, name);
    }
    if (strcmp(stringfn_to_lowercase(w->name), stringfn_to_lowercase(name)) == 0) {
      window_id = (size_t)w->window_id;
    }
    if (window_id == 0) {
      char *s;
      asprintf(&s, "%s.app", name);
      if (WINDOW_UTILS_DEBUG_MODE) {
        log_debug("Comparing %s|%s", w->name, s);
      }
      if (strcmp(stringfn_to_lowercase(s), stringfn_to_lowercase(w->name)) == 0) {
        window_id = (size_t)w->window_id;
      }
      if (s) {
        free(s);
      }
    }
    if (w) {
      free(w);
    }
  }
  vector_release(windows_v);
  return(window_id);
} /* get_first_window_id_by_name */

char *get_window_title(struct window_info_t *w){
  char      *title = NULL;
  CFTypeRef value  = NULL;

  AXUIElementCopyAttributeValue(w->window, kAXTitleAttribute, &value);
  if (value) {
    title = cstring_from_CFString(value);
    CFRelease(value);
  } else {
    title = "";
  }

  return(title);
}

void set_window_tags(struct window_info_t *w){
  CGSConnection cid;

  cid = _CGSDefaultConnection();
  CGSWindowTag tags[2];

  tags[0] = CGSTagSticky;
  tags[0] = tags[1] = 0;

  int tags1[2] = { 0 };

  tags1[0] |= (1 << 11);

  tags[0] = CGSTagSticky;
  CGSSetWindowTags(cid, w->window_id, tags1, 32);
  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_info("set window %lu tags: %d/%d", w->window_id, tags1[0], tags1[1]);
  }
}

void fade_window(struct window_info_t *w){
  float         alpha = 0;
  CGSConnection cid;

  cid = _CGSDefaultConnection();

  for (alpha = 1.0; alpha >= 0.0; alpha -= 0.05) {
    CGSSetWindowAlpha(cid, w->window_id, alpha);
    usleep(10000);
  }

  CGSSetWindowAlpha(cid, w->window_id, 0.0);
}

bool window_can_move(struct window_info_t *w){
  bool result;

  if (AXUIElementIsAttributeSettable(w->window, kAXPositionAttribute, &result) != kAXErrorSuccess) {
    result = 0;
  }
  return(result);
}

bool window_can_resize(struct window_info_t *w){
  bool result;

  if (AXUIElementIsAttributeSettable(w->window, kAXSizeAttribute, &result) != kAXErrorSuccess) {
    result = 0;
  }
  return(result);
}

bool window_can_minimize(struct window_info_t *w){
  bool result;

  if (AXUIElementIsAttributeSettable(w->window, kAXMinimizedAttribute, &result) != kAXErrorSuccess) {
    result = 0;
  }
  return(result);
}

bool window_is_topmost(struct window_info_t *w){
  bool is_topmost = window_layer(w->window) == CGWindowLevelForKey(LAYER_ABOVE);

  return(is_topmost);
}

uint32_t getWindowId(AXUIElementRef window) {
  CGWindowID _windowId;

  if (_AXUIElementGetWindow(window, &_windowId) == kAXErrorSuccess) {
    return(_windowId);
  }

  return(-1);
}

void get_window_tags(struct window_info_t *w){
  CGSConnection cid;

  cid = _CGSDefaultConnection();
  CGSWindowTag tags[2];

  tags[0] = tags[1] = 0;
  CGSGetWindowTags(cid, w->window_id, tags, 32);
  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_info("window %lu tags: %d/%d", w->window_id, tags[0], tags[1]);
  }

  uint32_t mask = 0;

  CGSGetWindowEventMask(cid, w->window_id, &mask);
  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_info("window %lu event mask: %lu", w->window_id, (size_t)mask);
  }
}

void focus_window_id(size_t WINDOW_ID){
  return(focus_window(get_window_id_info(WINDOW_ID)));
}

bool get_pid_is_minimized(int pid){
  AXError        err;
  AXUIElementRef app = AXUIElementCreateApplication(pid);
  CFArrayRef     appWindowList;

  AXUIElementCopyAttributeValue(
    app, kAXWindowsAttribute, (CFTypeRef *)&appWindowList
    );
  int window_count = CFArrayGetCount(appWindowList);

  log_info("pid %d has %d windows", pid, window_count);

  CFTypeRef isMinimizedBoolRef;

  err = AXUIElementCopyAttributeValue(app, kAXMinimizedAttribute, &isMinimizedBoolRef);
  if (err == kAXErrorSuccess) {
    log_info("Got is min for pid %d", pid);
  }else{
    log_error("Failed to get pid %d is minimized attribute: %s", pid, get_axerror_name(err));
  }

  CFBooleanRef boolRef;

  err = AXUIElementCopyAttributeValue(app, kAXHiddenAttribute, &boolRef);
  bool is_hidden = CFBooleanGetValue(boolRef);

  if (err == kAXErrorSuccess) {
    log_info("Got is hidden for pid %d: %s", pid, is_hidden?"Yes":"No");
  }else{
    log_error("Failed to get pid %d is hidden attribute: %s", pid, get_axerror_name(err));
  }

  Boolean   result = 0;
  CFTypeRef value;

  err = AXUIElementCopyAttributeValue(app, kAXMinimizedAttribute, &value);
  if (err == kAXErrorSuccess) {
    result = CFBooleanGetValue(value);
    CFRelease(value);
  }else{
    log_error("Failed to get pid %d is minimized attribute: %s", pid, get_axerror_name(err));
  }
  return(result);
} /* get_pid_is_minimized */

void minimize_window(struct window_info_t *w){
  CFTypeRef value = NULL;

  if (AXUIElementCopyAttributeValue(w->app, kAXMinimizedAttribute, &value) == kAXErrorSuccess) {
    CFRelease(value);
  }else{
    log_error("Failed to copy minimized attribute");
  }
  if (AXUIElementSetAttributeValue(w->app, kAXMinimizedAttribute, kCFBooleanTrue) == kAXErrorSuccess) {
    log_info("Set minimized property on window");
  }else{
    log_error("Failed to set minimized property");
  }
}

void focus_window(struct window_info_t *w){
  if (WINDOW_UTILS_DEBUG_MODE) {
    log_debug("window %lu:pid %d", w->window_id, w->pid);
  }
  assert(w->pid > 0 && w->window_id > 0);
  errno = 0;
  AXError             err;
  ProcessSerialNumber psn = {};
  psn   = PID2PSN(w->pid);
  errno = 0;
  err   = SetFrontProcessWithOptions(&psn, kSetFrontProcessFrontWindowOnly);
  if (err != kAXErrorSuccess) {
    log_error("failed to set psn");
  }

  CFArrayRef window_list = CGWindowListCopyWindowInfo(kCGWindowListOptionAll | kCGWindowListOptionIncludingWindow, w->window_id);
  int        qty         = CFArrayGetCount(window_list);
  if (WINDOW_UTILS_DEBUG_MODE) {
    log_debug("%d windows", qty);
  }
  if (qty > 0) {
    CFDictionaryRef window = CFArrayGetValueAtIndex(window_list, 0);
    AXUIElementRef  app    = AXWindowFromCGWindow(window);
    AXUIElementSetAttributeValue(app, kAXFrontmostAttribute, kCFBooleanTrue);
    AXUIElementPerformAction(window, kAXRaiseAction);
    AXUIElementSetAttributeValue(window, kAXMainAttribute, kCFBooleanTrue);
  }
  make_key_window(w);
  /*
   * return;
   *
   *
   * AXUIElementRef app = AXWindowFromCGWindow(window);
   * _SLPSSetFrontProcessWithOptions(&psn, (uint32_t)w->window_id, kCPSUserGenerated);
   * AXUIElementSetAttributeValue(app, kAXFrontmostAttribute, kCFBooleanTrue);
   * AXUIElementPerformAction(window, kAXRaiseAction);
   * AXUIElementSetAttributeValue(window, kAXMainAttribute, kCFBooleanTrue);
   * make_key_window(w);
   *
   *
   *
   * return;
   * AXUIElementSetAttributeValue(app, kAXFrontmostAttribute, kCFBooleanTrue);
   *
   * make_key_window(w);
   * AXUIElementPerformAction(window, kAXRaiseAction);
   * AXUIElementSetAttributeValue(window, kAXMainAttribute, kCFBooleanTrue);
   *
   *
   * return;
   * _SLPSSetFrontProcessWithOptions(&psn, w->window_id, kCPSUserGenerated);
   *
   * AXUIElementPerformAction(window, kAXRaiseAction);
   * AXUIElementSetAttributeValue(window, kAXFrontmostAttribute, kCFBooleanTrue);
   * //AXUIElementSetAttributeValue(w->window, kAXFullscreenAttribute, kCFBooleanTrue);
   * return;
   *
   *
   *
   *
   *
   *
   *
   *
   *
   *
   * err =  _SLPSSetFrontProcessWithOptions(&(w->psn), w->window_id, kCPSUserGenerated);
   * if (err != kAXErrorSuccess) {
   *  log_error("Set Front Fail for Window #%lu", w->window_id);
   * }
   * log_debug("set app");
   * make_key_window(w);
   * AXUIElementPerformAction(window, kAXRaiseAction);
   * AXUIElementSetAttributeValue(window, kAXFrontmostAttribute, kCFBooleanTrue);
   * log_debug("set win");
   *
   *
   * }else{
   * log_error("Failed to find window #%lu",w->window_id);
   * }
   */
}

void make_key_window(struct window_info_t *w){
  uint8_t bytes1[0xf8] = { [0x04] = 0xf8, [0x08] = 0x01, [0x3a] = 0x10 };
  uint8_t bytes2[0xf8] = { [0x04] = 0xf8, [0x08] = 0x02, [0x3a] = 0x10 };

  memcpy(bytes1 + 0x3c, &(w->window_id), sizeof(uint32_t));
  memset(bytes1 + 0x20, 0xFF, 0x10);

  memcpy(bytes2 + 0x3c, &(w->window_id), sizeof(uint32_t));
  memset(bytes2 + 0x20, 0xFF, 0x10);

  SLPSPostEventRecordTo(&(w->psn), bytes1);
  SLPSPostEventRecordTo(&(w->psn), bytes2);
}

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
  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_info("     |appName:%s|windowName:%s|windowTitle:%s|", appName, windowName, windowTitle);
  }

  return(windowTitle);
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

int get_window_layer(struct window_info_t *w){
  CFNumberRef objc_window_layer = CFDictionaryGetValue(w->window, kCGWindowLayer);
  int         window_layer;

  CFNumberGetValue(objc_window_layer, kCFNumberIntType, &window_layer);
  log_info("Window #%lu has layer %d", w->window_id, window_layer);
  return(window_layer);
}

bool get_window_is_focused(struct window_info_t *w){
  return((w->is_visible == true && w->is_minimized == false && w->pid == get_focused_pid() && vector_size(w->window_ids_above) == 0)
         ? true
         : false
         );
}

CGSize AXWindowGetSize(AXUIElementRef window) {
  CGSize size;

  AXWindowGetValue(window, kAXSizeAttribute, &size);
  return(size);
}

bool get_window_is_visible(struct window_info_t *w){
  bool result = true;

  if ((size_t)w->space_id != (size_t)get_current_space_id()) {
    result = false;
  }

  if (result == true) {
    if (get_window_id_is_minimized(w->window_id) == true) {
      result = false;
    }
  }
  if (result == true) {
  }

  return(result);
}

AXUIElementRef _get_matching_window_AXUIElementRef(AXUIElementRef app, CFStringRef window_title, CGRect window_frame) {
  AXUIElementRef ax_window = NULL;

  // get list of application windows
  CFArrayRef ax_windows = NULL;

  AXUIElementCopyAttributeValue(app, kAXWindowsAttribute, (CFTypeRef *)&ax_windows);
  if (ax_windows == NULL) {
    return(NULL);
  }
  CFIndex count = CFArrayGetCount(ax_windows);

  if (count == 0) {
    CFRelease(ax_windows);
    return(NULL);
  }

  bool match_found = false;

  // get window title & frame for comparison
  CFStringRef title;
  CGRect      frame;

  for (int i = 0; i < count; i++) {
    ax_window = CFArrayGetValueAtIndex(ax_windows, i);

    AXValueRef ax_size;
    AXValueRef ax_origin;

    AXUIElementCopyAttributeValue(ax_window, kAXTitleAttribute, (CFTypeRef *)&title);
    AXUIElementCopyAttributeValue(
      ax_window, kAXSizeAttribute, (CFTypeRef *)&ax_size);
    AXUIElementCopyAttributeValue(
      ax_window, kAXPositionAttribute, (CFTypeRef *)&ax_origin);

    AXValueGetValue(ax_size, kAXValueCGSizeType, &frame.size);
    AXValueGetValue(ax_origin, kAXValueCGPointType, &frame.origin);
    CFRelease(ax_size);
    CFRelease(ax_origin);

    // final checks
    if (title != NULL && window_title == NULL) {
      CFRelease(title);
      continue;
    }
    if ((title == NULL && window_title != NULL)
        || CFStringCompare(title, window_title, 0) != kCFCompareEqualTo
        || CGRectEqualToRect(frame, window_frame) == false) {
      continue;
    }

    CFRetain(ax_window);
    match_found = true;
    break;
  }
  CFRelease(ax_windows);

  if (match_found == false) {
    if (title != NULL) {
      CFRelease(title);
    }
    return(NULL);
  }

  return(ax_window);
} /* _get_matching_window_AXUIElementRef */

struct cf_window *get_cf_window(CFDictionaryRef window_dict){
  struct cf_window *w       = NULL;
  CFNumberRef      cf_layer = CFDictionaryGetValue(window_dict, kCGWindowLayer);
  int              layer;

  CFNumberGetValue(cf_layer, kCFNumberIntType, &layer);
  if (layer != 0) {
    log_error("no window");
    return(NULL);
  }

  CFNumberRef cf_pid = CFDictionaryGetValue(window_dict, kCGWindowOwnerPID);
  pid_t       pid;

  CFNumberGetValue(cf_pid, kCFNumberIntType, &pid);
  CFStringRef title = CFDictionaryGetValue(window_dict, kCGWindowName);

  if (title != NULL) {
    title = CFStringCreateCopy(kCFAllocatorDefault, title);
  }

  CFDictionaryRef cf_frame = CFDictionaryGetValue(window_dict, kCGWindowBounds);
  CGRect          frame;

  CGRectMakeWithDictionaryRepresentation(cf_frame, &frame);
  if (CGSizeEqualToSize(frame.size, (CGSize){ 0, 0 })) {
    if (title != NULL) {
      CFRelease(title);
    }
    log_error("no window");
    return(NULL);
  }

  w->app         = (struct cf_app *)malloc(sizeof(struct cf_app));
  w->app->ax_app = AXUIElementCreateApplication(pid);
  CFStringRef name;

  AXUIElementCopyAttributeValue(w->app->ax_app, kAXTitleAttribute, (CFTypeRef *)&name);
  w->app->name = CFStringCreateCopy(kCFAllocatorDefault, name);
  w->app->pid  = pid;
  AXUIElementCopyAttributeValue(w->app->ax_app, kAXTitleAttribute, (CFTypeRef *)&(w->app->name));
  if (w->app->name == NULL) {
    log_error("no app");
    return(NULL);
  }

  // get AXUIElementRef
  w->ax_window = _get_matching_window_AXUIElementRef(w->app->ax_app, title, frame);
  CFNumberRef cf_id = CFDictionaryGetValue(window_dict, kCGWindowNumber);
  CGWindowID  id;

  CFNumberGetValue(cf_id, kCGWindowIDCFNumberType, &id);

  if (w->ax_window == NULL) {
    if (title != NULL) {
      CFRelease(title);
    }
    log_error("no window");
    return(NULL);
  }
  w->id    = id;
  w->frame = frame;
  w->title = title;
  return(w);
} /* get_cf_window */

struct cf_window *get_cf_window_list(struct cf_window_list_options options, struct cf_window *w){
  CGWindowListOption cf_options =
    (kCGWindowListOptionAll * options.all)
    | (kCGWindowListOptionOnScreenOnly * options.on_screen_only)
    | (kCGWindowListExcludeDesktopElements * options.exclude_desktop_elements)
    | (kCGWindowListOptionIncludingWindow * options.including_window)
    | (kCGWindowListOptionOnScreenAboveWindow * options.above_window)
    | (kCGWindowListOptionOnScreenBelowWindow * options.below_window);

  CGWindowID relative_window = kCGNullWindowID;

  if (w != NULL) {
    relative_window = w->id;
  }

  CFArrayRef            cf_window_dicts = CGWindowListCopyWindowInfo(cf_options, relative_window);
  CFIndex               window_count    = CFArrayGetCount(cf_window_dicts);

  struct cf_window_list *wl = &(struct cf_window_list){
    .count    = 0,
    .capacity = window_count,
    .windows  = malloc(sizeof(pointer_t) * window_count),
  };

  int counter = 0;

  for (int i = 0; i < window_count; i++) {
    CFDictionaryRef  cf_window_dict = CFArrayGetValueAtIndex(cf_window_dicts, i);
    struct cf_window *new_w         = get_cf_window(cf_window_dict);

    // only append if window is valid
    if (new_w != NULL) {
      wl->windows[counter] = new_w;
      counter             += 1;
    }
  }
  wl->count = counter;

  CFRelease(cf_window_dicts);

  return(wl);
} /* get_cf_window_list */

AXError AXUIElementCopyWindowAtPosition(CGPoint position, AXUIElementRef *window) {
  *window = NULL;

  AXUIElementRef systemWideElement = AXUIElementCreateSystemWide();
  AXUIElementRef element           = NULL;
  CFStringRef    role              = NULL;

  // First, retrieve the element at the given position.
  AXError error = AXUIElementCopyElementAtPosition(systemWideElement,
                                                   position.x,
                                                   position.y,
                                                   &element);

  if (error != kAXErrorSuccess) {
    goto end;
  }

  // If this element is a window, return it.
  error = AXUIElementCopyAttributeValue(element,
                                        kAXRoleAttribute,
                                        (CFTypeRef *)&role);
  if (error != kAXErrorSuccess) {
    goto end;
  }

  if (CFStringCompare(role, kAXWindowRole, 0) == kCFCompareEqualTo) {
    *window = element;
    CFRetain(*window);
    goto end;
  }

  // Otherwise, return the window attribute.
  error = AXUIElementCopyAttributeValue(element,
                                        kAXWindowAttribute,
                                        (CFTypeRef *)window);

end:
  CFRelease(systemWideElement);
  if (element != NULL) {
    CFRelease(element);
  }
  if (role != NULL) {
    CFRelease(role);
  }
  return(error);
} /* AXUIElementCopyWindowAtPosition */

void set_window_active_on_all_spaces(struct window_info_t *w){
  SLSProcessAssignToAllSpaces(g_connection, w->pid);
}

int window_layer(struct window_info_t *window){
  int layer = 0;

  SLSGetWindowLevel(g_connection, window->window_id, &layer);
  return(layer);
}

void window_set_layer(struct window_info_t *window, uint32_t layer) {
  SLSSetWindowLevel(g_connection, window->window_id, layer);
}

void window_id_send_to_space(size_t window_id, uint64_t dsid) {
  uint32_t   wid         = (uint32_t)window_id;
  CFArrayRef window_list = cfarray_of_cfnumbers(&wid, sizeof(uint32_t), 1, kCFNumberSInt32Type);

  SLSMoveWindowsToManagedSpace(g_connection, window_list, dsid);
}

bool set_window_id_to_space(size_t window_id, int space_id) {
  uint32_t wid = (uint32_t)window_id;
  uint64_t sid = (uint64_t)space_id;

  log_info("%lu|%lld", window_id, sid);
  CFArrayRef wids = cfarray_of_cfnumbers(&wid, sizeof(uint32_t), 1, kCFNumberSInt32Type);

  SLSMoveWindowsToManagedSpace(CGSMainConnectionID(), wids, (uint64_t)space_id);
  log_info("%lu|%d", window_id, space_id);
  return(true);
}

void window_send_to_space(struct window_info_t *window, uint64_t dsid) {
  uint32_t   wid         = (uint32_t)window->window_id;
  CFArrayRef window_list = cfarray_of_cfnumbers(&wid, sizeof(uint32_t), 1, kCFNumberSInt32Type);

  SLSMoveWindowsToManagedSpace(g_connection, window_list, dsid);
  if (CGPointEqualToPoint(window->rect.origin, g_nirvana)) {
    SLSMoveWindow(g_connection, window->window_id, &g_nirvana);
  }
}

bool get_window_id_is_minimized(size_t window_id){
  bool                 ret    = false;
  bool                 is_min = false;
  CFTypeRef            value;
  struct window_info_t *w = get_window_id_info(window_id);

  log_debug("Checking if window %lu is minimized", window_id);
  if (w->window_id != window_id) {
    return(false);
  }
  errno = 0;
  AXUIElementRef app = AXWindowFromCGWindow(w->window);
  AXError        err = AXUIElementCopyAttributeValue(app, kAXMinimizedAttribute, &value);

  if (err == kAXErrorSuccess) {
    is_min = CFBooleanGetValue(value);
    CFRelease(value);
    return(is_min);
  }else{
    log_error("Failed to copy minimized attribute for window ID #%lu. Error: %s",
              window_id,
              get_axerror_name(err)
              );
    errno = 0;
  }
  return(ret);
}

CFStringRef get_window_role_ref(struct window_info_t *w){
  const void *role = NULL;

  AXUIElementCopyAttributeValue(w->window, kAXRoleAttribute, &role);
  return(role);
}

int get_window_connection_id(struct window_info_t *w){
  int conn;

  SLSGetConnectionIDForPSN(g_connection, &w->psn, &conn);
  return(conn);
}

bool get_window_is_popover(struct window_info_t *w){
  CFStringRef role = get_window_role_ref(w->window);

  if (!role) {
    return(false);
  }

  bool result = CFEqual(role, kAXPopoverRole);

  CFRelease(role);

  return(result);
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

struct window_info_t *get_focused_window_info(){
  struct Vector        *window_infos_v = get_window_infos_v();
  struct window_info_t *window_info    = NULL;

  for (size_t i = 0; i < vector_size(window_infos_v); i++) {
    struct window_info_t *wi = (struct window_info_t *)vector_get(window_infos_v, i);
    if (wi->is_focused == true) {
      window_info = wi;
    }else{
      free(wi);
    }
  }
  return(window_info);
}
struct window_info_t *get_window_id_info(size_t window_id){
  struct Vector        *window_infos_v = get_window_infos_id_v(window_id);
  struct window_info_t *window_info    = NULL;

  for (size_t i = 0; i < vector_size(window_infos_v); i++) {
    struct window_info_t *wi = (struct window_info_t *)vector_get(window_infos_v, i);
    if (wi->window_id == window_id) {
      window_info = wi;
    }else{
      free(wi);
    }
  }
  return(window_info);
}

void print_all_window_items(FILE *rsp) {
  unsigned long started      = timestamp();
  CFArrayRef    window_list  = CGWindowListCopyWindowInfo(kCGWindowListExcludeDesktopElements, kCGNullWindowID);
  int           window_count = CFArrayGetCount(window_list);
  size_t        qty          = 0;

  for (int i = 0; i < window_count; ++i) {
    CFDictionaryRef dictionary = CFArrayGetValueAtIndex(window_list, i);
    if (!dictionary) {
      continue;
    }

    CFStringRef     owner_ref         = CFDictionaryGetValue(dictionary, kCGWindowOwnerName);
    CFNumberRef     owner_pid_ref     = CFDictionaryGetValue(dictionary, kCGWindowOwnerPID);
    CFStringRef     name_ref          = CFDictionaryGetValue(dictionary, kCGWindowName);
    CFNumberRef     layer_ref         = CFDictionaryGetValue(dictionary, kCGWindowLayer);
    CFNumberRef     window_id_ref     = CFDictionaryGetValue(dictionary, kCGWindowNumber);
    CFDictionaryRef window_bounds     = CFDictionaryGetValue(dictionary, kCGWindowBounds);
    CFNumberRef     memory_usage_ref  = CFDictionaryGetValue(dictionary, kCGWindowMemoryUsage);
    CFNumberRef     sharing_state_ref = CFDictionaryGetValue(dictionary, kCGWindowSharingState);
    CFNumberRef     store_type_ref    = CFDictionaryGetValue(dictionary, kCGWindowStoreType);
    CFBooleanRef    is_onscreen_ref   = CFDictionaryGetValue(dictionary, kCGWindowIsOnscreen);

    if (!name_ref || !owner_ref || !owner_pid_ref || !layer_ref || !window_id_ref) {
      continue;
    }

    uint64_t      owner_pid = 0, window_id = 0, sharing_state = 0, store_type = 0;
    long long int layer = 0, memory_usage = 0;
    bool          is_onscreen = false;
    char          *name, *owner;

    CFNumberGetValue(layer_ref, CFNumberGetType(layer_ref), &layer);
    CFNumberGetValue(memory_usage_ref, CFNumberGetType(memory_usage_ref), &memory_usage);
    CGRect bounds;
    CFNumberGetValue(store_type_ref, CFNumberGetType(store_type_ref), &store_type);
    CFNumberGetValue(sharing_state_ref, CFNumberGetType(sharing_state_ref), &sharing_state);
    CFNumberGetValue(window_id_ref, CFNumberGetType(window_id_ref), &window_id);
    CFNumberGetValue(owner_pid_ref, CFNumberGetType(owner_pid_ref), &owner_pid);
    if (is_onscreen_ref) {
      is_onscreen = CFBooleanGetValue(is_onscreen_ref);
    }
    CGRectMakeWithDictionaryRepresentation(window_bounds, &bounds);
    owner = cfstring_copy(owner_ref);
    name  = cfstring_copy(name_ref);

    if (strcmp(name, "") != 0) {
      qty++;
      fprintf(rsp,
              " owner:%s,window id:%lld,pid:%lld,name:%s,layer:%lld,"
              "size:%dx%d,pos:%dx%d,mem:%lld,sharingstate:%lld,storetype:%lld,"
              "onscreen:%s,"
              "\n",
              owner,
              window_id,
              owner_pid,
              name,
              layer,
              (int)bounds.size.height, (int)bounds.size.width,
              (int)bounds.origin.x, (int)bounds.origin.y,
              memory_usage,
              sharing_state,
              store_type,
              is_onscreen?"Yes":"No"
              );
    }

    free(owner);
    free(name);
  }
  CFRelease(window_list);
  log_debug("Listed %lu Window Items in %s",
            qty,
            milliseconds_to_string(timestamp() - started)
            );
} /* print_all_menu_items */

CGImageRef capture_window_id_height(size_t window_id, size_t height){
  CGImageRef img_ref = capture_window_id(window_id);
  int        w[2], h[2];

  w[0] = CGImageGetWidth(img_ref);
  h[0] = CGImageGetHeight(img_ref);
  h[1] = height;
  float factor = 1;
  if(h[0] > 100){
    factor = (float)(h[0]) / (float)(h[1]);
  }

  w[1] = (int)((float)w[0] / factor);
  return(resize_cgimage(img_ref, w[1], h[1]));
}

struct Vector *get_captured_window_infos_v(){
  struct Vector *v = vector_new();
  log_info("get_captured_window_infos_v");
  return(v);

}


CGImageRef capture_window_id_width(size_t window_id, size_t width){
  CGImageRef img_ref = capture_window_id(window_id);
  int        w[2], h[2];

  w[0] = CGImageGetWidth(img_ref);
  h[0] = CGImageGetHeight(img_ref);
  w[1] = width;
  float factor = (float)(w[0]) / (float)(w[1]);

  h[1] = (int)((float)h[0] / factor);
  return(resize_cgimage(img_ref, w[1], h[1]));
}

CGImageRef preview_window_id(size_t window_id){
  CGImageRef img_ref = capture_window_id(window_id);

  return(resize_cgimage(img_ref, CGImageGetWidth(img_ref) / 5, CGImageGetHeight(img_ref) / 5));
}

CGImageRef capture_window_id_rect(size_t window_id, CGRect rect){
  uint64_t   wid       = (uint64_t)(window_id);
  CGImageRef image_ref = NULL;

  SLSCaptureWindowsContentsToRectWithOptions(g_connection, &wid, true, rect, 1 << 8, &image_ref);
  return(image_ref);
}

CGImageRef capture_window_id_ptr(size_t window_id){
  CGImageRef *image_ref = NULL;
  uint64_t   wid        = (uint64_t)(window_id);

  SLSCaptureWindowsContentsToRectWithOptions(g_connection, &wid, true, CGRectNull, 1 << 8, image_ref);
  return(image_ref);
}

CGImageRef capture_window_id(size_t window_id){
  CGImageRef image_ref = NULL;
  uint64_t   wid       = (uint64_t)(window_id);

  SLSCaptureWindowsContentsToRectWithOptions(g_connection, &wid, true, CGRectNull, 1 << 8, &image_ref);
  return(image_ref);
}

void window_move(struct window_info_t *window, CGPoint point) {
  window->rect.origin = point;
  SLSMoveWindow(g_connection, window->window_id, &point);
  CFNumberRef number = CFNumberCreate(NULL,
                                      kCFNumberSInt32Type,
                                      &window->window_id);
  const void *values[1] = { number };
  CFArrayRef array      = CFArrayCreate(NULL, values, 1, &kCFTypeArrayCallBacks);
  SLSReassociateWindowsSpacesByGeometry(g_connection, array);
  CFRelease(array);
  CFRelease(number);
}

CFStringRef display_active_display_uuid(void) {
  return(SLSCopyActiveMenuBarDisplayIdentifier(g_connection));
}

ProcessSerialNumber get_window_ProcessSerialNumber(__attribute__((unused)) struct window_info_t *w){
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

uint32_t display_manager_active_display_count(void){
  uint32_t count;

  CGGetActiveDisplayList(0, NULL, &count);
  return(count);
}

CGRect get_resized_window_info_rect_by_factor_bottom_side(struct window_info_t *w, float width_factor, float height_factor){
  int cur_display_width = get_display_width(), cur_display_height = get_display_height();

  log_info("resizing to bottom with display size %dx%d", cur_display_width, cur_display_height);
  float
    new_width  = (int)((float)cur_display_width * width_factor),
    new_height = (int)((float)cur_display_height * height_factor),
    new_x      = 0,
    new_y      = (int)((float)cur_display_height * height_factor)
  ;
  CGRect new_rect = CGRectMake((float)new_x, (float)new_y, (float)new_width, (float)new_height);

  return(new_rect);
}

CGRect get_resized_window_info_rect_by_factor_top_side(struct window_info_t *w, float width_factor, float height_factor){
  int cur_display_width = get_display_width(), cur_display_height = get_display_height();

  log_info("resizing to top with display size %dx%d", cur_display_width, cur_display_height);
  float
    new_width  = (int)((float)cur_display_width * width_factor),
    new_height = (int)((float)cur_display_height * height_factor),
    new_x      = 0,
    new_y      = 0
  ;
  CGRect new_rect = CGRectMake((float)new_x, (float)new_y, (float)new_width, (float)new_height);

  return(new_rect);
}

CGRect get_resized_window_info_rect_by_factor_left_side(struct window_info_t *w, float width_factor, float height_factor){
  int cur_display_width = get_display_width(), cur_display_height = get_display_height();

  log_info("resizing to left with display size %dx%d", cur_display_width, cur_display_height);
  float
    new_width  = (int)((float)cur_display_width * width_factor),
    new_height = (int)((float)cur_display_height * height_factor),
    new_x      = 0,
    new_y      = 0
  ;
  CGRect new_rect = CGRectMake((float)new_x, (float)new_y, (float)new_width, (float)new_height);

  return(new_rect);
}

CGRect get_resized_window_info_rect_by_factor_right_side(struct window_info_t *w, float width_factor, float height_factor){
  int cur_display_width = get_display_width(), cur_display_height = get_display_height();
  float
      new_width = (int)((float)cur_display_width * width_factor),
    new_height  = (int)((float)cur_display_height * height_factor),
    new_x       = (int)(((float)cur_display_width) - ((float)cur_display_width) * (float)width_factor),
    new_y       = 0
  ;
  CGRect new_rect = CGRectMake((float)new_x, (float)new_y, (float)new_width, (float)new_height);

  return(new_rect);
}

CGRect get_resized_window_info_rect_by_factor(struct window_info_t *w, float width_factor, float height_factor){
  float
    cur_width  = w->rect.size.width,
    cur_height = w->rect.size.height,
    cur_x      = w->rect.origin.x,
    cur_y      = w->rect.origin.y,
    new_width  = cur_width * width_factor,
    new_height = cur_height * height_factor,
    new_x      = (cur_x == 0)
              ? cur_x
              : cur_x - (new_width - cur_width)
  ,
    new_y = cur_y
  ;
  CGRect new_rect = CGRectMake((float)new_x, (float)new_y, (float)new_width, (float)new_height);

  return(new_rect);
}

bool move_window_id(size_t window_id, const int X, const int Y){
  CGPoint newPosition;

  newPosition.x = X;
  newPosition.y = Y;
  struct window_info_t *w = get_window_id_info(window_id);

  assert(w->window_id == window_id);
  AXUIElementRef app = AXWindowFromCGWindow(w->window);

  AXWindowSetPosition(app, newPosition);
  return(true);
}

bool move_window(struct window_info_t *w, const int X, const int Y){
  CGPoint newPosition;

  newPosition.x = X;
  newPosition.y = Y;
  AXUIElementRef app = AXWindowFromCGWindow(w->window);

  AXWindowSetPosition(app, newPosition);
  return(true);
}

bool move_window_info(struct window_info_t *w, const int X, const int Y){
  AXUIElementRef app = AXWindowFromCGWindow(w->window);

  if (!app) {
    log_error("Invalid app");
    return(false);
  }
  CGPoint newPosition;

  newPosition.x = X;
  newPosition.y = Y;
  AXWindowSetPosition(app, newPosition);
  bool ok = true;

  if (ok == false) {
    log_error("Failed moving app");
  }
  return(ok);
}

bool resize_window_info(struct window_info_t *w, const int WIDTH, const int HEIGHT){
  CFArrayRef window_list = CGWindowListCopyWindowInfo(
    kCGWindowListExcludeDesktopElements | kCGWindowListOptionAll | kCGWindowListOptionIncludingWindow,
    w->window_id);
  int qty = CFArrayGetCount(window_list);

  if (WINDOW_UTILS_DEBUG_MODE) {
    log_debug("%d windows", qty);
  }
  AXUIElementRef app       = AXWindowFromCGWindow(w->window);
  CGSize         size      = CGSizeMake(WIDTH, HEIGHT);
  AXValueRef     attrValue = AXValueCreate(kAXValueCGSizeType, &size);

  AXUIElementSetAttributeValue(app, kAXSizeAttribute, attrValue);
  return(true);
}

bool resize_window(struct window_info_t *w, const int WIDTH, const int HEIGHT){
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

CGWindowID CGWindowWithInfo(AXUIElementRef window, __attribute__((unused)) CGPoint location) {
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

  AXUIElementSetAttributeValue(ax_window, kAXPositionAttribute, ax_position);
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

static char *get_axerror_name(AXError err){
  return(ax_error_str[-err]);
}
static void __attribute__((constructor)) __constructor__window_utils(void){
  if (getenv("DEBUG") != NULL || getenv("VERBOSE_DEBUG_WINDOW_UTILS") != NULL) {
    log_debug("Enabling Window Utils Verbose Debug Mode");
    WINDOW_UTILS_DEBUG_MODE         = true;
    WINDOW_UTILS_VERBOSE_DEBUG_MODE = true;
  }
  if (getenv("DEBUG") != NULL || getenv("DEBUG_WINDOW_UTILS") != NULL) {
    log_debug("Enabling Window Utils Debug Mode");
    WINDOW_UTILS_DEBUG_MODE = true;
  }
}
