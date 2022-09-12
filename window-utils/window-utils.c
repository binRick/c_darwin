#pragma once
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "frameworks/frameworks.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process-utils/process-utils.h"
#include "process/process.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
#include "submodules/log.h/log.h"
#include "system-utils/system-utils.h"
#include "timestamp/timestamp.h"
#include "wildcardcmp/wildcardcmp.h"
#include "window-utils/window-utils.h"
///////////////////////////////////////////////////////////////////////////////
static bool WINDOW_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__window_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_WINDOW_UTILS") != NULL) {
    log_debug("Enabling Window Utils Debug Mode");
    WINDOW_UTILS_DEBUG_MODE = true;
  }
}
static const char *window_levels[]             = { "Base", "Minimum", "Desktop", "Backstop", "Normal", "Floating", "TornOffMenu", "Dock", "MainMenu", "Status", "ModalPanel", "PopUpMenu", "Dragging", "ScreenSaver", "Maximum", "Overlay", "Help", "Utility", "DesktopIcon", "Cursor", "AssistiveTechHigh" };
static const char *EXCLUDED_WINDOW_APP_NAMES[] = {
  "Install macOS*",
  "Control Center",
  "Dock",
  "universalaccessd",
  "TextInputMenuAgent",
  "Spotlight",
  "Finder",
  "Rectangle",
  "SystemUIServer",
  "Window Server",
  "System Preferences",
  "AccountProfileRemote",
  "Analytics & Improvements",
  "AccountProfileRemoteViewService",
  "Apple Advertising",
  "com.apple.preference.security.remoteservice",
  "com.apple.preference",
  "PrivacyAnalytics",
  "com.apple.preference.security.r",
  "com.apple.preference.displays.A",
  NULL,
};

///////////////////////////////////////////////////////////////////////////////

struct Vector *get_windows(){
  struct   Vector *WINDOWS = vector_new(); struct Vector *WINDOW_IDS = vector_new(); struct Vector *SPACE_IDS = vector_new();
  CFArrayRef      windowList = CGWindowListCopyWindowInfo((kCGWindowListExcludeDesktopElements), kCGNullWindowID);
  int             qty        = CFArrayGetCount(windowList);

  for (int i = 0; i < qty; i++) {
    struct window_t *w = get_window_id(
      (size_t)CFDictionaryGetInt(
        CFArrayGetValueAtIndex(windowList, i), kCGWindowNumber
        )
      );
    if (!w) {
      continue;
    }
    vector_push(WINDOWS, (void *)w);
    vector_push(WINDOW_IDS, (void *)(size_t)w->window_id);
  }
  CFArrayRef window_list_ref = cfarray_of_cfnumbers(vector_to_array(WINDOW_IDS), sizeof(uint32_t), vector_size(WINDOW_IDS), kCFNumberSInt32Type);
  CFArrayRef space_list_ref  = SLSCopySpacesForWindows(g_connection, 0x7, window_list_ref);
  int        count           = CFArrayGetCount(space_list_ref);

  for (int ii = 0; ii < count; ii++) {
    CFNumberRef id_ref   = CFArrayGetValueAtIndex(space_list_ref, ii);
    int         space_id = 0;
    CFNumberGetValue(id_ref, CFNumberGetType(id_ref), &space_id);
    vector_push(SPACE_IDS, (void *)(size_t)space_id);
  }
  int active_space_id = SLSGetActiveSpace(g_connection);

  vector_foreach_cast(SPACE_IDS, size_t, (__attribute__((unused)) size_t i, size_t space_id){
    uint64_t space_id_type        = SLSSpaceGetType(g_connection, space_id);
    CFStringRef uuid_ref          = SLSSpaceCopyName(g_connection, space_id);
    char *uuid                    = cstring_from_CFString(uuid_ref);
    int space_mgmt_mode           = SLSGetSpaceManagementMode(space_id);
    CFStringRef space_id_name_ref = SLSSpaceCopyName(g_connection, space_id);
    char *space_name              = cstring_from_CFString(space_id_name_ref);
    CFStringRef space_display_ref = SLSCopyManagedDisplayForSpace(g_connection, space_id);
    CFUUIDRef space_display_uuid  = CFUUIDCreateFromString(NULL, space_display_ref);
    char *space_display           = cstring_from_CFString(space_display_ref);
    uint32_t space_display_id     = CGDisplayGetDisplayIDFromUUID(space_display_uuid);
    uint64_t dsid                 = get_dsid_from_sid(space_id);
    if (WINDOW_UTILS_DEBUG_MODE == true) {
      log_debug("   Space ID %lu|type:%llu|uuid:%s|mgmt mode:%d|name:%s|active? %s|display #%u:%s|dsid:%llu|",
                space_id, space_id_type, uuid, space_mgmt_mode, space_name,
                ((size_t)active_space_id == (size_t)space_id) ? "Yes" : "No",
                space_display_id, space_display,
                dsid
                );
    }
    return(0);
  });
  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_debug("loaded %lu space ids for %lu windows", vector_size(SPACE_IDS), vector_size(WINDOW_IDS));
  }
  return(WINDOWS);
} /* get_windows */

struct window_t *get_window_id(size_t WINDOW_ID){
  CFArrayRef window_list = CGWindowListCopyWindowInfo((kCGWindowListExcludeDesktopElements | kCGWindowListOptionAll | kCGWindowListOptionIncludingWindow), WINDOW_ID);
  int        qty         = CFArrayGetCount(window_list);

  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_error("window #%lu has %d windows", WINDOW_ID, qty);
  }
  if (qty != 1) {
    return(NULL);
  }
  struct window_t *w = calloc(1, sizeof(struct window_t));

  w->started   = timestamp();
  w->window    = CFArrayGetValueAtIndex(window_list, 0);
  w->window_id = (size_t)CFDictionaryGetInt(w->window, kCGWindowNumber);
  w->layer     = CFDictionaryGetInt(w->window, kCGWindowLayer);
  w->pid       = CFDictionaryGetInt(w->window, kCGWindowOwnerPID);
  w->position  = CGWindowGetPosition(w->window);
  w->size      = CGWindowGetSize(w->window);
  proc_pidpath(w->pid, w->pid_path, PATH_MAX);
  w->app_name = CFDictionaryCopyCString(w->window, kCGWindowOwnerName);
  CFArrayRef window_list_ref = cfarray_of_cfnumbers(&w->window_id, sizeof(int), 1, kCFNumberSInt32Type);
  CFArrayRef space_list_ref  = SLSCopySpacesForWindows(g_connection, 0x7, window_list_ref);
  int        space_qty       = CFArrayGetCount(space_list_ref);

  w->space_id = -100;
  if (space_qty == 1) {
    CFNumberRef id_ref = CFArrayGetValueAtIndex(space_list_ref, 0);
    CFNumberGetValue(id_ref, CFNumberGetType(id_ref), &w->space_id);
  }
  errno  = 0;
  w->app = AXUIElementCreateApplication(w->pid);
  errno  = 0;
  if (window_is_excluded(w) == true) {
    if (WINDOW_UTILS_DEBUG_MODE == true) {
      log_warn("window #%lu is excluded|pid:%d|space:%d|", w->window_id, w->pid, w->space_id);
    }
    return(NULL);
  }
  w->psn = PID2PSN(w->pid);
  SLSGetConnectionIDForPSN(g_connection, &w->psn, &w->connection_id);
  w->level = get_window_level(w);
  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_info("window #%lu |pid:%d| level:%d| is not excluded", w->window_id, w->pid, w->level);
  }
  w->app_window_list = calloc(1, sizeof(CFTypeRef));
  AXUIElementCopyAttributeValue(w->app, kAXWindowsAttribute, (CFTypeRef *)&(w->app_window_list));
  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_info("window #%lu |app:%s|pid:%d|w->app_window_list null? %s", w->window_id, w->app_name, w->pid,
             (w->app_window_list == NULL) ? "Yes" : "No"
             );
  }
  w->app_window_list_qty = CFArrayGetCount(w->app_window_list);
  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_info("window #%lu |app:%s|pid:%d|w->app_window_list qty: %lu", w->window_id, w->app_name, w->pid,
             w->app_window_list_qty
             );
  }
  w->rect             = CGRectMake(w->position.x, w->position.y, w->size.width, w->size.height);
  w->width            = (int)(w->size.width);
  w->height           = (int)(w->size.height);
  w->display_index    = 100;
  w->display_uuid     = get_window_display_uuid(w);
  w->window_ids_above = get_window_ids_above_window(w);
  w->window_ids_below = get_window_ids_below_window(w);
  w->is_visible       = get_window_is_visible(w);
  w->is_minimized     = get_window_is_minimized(w);
  w->is_focused       = get_window_is_focused(w);
  w->dur              = timestamp() - w->started;
  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_debug("window id> %lu|dur:%s|pid:%d|app win list len:%lu|app:%s|%dx%d@%dx%d|space id:%d|",
              w->window_id,
              milliseconds_to_string(w->dur),
              w->pid, w->app_window_list_qty,
              w->app_name,
              w->width, w->height,
              (int)w->position.x, (int)w->position.y,
              w->space_id
              );
  }
  return(w);
} /* get_window_id */

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

int get_window_display_id(struct window_t *window){
  CFStringRef _uuid = SLSCopyManagedDisplayForWindow(CGSMainConnectionID(), window->window_id);
  CFUUIDRef   uuid  = CFUUIDCreateFromString(NULL, _uuid);

  CFRelease(_uuid);
  int id = CGDisplayGetDisplayIDFromUUID(uuid);

  CFRelease(uuid);

  return(id);
}

int get_focused_window_id(){
  return(get_focused_window()->window_id);
}

struct window_t *get_focused_window(){
  int pid = get_focused_pid();

  if (pid < 1) {
    return(NULL);
  }
  size_t window_id = get_pid_window_id(pid);

  if (window_id < 1) {
    return(NULL);
  }
  return(get_window_id(window_id));
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

char *get_window_display_uuid(struct window_t *window){
  CFStringRef _uuid = SLSCopyManagedDisplayForWindow(CGSMainConnectionID(), window->window_id);
  char        *uuid = cstring_from_CFString(_uuid);

  CFRelease(_uuid);
  return(uuid);
}

size_t get_pid_window_id(int PID){
  size_t          WINDOW_ID = 0, tmp_WINDOW_ID = 0;
  CFArrayRef      windowList;
  CFDictionaryRef window;
  struct window_t *w = NULL;

  windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );
  for (int i = 0; i < CFArrayGetCount(windowList) && (WINDOW_ID <= 0); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);
    if (CFDictionaryGetInt(window, kCGWindowOwnerPID) == PID) {
      tmp_WINDOW_ID = (size_t)CFDictionaryGetInt(window, kCGWindowNumber);
      w             = get_window_id(tmp_WINDOW_ID);
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

int get_window_space_id(struct window_t *w){
  int           space_id     = -1;
  uint32_t      *window_list = 0;
  int           window_count = 0;
  struct Vector *space_ids_v = vector_new();   //get_space_ids_v();

  for (size_t i = 0; i <= vector_size(space_ids_v) && space_id == -1; i++) {
    window_count = 0;
    for (int ii = 0; ii < window_count && space_id == -1; ii++) {
      if ((uint32_t)window_list[ii] == (uint32_t)w->window_id) {
        space_id = i;
        goto done;
      }
    }
  }
  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_error("cannot find space for window #%lu|%d|%s|", w->window_id, w->pid, w->app_name);
  }
done:
  return(space_id);
}

struct Vector *get_window_space_ids_v(struct Vector *windows_v){
  struct   vector *ids = vector_new();

  for (size_t i = 0; i < vector_size(windows_v); i++) {
    struct window_t *w  = (struct window_t *)vector_get(windows_v, i);
    bool            has = false;
    for (size_t j = 0; has == false && j < vector_size(ids); j++) {
      if ((size_t)w->space_id == (size_t)vector_get(ids, j)) {
        has = true;
      }
    }
    if (has == false) {
      vector_push(ids, (void *)(size_t)w->space_id);
    }
  }
  return(ids);
}

struct Vector *get_window_ids_below_window(struct window_t *w){
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

struct Vector *get_window_ids_above_window(struct window_t *w){
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

struct window_t *get_pid_window(const int PID){
  struct Vector   *windows = get_windows();
  struct window_t *w = NULL, *_w = NULL;

  for (size_t i = 0; i < vector_size(windows); i++) {
    struct window_t *w = vector_get(windows, i);
    _w = (struct window_t *)vector_get(windows, i);
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

int get_window_level(struct window_t *w){
  int level = 0;

  CGSGetWindowLevel(w->connection_id, (CGWindowID)(w->window_id), &level);
  //CGSGetWindowLevel(CGSMainConnectionID(), (CGWindowID)(w->window_id), &level);
  //SLSGetWindowLevel(w->connection_id, (uint32_t)(w->window_id), &level);
  return(level);
}

char *get_window_title(struct window_t *w){
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

void set_window_tags(struct window_t *w){
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

void fade_window(struct window_t *w){
  float         alpha = 0;
  CGSConnection cid;

  cid = _CGSDefaultConnection();

  for (alpha = 1.0; alpha >= 0.0; alpha -= 0.05) {
    CGSSetWindowAlpha(cid, w->window_id, alpha);
    usleep(10000);
  }

  CGSSetWindowAlpha(cid, w->window_id, 0.0);
}

bool window_can_move(struct window_t *w){
  bool result;

  if (AXUIElementIsAttributeSettable(w->window, kAXPositionAttribute, &result) != kAXErrorSuccess) {
    result = 0;
  }
  return(result);
}

bool window_can_resize(struct window_t *w){
  bool result;

  if (AXUIElementIsAttributeSettable(w->window, kAXSizeAttribute, &result) != kAXErrorSuccess) {
    result = 0;
  }
  return(result);
}

bool window_can_minimize(struct window_t *w){
  bool result;

  if (AXUIElementIsAttributeSettable(w->window, kAXMinimizedAttribute, &result) != kAXErrorSuccess) {
    result = 0;
  }
  return(result);
}

bool window_is_topmost(struct window_t *w){
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

void get_window_tags(struct window_t *w){
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

void focus_window(struct window_t *w){
  if (w->space_id != get_space_id()) {
    if (WINDOW_UTILS_DEBUG_MODE == true) {
      log_info("changing space from %d to %d", get_space_id(), w->space_id);
    }
    _SLPSSetFrontProcessWithOptions(&(w->psn), w->window_id, kCPSUserGenerated);
    AXUIElementSetAttributeValue(w->app, kAXFrontmostAttribute, kCFBooleanTrue);
    make_key_window(w);
    AXUIElementPerformAction(w->window, kAXRaiseAction);
    AXUIElementSetAttributeValue(w->window, kAXFrontmostAttribute, kCFBooleanTrue);
  }
}

void make_key_window(struct window_t *w){
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

int get_window_layer(struct window_t *w){
  CFNumberRef objc_window_layer = CFDictionaryGetValue(w->window, kCGWindowLayer);
  int         window_layer;

  CFNumberGetValue(objc_window_layer, kCFNumberIntType, &window_layer);
  return(window_layer);
}

bool get_window_is_focused(struct window_t *w){
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

bool get_window_is_onscreen(struct window_t *w){
  bool result = true;

  return(result);
}

bool get_window_is_visible(struct window_t *w){
  bool result = true;

  if (w->space_id != get_space_id()) {
    result = false;
  }

  if (result == true) {
    if (get_window_is_minimized(w) == true) {
      result = false;
    }
  }
  if (result == true) {
  }

  return(result);
}

bool window_is_excluded(struct window_t *w){
  char **tmp           = EXCLUDED_WINDOW_APP_NAMES;
  bool excluded_window = false;

  if (w->window_id < 1) {
    return(true);
  }

  while (*tmp != NULL && excluded_window == false) {
    if (wildcardcmp(*tmp, w->app_name) == 1) {
      return(true);
    }
    tmp++;
  }
  if (w->layer > 2000) {
    return(true);
  }
  if (strlen(w->pid_path) > 1 && stringfn_starts_with(w->pid_path, "/System/Library/") == true) {
    return(true);
  }
  if (w->space_id < -1) {
    return(true);
  }
  if (WINDOW_UTILS_DEBUG_MODE == true) {
    log_info("wid %lu|y pos:%d|height:%d|pid path:%s|app name:%s|",
             w->window_id, (int)w->position.y, (int)w->size.height,
             w->pid_path, w->app_name
             );
  }
  if (((int)w->position.y < 1) && ((int)((int)w->size.height * -1) == (int)w->position.y)) {
    return(true);
  }

  return(false);
}

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
  int  spaces_qty = get_total_spaces();

  for (int s = 1; s < spaces_qty && is_min == false; s++) {
    uint32_t *minimized_window_list = get_space_minimized_window_list(s, &space_minimized_window_qty);
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
  uint64_t   wid       = (uint64_t)(window->window_id);

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
  log_info("captured image of window %lu :: %dx%d",
           window->window_id,
           (int)bounds.size.width, (int)bounds.size.height
           );

  return(image_ref);
}

bool save_window_cgref_to_png(const CGImageRef image, const char *filename) {
  bool success = false; CFStringRef path; CFURLRef url; CGImageDestinationRef destination;
  {
    path        = CFStringCreateWithCString(NULL, filename, kCFStringEncodingUTF8);
    url         = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, 0);
    destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
    CGImageDestinationAddImage(destination, image, nil);
    success = CGImageDestinationFinalize(destination);
  }

  CFRelease(url); CFRelease(path); CFRelease(destination);
  return((success == true) && fsio_file_exists(filename));
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
  int _window_id = CFDictionaryGetInt(window, kCGWindowNumber);

  if (window_id != _window_id) {
    return(-1);
  }
  pid_t pid = CFDictionaryGetInt(window, kCGWindowOwnerPID);

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

#include <ApplicationServices/ApplicationServices.h>
#include <ColorSync/ColorSyncDevice.h>
#include <CoreGraphics/CGDirectDisplay.h>
#include <CoreGraphics/CGDisplayConfiguration.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <IOKit/i2c/IOI2CInterface.h>
#include <IOKit/IOKitLib.h>
#ifndef kMaxRequests
#define kMaxRequests    10
#endif

#ifndef _IOKIT_IOFRAMEBUFFER_H
#define kIOFBDependentIDKey       "IOFBDependentID"
#define kIOFBDependentIndexKey    "IOFBDependentIndex"
#endif

long DDCDelayBase = 1;

io_service_t IOFramebufferPortFromCGDisplayID(CGDirectDisplayID displayID, CFStringRef displayLocation){
  io_iterator_t iter;
  io_service_t  serv, servicePort = 0;

  kern_return_t err = IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching(IOFRAMEBUFFER_CONFORMSTO), &iter);

  if (err != KERN_SUCCESS) {
    return(0);
  }

  // now recurse the IOReg tree
  while ((serv = IOIteratorNext(iter)) != MACH_PORT_NULL) {
    CFDictionaryRef info;
    CFIndex         vendorID = 0, productID = 0, serialNumber = 0;
    CFNumberRef     vendorIDRef, productIDRef, serialNumberRef;
    CFStringRef     location = CFSTR("");
#ifdef DEBUG
    CFIndex         dependID = 0, dependIndex = 0;
    CFNumberRef     dependIDRef, dependIndexRef;
    io_name_t       name;
    CFStringRef     serial = CFSTR("");

    // get metadata from IOreg node
    IORegistryEntryGetName(serv, name);
    info = IODisplayCreateInfoDictionary(serv, kIODisplayOnlyPreferredName);

    CFStringRef ioRegPath = IORegistryEntryCopyPath(serv, kIOServicePlane);
    // just location/../../ (-- /display0/AppleDisplay)

    // https://stackoverflow.com/a/48450870/3878712
    CFUUIDRef   uuid    = CGDisplayCreateUUIDFromDisplayID(displayID);
    CFStringRef uuidStr = CFUUIDCreateString(NULL, uuid);
#endif
    Boolean     success = 0;

    info = IODisplayCreateInfoDictionary(serv, kIODisplayOnlyPreferredName);     // kIODisplayMatchingInfo

    /* When assigning a display ID, Quartz considers the following parameters:Vendor, Model, Serial Number and Position in the I/O Kit registry */
    // http://opensource.apple.com//source/IOGraphics/IOGraphics-179.2/IOGraphicsFamily/IOKit/graphics/IOGraphicsTypes.h
    CFStringRef locationRef = CFDictionaryGetValue(info, CFSTR(kIODisplayLocationKey));
    if (locationRef) {
      location = CFStringCreateCopy(NULL, locationRef);
    }
#ifdef DEBUG
    CFStringRef serialRef = CFDictionaryGetValue(info, CFSTR(kDisplaySerialString));
    if (serialRef) {
      serial = CFStringCreateCopy(NULL, serialRef);
    }

    if ((dependIDRef = CFDictionaryGetValue(info, CFSTR(kIOFBDependentIDKey)))) {
      CFNumberGetValue(dependIDRef, kCFNumberCFIndexType, &dependID);
    }
    if ((dependIndexRef = CFDictionaryGetValue(info, CFSTR(kIOFBDependentIndexKey)))) {
      CFNumberGetValue(dependIndexRef, kCFNumberCFIndexType, &dependIndex);
    }
#endif
    if (CFDictionaryGetValueIfPresent(info, CFSTR(kDisplayVendorID), (const void **)&vendorIDRef)) {
      success = CFNumberGetValue(vendorIDRef, kCFNumberCFIndexType, &vendorID);
    }

    if (CFDictionaryGetValueIfPresent(info, CFSTR(kDisplayProductID), (const void **)&productIDRef)) {
      success &= CFNumberGetValue(productIDRef, kCFNumberCFIndexType, &productID);
    }

    IOItemCount busCount;
    IOFBGetI2CInterfaceCount(serv, &busCount);

    if (!success || busCount < 1 || CGDisplayIsBuiltin(displayID)) {
      // this does not seem to be a DDC-enabled display, skip it
#ifdef DEBUG
      CFRelease(location);
      CFRelease(serial);
#endif
      CFRelease(info);
      continue;
    }
    // kAppleDisplayTypeKey -- if this is an Apple display, can use IODisplay func to change brightness: http://stackoverflow.com/a/32691700/3878712

    if (CFDictionaryGetValueIfPresent(info, CFSTR(kDisplaySerialNumber), (const void **)&serialNumberRef)) {
      CFNumberGetValue(serialNumberRef, kCFNumberCFIndexType, &serialNumber);
    }
#ifdef DEBUG
    printf("I: Attempting match for %s's IODisplayPort @ %s...\n",
           CFStringGetCStringPtr(uuidStr, kCFStringEncodingUTF8), CFStringGetCStringPtr(location, kCFStringEncodingUTF8));
#endif
    if (displayLocation) {
      // we were provided with a specific ioreg location we suspect the targeted framebuffer to be attached to...
      if (!CFStringHasPrefix(location, displayLocation)) {
        // this framebuffer doesn't reside within the provided location
        CFRelease(info);
        continue;
      }
    }
    // compare IOreg's metadata to CGDisplay's metadata to infer if the IOReg's I2C monitor is the display for the given NSScreen.displayID
    if (CGDisplayVendorNumber(displayID) != (UInt32)vendorID
        || CGDisplayModelNumber(displayID) != (UInt32)productID
        || CGDisplaySerialNumber(displayID) != (UInt32)serialNumber) { // SN is zero in lots of cases, so duplicate-monitors can confuse us :-/
#ifdef DEBUG
      CFRelease(location);
      CFRelease(serial);
#endif
      CFRelease(info);
      continue;
    }

#ifdef DEBUG
    // considering this IOFramebuffer as the match for the CGDisplay, dump out its information
    // compare with `make displaylist`
    printf("\nFramebuffer: %s\n", name);
    printf("%s\n", CFStringGetCStringPtr(ioRegPath, kCFStringEncodingUTF8));
    printf("%s\n", CFStringGetCStringPtr(location, kCFStringEncodingUTF8));
    printf("VN:%ld PN:%ld SN:%ld", vendorID, productID, serialNumber);
    printf(" UN:%d", CGDisplayUnitNumber(displayID));
    printf(" IN:%d", iter);
    printf(" depID:%ld depIdx:%ld", dependID, dependIndex);
    printf(" Serial:%s\n\n", CFStringGetCStringPtr(serial, kCFStringEncodingUTF8));
    CFRelease(location);
    CFRelease(serial);
#endif
    servicePort = serv;
    CFRelease(info);
    break;
  }
} /* IOFramebufferPortFromCGDisplayID */
