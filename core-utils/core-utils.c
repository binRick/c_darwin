#pragma once
#include "core-utils/core-utils.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <pthread.h>
static const char *EXCLUDED_WINDOW_APP_NAMES[] = {
  "Control Center",
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
  NULL,
};

#define WINDOW_APP_CACHE_QTY      5500
#define WINDOW_APP_CACHE_MS       100
#define WINDOW_APP_CACHE_INDEX    (window)
#define WINDOW_APP_CACHE_DEBUG    false
pthread_mutex_t space_ids_cache_mutex;
struct window_app_cache_t {
  CFArrayRef    app_window_list;
  unsigned long ts;
};
struct window_app_cache_t window_app_cache[WINDOW_APP_CACHE_QTY + 1] = { 0 };

AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window) {
  unsigned long  ts_start = timestamp();
  bool           cached = false;
  CGWindowID     targetWindowId, actualWindowId;
  pid_t          pid;
  AXUIElementRef app, appWindow, foundAppWindow;
  int            i;
  unsigned long  ts;
  CFArrayRef     appWindowList;

  targetWindowId = CFDictionaryGetInt(window, kCGWindowNumber);
  pid            = CFDictionaryGetInt(window, kCGWindowOwnerPID);
  app            = AXUIElementCreateApplication(pid);
  ts             = timestamp();
  if (window_app_cache[pid].app_window_list != NULL && window_app_cache[pid].ts > 0 && (timestamp() - window_app_cache[pid].ts) < WINDOW_APP_CACHE_MS) {
    if (WINDOW_APP_CACHE_DEBUG) {
      log_debug("reading pid %d window %d app window list from %s old cache", pid, targetWindowId, milliseconds_to_string(timestamp() - window_app_cache[pid].ts));
    }
    cached        = true;
    appWindowList = window_app_cache[pid].app_window_list;
  }else{
    AXUIElementCopyAttributeValue(
      app, kAXWindowsAttribute, (CFTypeRef *)&appWindowList
      );
    if (WINDOW_APP_CACHE_DEBUG) {
      log_debug("copied window %d | pid %d app attribute in %lums", targetWindowId, pid, (unsigned long)timestamp() - ts);
      log_debug("saving pid %d window %d app window list to cache", pid, targetWindowId);
    }
    window_app_cache[pid] = (struct window_app_cache_t){
      .app_window_list = appWindowList,
      .ts              = timestamp(),
    };
  }
  if (WINDOW_APP_CACHE_DEBUG) {
    log_debug("got app list for window %d pid %d in %lums", targetWindowId, pid, (unsigned long)timestamp() - ts);
  }

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

  if (WINDOW_APP_CACHE_DEBUG) {
    log_debug("completed app window looking for window #%d|pid %d in %lums | cached? %s",
              targetWindowId,
              pid,
              (unsigned long)timestamp() - ts_start,
              cached == true ? "Yes" :"No"
              );
  }
  return(foundAppWindow);
} /* AXWindowFromCGWindow */

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

int get_window_display_id(struct window_t *window){
  CFStringRef _uuid = SLSCopyManagedDisplayForWindow(CGSMainConnectionID(), window->window_id);
  CFUUIDRef   uuid  = CFUUIDCreateFromString(NULL, _uuid);

  CFRelease(_uuid);
  int id = CGDisplayGetDisplayIDFromUUID(uuid);

  CFRelease(uuid);

  return(id);
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
  if (DEBUG_MODE == true) {
    log_info("     |appName:%s|windowName:%s|windowTitle:%s|", appName, windowName, windowTitle);
  }

  return(windowTitle);
}

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

CGSize AXWindowGetSize(AXUIElementRef window) {
  CGSize size;

  AXWindowGetValue(window, kAXSizeAttribute, &size);
  return(size);
}

bool get_window_is_visible(struct window_t *window){
  bool result = true;

  if (window->position.x == 0 && window->position.y == 25 && window->layer == 0) {
    result = true;
  }
  if (result == true) {
    if (window_id_is_minimized(window->window_id) == true) {
      result = false;
    }
  }

  return(result);
}

int get_focused_window_id(){
  return(get_focused_window()->window_id);
}

struct window_t *get_focused_window(){
  int             pid       = get_focused_pid();
  int             window_id = get_pid_window_id(pid);
  struct window_t *w        = get_window_id(window_id);

  return(w);
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

char *get_window_display_uuid(struct window_t *window){
  CFStringRef _uuid = SLSCopyManagedDisplayForWindow(CGSMainConnectionID(), window->window_id);
  char        *uuid = cstring_from_CFString(_uuid);

  CFRelease(_uuid);
  return(uuid);
}

struct Vector *get_window_ids(void){
  struct Vector   *ids_v = vector_new();
  CFArrayRef      windowList;
  CFDictionaryRef window;

  windowList = CGWindowListCopyWindowInfo(
    (
      kCGWindowListExcludeDesktopElements
      //| kCGWindowListOptionOnScreenOnly
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

int space_type(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid));
}

char *get_space_uuid(uint64_t sid){
  int type = space_type(sid);

  if (type != 0 && type != 4) {
    log_error("space not user or fullscreen");
    return("");
  }
  CFStringRef uuid_ref = SLSSpaceCopyName(g_connection, sid);
  char        *uuid    = cstring_from_CFString(uuid_ref);

  if (!uuid_ref) {
    log_error("uuid ref err");
  }
  CFRelease(uuid_ref);
  return(uuid);
}

int get_space_via_keywin(void) {
  CFArrayRef winList =
    CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);

  int len = CFArrayGetCount(winList);
  int i, num = 0;

  for (i = 0; i < len; i++) {
    CFDictionaryRef winDict = CFArrayGetValueAtIndex(winList, i);
    if (CFDictionaryContainsKey(winDict, kCGWindowWorkspace)) {
      const void  *thing = CFDictionaryGetValue(winDict, kCGWindowWorkspace);
      CFNumberRef numRef = (CFNumberRef)thing;
      CFNumberGetValue(numRef, kCFNumberIntType, &num);
      return(num);
    }
  }
  return(-1);
}

int get_space_id(void){
  int        space;
  CFArrayRef windows = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
  CFIndex    i, n;

  for (i = 0, n = CFArrayGetCount(windows); i < n; i++) {
    CFDictionaryRef windict  = CFArrayGetValueAtIndex(windows, i);
    CFNumberRef     spacenum = CFDictionaryGetValue(windict, kCGWindowWorkspace);
    if (spacenum) {
      CFNumberGetValue(spacenum, kCFNumberIntType, &space);
      return(space);
    }
  }
  return(-1);
}

CGPoint display_center(uint32_t did){
  CGRect bounds = display_bounds(did);

  return((CGPoint) { bounds.origin.x + bounds.size.width / 2, bounds.origin.y + bounds.size.height / 2 });
}

void set_space_by_index(int space){
  CFNotificationCenterRef nc     = CFNotificationCenterGetDistributedCenter();
  CFStringRef             numstr = CFStringCreateWithFormat(NULL, nil, CFSTR("%d"), space);

  CFNotificationCenterPostNotification(nc, CFSTR("com.apple.switchSpaces"), numstr, NULL, TRUE);
}

CGRect display_bounds(uint32_t did){
  return(CGDisplayBounds(did));
}

#define SPACE_IDS_CACHE_QTY      15
#define SPACE_IDS_CACHE_MS       500
#define SPACE_IDS_CACHE_INDEX    (i + 1)
#define SPACE_IDS_CACHE_DEBUG    false
pthread_mutex_t space_ids_cache_mutex;
struct space_ids_cache_t {
  uint32_t      *window_list;
  unsigned long ts;
  int           window_count;
};
struct space_ids_cache_t space_ids_cache[SPACE_IDS_CACHE_QTY + 1] = { 0 };

int get_window_space_id(struct window_t *w){
  int      space_id  = -1;
  int      space_cnt = total_spaces();
  uint32_t *window_list;
  int      window_count;

  for (int i = 0; i <= space_cnt + 1 && space_id == -1; i++) {
    window_count = 0;
    pthread_mutex_lock(&space_ids_cache_mutex);
    if (space_ids_cache[SPACE_IDS_CACHE_INDEX].ts > 0 && (timestamp() - space_ids_cache[SPACE_IDS_CACHE_INDEX].ts) < SPACE_IDS_CACHE_MS && SPACE_IDS_CACHE_INDEX < SPACE_IDS_CACHE_QTY) {
      if (SPACE_IDS_CACHE_DEBUG == true) {
        log_info("reading %d window cached space window list #%d (%s old) from cache",
                 space_ids_cache[SPACE_IDS_CACHE_INDEX].window_count,
                 SPACE_IDS_CACHE_INDEX,
                 milliseconds_to_string((timestamp() - space_ids_cache[SPACE_IDS_CACHE_INDEX].ts))
                 );
      }
      window_list  = space_ids_cache[SPACE_IDS_CACHE_INDEX].window_list;
      window_count = space_ids_cache[SPACE_IDS_CACHE_INDEX].window_count;
    }else{
      window_list = space_window_list(i, &window_count, true);
      if (SPACE_IDS_CACHE_DEBUG == true) {
        log_info("caching space #%d=>%d", i, SPACE_IDS_CACHE_INDEX);
      }
      space_ids_cache[SPACE_IDS_CACHE_INDEX] = (struct space_ids_cache_t){
        .ts           = timestamp(),
        .window_list  = window_list,
        .window_count = window_count,
      };
    }
    pthread_mutex_unlock(&space_ids_cache_mutex);
    for (int ii = 0; ii < window_count && space_id == -1; ii++) {
      if ((uint32_t)window_list[ii] == (uint32_t)w->window_id) {
        space_id = i;
        goto done;
      }
    }
  }
  //log_error("cannot find space for window #%d|%d|%s|", w->window_id, w->pid, w->app_name);
done:
  return(space_id);
} /* get_window_space_id */

struct window_t *get_window_id(const int WINDOW_ID){
  int             focused_pid = get_focused_pid();
  struct window_t *w          = malloc(sizeof(struct window_t));

  if (!w) {
    return(NULL);
  }

  w->window_id = WINDOW_ID;
  w->window    = window_id_to_window(w->window_id);
  w->app_name  = stringfn_trim(CFDictionaryCopyCString(w->window, kCGWindowOwnerName));
  if (window_is_excluded(w) == true) {
    return(NULL);
  }
  w->app          = AXWindowFromCGWindow(w->window);
  w->pid          = CFDictionaryGetInt(w->window, kCGWindowOwnerPID);
  w->child_pids_v = get_child_pids(w->pid);
  w->is_minimized = window_id_is_minimized(w->window_id);
  w->layer_ref    = CFDictionaryGetValue(w->window, kCGWindowLayer);
  CFNumberGetValue(w->layer_ref, kCFNumberIntType, &w->layer);
  w->space_id   = get_window_space_id(w);
  w->is_visible = get_window_is_visible(w);
  w->psn        = PID2PSN(w->pid);
  SLSGetConnectionIDForPSN(g_connection, &w->psn, &w->connection_id);
  w->window_name  = stringfn_trim(CFDictionaryCopyCString(w->window, kCGWindowName));
  w->display_id   = get_window_display_id(w);
  w->display_uuid = get_window_display_uuid(w);
  w->window_title = stringfn_trim(windowTitle(w->app_name, w->window_name));
  w->position     = CGWindowGetPosition(w->window);
  w->size         = CGWindowGetSize(w->window);
  w->rect         = CGRectMake(w->position.x, w->position.y, w->size.width, w->size.height);
  w->width        = (int)(w->size.width);
  w->height       = (int)(w->size.height);
  w->is_focused   = (w->is_minimized == false && w->is_visible == true && focused_pid == w->pid) ? true : false;
  get_kinfo_proc(w->pid, &w->pid_info);

  return(w);
} /* get_window_id */

uint32_t getWindowId(AXUIElementRef window) {
  CGWindowID _windowId;

  if (_AXUIElementGetWindow(window, &_windowId) == kAXErrorSuccess) {
    return(_windowId);
  }

  return(-1);
}

struct Vector *get_windows(){
  struct Vector *windows    = vector_new();
  unsigned long ts          = timestamp();
  struct Vector *window_ids = get_window_ids();

  for (size_t i = 0; i < vector_size(window_ids); i++) {
    struct window_t *w = get_window_id((size_t)vector_get(window_ids, i));
    if (w != NULL) {
      vector_push(windows, (void *)w);
    }
  }
  log_info("created %lu windows in %lums", vector_size(windows), (unsigned long)timestamp() - ts);
  return(windows);
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

CGImageRef resize_cgimage(CGImageRef imageRef, int width, int height) {
  CGRect       newRect = CGRectIntegral(CGRectMake(0, 0, width, height));
  CGContextRef context = CGBitmapContextCreate(NULL, width, height,
                                               CGImageGetBitsPerComponent(imageRef),
                                               CGImageGetBytesPerRow(imageRef),
                                               CGImageGetColorSpace(imageRef),
                                               CGImageGetBitmapInfo(imageRef));

  CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
  CGContextDrawImage(context, newRect, imageRef);
  CGImageRef newImageRef = CGBitmapContextCreateImage(context);

  CGContextRelease(context);
  return(newImageRef);
}

bool window_is_excluded(struct window_t *w){
  char **tmp           = EXCLUDED_WINDOW_APP_NAMES;
  bool excluded_window = false;

  while (*tmp != NULL && excluded_window == false) {
    if (strcmp(*tmp, w->app_name) == 0) {
      return(true);
    }
    tmp++;
  }
  return(false);
}

void command_list_displays(){
  CGDisplayCount display_count;

  CGGetOnlineDisplayList(INT_MAX, NULL, &display_count);

  CGDirectDisplayID display_list[display_count];

  CGGetOnlineDisplayList(INT_MAX, display_list, &display_count);

  for (unsigned int i = 0; i < display_count; i++) {
    CGDirectDisplayID current_display = display_list[i];
    CGRect            rect            = CGDisplayBounds(current_display);
    bool              is_builtin      = CGDisplayIsBuiltin(current_display);
    bool              is_main         = CGDisplayIsMain(current_display);

    printf("Display #%d\n", i + 1);
    printf("ID: %u\n", current_display);
    printf("X: %d\n", (int)rect.origin.x);
    printf("Y: %d\n", (int)rect.origin.y);
    printf("Width: %d\n", (int)rect.size.width);
    printf("Height: %d\n", (int)rect.size.height);
    printf("Built-in: %s\n", is_builtin ? "true" : "false");
    printf("Main: %s\n", is_main ? "true" : "false");
    printf("\n");
  }
}

uint64_t dsid_from_sid(uint32_t sid) {
  uint64_t   result      = 0;
  int        desktop_cnt = 1;

  CFArrayRef display_spaces_ref   = SLSCopyManagedDisplaySpaces(g_connection);
  int        display_spaces_count = CFArrayGetCount(display_spaces_ref);

  for (int i = 0; i < display_spaces_count; ++i) {
    CFDictionaryRef display_ref  = CFArrayGetValueAtIndex(display_spaces_ref, i);
    CFArrayRef      spaces_ref   = CFDictionaryGetValue(display_ref, CFSTR("Spaces"));
    int             spaces_count = CFArrayGetCount(spaces_ref);

    for (int j = 0; j < spaces_count; ++j) {
      CFDictionaryRef space_ref = CFArrayGetValueAtIndex(spaces_ref, j);
      CFNumberRef     sid_ref   = CFDictionaryGetValue(space_ref, CFSTR("id64"));
      CFNumberGetValue(sid_ref, CFNumberGetType(sid_ref), &result);
      if (sid == (uint32_t)desktop_cnt) {
        goto out;
      }

      ++desktop_cnt;
    }
  }

  result = 0;
out:
  CFRelease(display_spaces_ref);
  return(result);
}

uint32_t display_id_for_space(uint32_t sid) {
  uint64_t dsid = dsid_from_sid(sid);

  if (!dsid) {
    return(0);
  }
  CFStringRef uuid_string = SLSCopyManagedDisplayForSpace(g_connection, dsid);

  if (!uuid_string) {
    return(0);
  }

  CFUUIDRef uuid = CFUUIDCreateFromString(NULL, uuid_string);
  uint32_t  id   = CGDisplayGetDisplayIDFromUUID(uuid);

  CFRelease(uuid);
  CFRelease(uuid_string);

  return(id);
}

CGImageRef space_capture(uint32_t sid) {
  uint64_t   dsid  = dsid_from_sid(sid);
  CGImageRef image = NULL;

  if (dsid) {
    CFArrayRef result = SLSHWCaptureSpace(g_connection, dsid, 0);
    uint32_t   count  = CFArrayGetCount(result);
    if (count > 0) {
      image = (CGImageRef)CFArrayGetValueAtIndex(result, 0);
    }
  }
  return(image);
}

int mission_control_index(uint64_t sid) {
  uint64_t   result      = 0;
  int        desktop_cnt = 1;

  CFArrayRef display_spaces_ref   = SLSCopyManagedDisplaySpaces(g_connection);
  int        display_spaces_count = CFArrayGetCount(display_spaces_ref);

  for (int i = 0; i < display_spaces_count; ++i) {
    CFDictionaryRef display_ref  = CFArrayGetValueAtIndex(display_spaces_ref, i);
    CFArrayRef      spaces_ref   = CFDictionaryGetValue(display_ref, CFSTR("Spaces"));
    int             spaces_count = CFArrayGetCount(spaces_ref);

    for (int j = 0; j < spaces_count; ++j) {
      CFDictionaryRef space_ref = CFArrayGetValueAtIndex(spaces_ref, j);
      CFNumberRef     sid_ref   = CFDictionaryGetValue(space_ref, CFSTR("id64"));
      CFNumberGetValue(sid_ref, CFNumberGetType(sid_ref), &result);
      if (sid == result) {
        goto out;
      }

      ++desktop_cnt;
    }
  }

  desktop_cnt = 0;
out:
  CFRelease(display_spaces_ref);
  return(desktop_cnt);
}

void get_display_bounds(int *x, int *y, int *w, int *h){
  // get the cursor position
  CGEventRef event          = CGEventCreate(NULL);
  CGPoint    cursorLocation = CGEventGetLocation(event);

  CFRelease(event);

  // get display which contains the cursor, that's the one we want to tile on
  int numDisplays; CGDirectDisplayID displays[16];

  CGGetDisplaysWithPoint(cursorLocation, 16, displays, &numDisplays);

  HIRect bounds;

  HIWindowGetAvailablePositioningBounds(displays[0], kHICoordSpace72DPIGlobal,
                                        &bounds);

  *x = bounds.origin.x;
  *y = bounds.origin.y;
  *w = bounds.size.width;
  *h = bounds.size.height;
}

AXUIElementRef get_frontmost_app(){
  pid_t pid; ProcessSerialNumber psn;

  GetFrontProcess(&psn); GetProcessPID(&psn, &pid);
  return(AXUIElementCreateApplication(pid));
}

char *cfstring_copy(CFStringRef string) {
  CFIndex num_bytes = CFStringGetMaximumSizeForEncoding(CFStringGetLength(string), kCFStringEncodingUTF8);
  char    *result   = malloc(num_bytes + 1);

  if (!result) {
    return(NULL);
  }

  if (!CFStringGetCString(string, result, num_bytes + 1, kCFStringEncodingUTF8)) {
    free(result);
    result = NULL;
  }

  return(result);
}

char *string_copy(char *s) {
  int  length  = strlen(s);
  char *result = malloc(length + 1);

  if (!result) {
    return(NULL);
  }

  memcpy(result, s, length);
  result[length] = '\0';
  return(result);
}

uint32_t *display_active_display_list(uint32_t *count) {
  int      display_count = display_active_display_count();
  uint32_t *result       = malloc(sizeof(uint32_t) * display_count);

  CGGetActiveDisplayList(display_count, result, count);
  return(result);
}

CGRect display_menu_bar_rect(uint32_t did) {
  CGRect bounds = {};

  SLSGetRevealedMenuBarBounds(&bounds, g_connection, display_space_id(did));
  return(bounds);
}

uint32_t display_active_display_count(void) {
  uint32_t count;

  CGGetActiveDisplayList(0, NULL, &count);
  return(count);
}

bool display_menu_bar_visible(void) {
  int status = 0;

  SLSGetMenuBarAutohideEnabled(g_connection, &status);
  return(!status);
}

int get_focused_pid(){
  ProcessSerialNumber psn;

  GetFrontProcess(&psn);
  pid_t pid = PSN2PID(psn);

  if (pid > 1) {
    return((int)pid);
  }

  CFArrayRef window_list = CGWindowListCopyWindowInfo(
    kCGWindowListExcludeDesktopElements | kCGWindowListOptionOnScreenOnly,
    kCGNullWindowID);

  int num_windows = CFArrayGetCount(window_list);

  for (int i = 0; i < num_windows; i++) {
    CFDictionaryRef dict              = CFArrayGetValueAtIndex(window_list, i);
    CFNumberRef     objc_window_layer = CFDictionaryGetValue(dict, kCGWindowLayer);

    int             window_layer;
    CFNumberGetValue(objc_window_layer, kCFNumberIntType, &window_layer);

    if (window_layer == 0) {
      CFNumberRef objc_window_pid = CFDictionaryGetValue(dict, kCGWindowOwnerPID);

      int         window_pid = 0;
      CFNumberGetValue(objc_window_pid, kCFNumberIntType, &window_pid);

      return(window_pid);
    }
  }

  return(-1);
}

ProcessSerialNumber PID2PSN(pid_t pid) {
  ProcessSerialNumber tempPSN;

  GetProcessForPID(pid, &tempPSN);
  return(tempPSN);
}

pid_t PSN2PID(ProcessSerialNumber psn) {
  pid_t tempPID;

  GetProcessPID(&psn, &tempPID);
  return(tempPID);
}

int space_display_id(int sid){
  CFStringRef uuid_string = SLSCopyManagedDisplayForSpace(g_connection, (uint32_t)sid);

  if (!uuid_string) {
    return(0);
  }

  CFUUIDRef uuid = CFUUIDCreateFromString(NULL, uuid_string);
  uint32_t  id   = CGDisplayGetDisplayIDFromUUID(uuid);

  CFRelease(uuid);
  CFRelease(uuid_string);

  return((int)id);
}

uint32_t *space_minimized_window_list(uint64_t sid, int *count){
  int      window_count, non_min_window_count;
  uint32_t *windows_list         = space_window_list_for_connection(&sid, 1, 0, &window_count, true);
  uint32_t *non_min_windows_list = space_window_list_for_connection(&sid, 1, 0, &non_min_window_count, false);

  *count = (window_count - non_min_window_count);
  uint32_t *minimized_window_list = calloc((*count) + 1, sizeof(uint32_t));
  int      on                     = 0;

  for (int i = 0; i < window_count; i++) {
    bool is_minimized = true;
    for (int ii = 0; ii < non_min_window_count && is_minimized == true; ii++) {
      if (non_min_windows_list[ii] == windows_list[i]) {
        is_minimized = false;
      }
    }
    if (is_minimized == true) {
      minimized_window_list[on++] = windows_list[i];
    }
  }

  return(minimized_window_list);
}

CFStringRef cfstring_from_cstring(char *cstring){
  CFStringRef cfstring = CFStringCreateWithCString(NULL, cstring, kCFStringEncodingUTF8);

  return(cfstring);
}

char * CFStringCopyUTF8String(CFStringRef aString){
  if (aString == NULL) {
    return(NULL);
  }

  CFIndex length  = CFStringGetLength(aString);
  CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);

  char    *buffer = (char *)malloc(maxSize);

  if (CFStringGetCString(aString, buffer, maxSize, kCFStringEncodingUTF8)) {
    return(buffer);
  }

  return(buffer);
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

char * cstring_get_ascii_string(CFStringRef data) {
  const char *bytes = NULL;
  char       *chars = NULL;
  size_t     len    = 0;

  bytes = CFStringGetCStringPtr(data, ASCII_ENCODING);
  if (bytes == NULL) {
    len   = (size_t)CFStringGetLength(data) + 1;
    chars = (char *)calloc(len, sizeof(char));
    if (chars != NULL) {
      if (!CFStringGetCString(data, chars, (CFIndex)len, ASCII_ENCODING)) {
        free(chars);
        chars = NULL;
      }
    }
  } else {
    len   = strlen(bytes) + 1;
    chars = (char *)calloc(len, sizeof(char));
    if (chars != NULL) {
      strcpy(chars, bytes);
    }
  }
  return(chars);
}

char *get_chars_from_CFString(CFStringRef cf_string) {
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

int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key){
  CFTypeRef ref;
  int32_t   value;

  ref = IOHIDDeviceGetProperty(device, key);
  if (ref) {
    if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
      CFNumberGetValue((CFNumberRef)ref, kCFNumberSInt32Type, &value);
      return(value);
    }
  }
  return(0);
}

int get_string_property(IOHIDDeviceRef device, CFStringRef prop, wchar_t *buf, size_t len){
  CFStringRef str;

  if (!len) {
    return(0);
  }

  str = (CFStringRef)IOHIDDeviceGetProperty(device, prop);

  buf[0] = 0;

  if (str) {
    CFIndex str_len = CFStringGetLength(str);
    CFRange range;
    CFIndex used_buf_len;
    CFIndex chars_copied;

    len--;

    range.location = 0;
    range.length   = ((size_t)str_len > len)? len: (size_t)str_len;
    chars_copied   = CFStringGetBytes(str,
                                      range,
                                      kCFStringEncodingUTF32LE,
                                      (char)'?',
                                      FALSE,
                                      (UInt8 *)buf,
                                      len * sizeof(wchar_t),
                                      &used_buf_len);

    if (chars_copied <= 0) {
      buf[0] = 0;
    }else{
      buf[chars_copied] = 0;
    }

    return(0);
  }else {
    return(-1);
  }
} /* get_string_property */

unsigned short get_vendor_id(IOHIDDeviceRef device){
  return(get_int_property(device, CFSTR(kIOHIDVendorIDKey)));
}

unsigned short get_product_id(IOHIDDeviceRef device){
  return(get_int_property(device, CFSTR(kIOHIDProductIDKey)));
}

int get_serial_number(IOHIDDeviceRef device, wchar_t *buf, size_t len){
  return(get_string_property(device, CFSTR(kIOHIDSerialNumberKey), buf, len));
}

int get_manufacturer_string(IOHIDDeviceRef device, wchar_t *buf, size_t len){
  return(get_string_property(device, CFSTR(kIOHIDManufacturerKey), buf, len));
}

int get_product_string(IOHIDDeviceRef device, wchar_t *buf, size_t len){
  return(get_string_property(device, CFSTR(kIOHIDProductKey), buf, len));
}

CFArrayRef cfarray_of_cfnumbers(void *values, size_t size, int count, CFNumberType type){
  CFNumberRef temp[count];

  for (int i = 0; i < count; ++i) {
    temp[i] = CFNumberCreate(NULL, type, ((char *)values) + (size * i));
  }

  CFArrayRef result = CFArrayCreate(NULL, (const void **)temp, count, &kCFTypeArrayCallBacks);

  for (int i = 0; i < count; ++i) {
    CFRelease(temp[i]);
  }

  return(result);
}

bool ts_init(uint64_t size){
  int      page_size = getpagesize();

  uint64_t num_pages = size / page_size;
  uint64_t remainder = size % page_size;

  if (remainder) {
    num_pages++;
  }

  g_temp_storage.used   = 0;
  g_temp_storage.size   = num_pages * page_size;
  g_temp_storage.memory = mmap(0, g_temp_storage.size + page_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

  bool result = g_temp_storage.memory != MAP_FAILED;

  if (result) {
    mprotect(g_temp_storage.memory + g_temp_storage.size, page_size, PROT_NONE);
  }

  return(result);
}

uint64_t ts_align(uint64_t used, uint64_t align){
  assert((align & (align - 1)) == 0);

  uintptr_t ptr   = (uintptr_t)(g_temp_storage.memory + used);
  uintptr_t a_ptr = (uintptr_t)align;
  uintptr_t mod   = ptr & (a_ptr - 1);

  if (mod != 0) {
    ptr += a_ptr - mod;
  }

  return(ptr - (uintptr_t)g_temp_storage.memory);
}

void ts_assert_within_bounds(void){
  if (g_temp_storage.used > g_temp_storage.size) {
    fprintf(stderr, "fatal error: temporary_storage exceeded amount of allocated memory. requested %lld, but allocated size is %lld\n", g_temp_storage.used, g_temp_storage.size);
    exit(EXIT_FAILURE);
  }
}

void *ts_alloc_aligned(uint64_t elem_size, uint64_t elem_count){
  for ( ;;) {
    uint64_t used     = g_temp_storage.used;
    uint64_t aligned  = ts_align(used, elem_size);
    uint64_t new_used = aligned + (elem_size * elem_count);

    if (__sync_bool_compare_and_swap(&g_temp_storage.used, used, new_used)) {
      ts_assert_within_bounds();
      return(g_temp_storage.memory + aligned);
    }
  }
}

void *ts_alloc_unaligned(uint64_t size){
  uint64_t used = __sync_fetch_and_add(&g_temp_storage.used, size);

  ts_assert_within_bounds();
  return(g_temp_storage.memory + used);
}

void *ts_expand(void *ptr, uint64_t old_size, uint64_t increment){
  if (ptr) {
    assert(ptr == g_temp_storage.memory + g_temp_storage.used - old_size);
    __sync_fetch_and_add(&g_temp_storage.used, increment);
    ts_assert_within_bounds();
  } else {
    ptr = ts_alloc_unaligned(increment);
  }

  return(ptr);
}

void *ts_resize(void *ptr, uint64_t old_size, uint64_t new_size){
  assert(ptr == g_temp_storage.memory + g_temp_storage.used - old_size);
  if (new_size > old_size) {
    __sync_fetch_and_add(&g_temp_storage.used, new_size - old_size);
  } else if (new_size < old_size) {
    __sync_fetch_and_sub(&g_temp_storage.used, old_size - new_size);
  }
  return(ptr);
}

void ts_reset(void){
  g_temp_storage.used = 0;
}

uint32_t ax_window_id(AXUIElementRef ref){
  uint32_t wid = 0;

  _AXUIElementGetWindow(ref, &wid);
  return(wid);
}

pid_t ax_window_pid(AXUIElementRef ref){
  return(*(pid_t *)((void *)ref + 0x10));
}

int total_spaces(void){
  int rows, cols;

  CoreDockGetWorkspacesCount(&rows, &cols);
  return(cols);
}

uint32_t *space_window_list(uint64_t sid, int *count, bool include_minimized){
  return(space_window_list_for_connection(&sid, 1, 0, count, include_minimized));
}

uint32_t *space_window_list_for_connection(uint64_t *space_list, int space_count, int cid, int *count, bool include_minimized){
  int        window_count = 0;
  uint64_t   set_tags     = 1;
  uint64_t   clear_tags   = 0;
  uint32_t   options      = include_minimized ? 0x7 : 0x2;
  uint32_t   *window_list = calloc(1, sizeof(uint32_t));

  CFArrayRef space_list_ref = cfarray_of_cfnumbers(space_list, sizeof(uint64_t), 100, kCFNumberSInt64Type);
//  log_info("space window list> %d||min?%d|cid:%d|space_count:%d|",space_count,include_minimized,cid,space_count);
//  space_list_ref = calloc(100,sizeof(uint64_t));
  CFArrayRef window_list_ref = SLSCopyWindowsWithOptionsAndTags(g_connection, cid, space_list_ref, options, &set_tags, &clear_tags);

  if (!window_list_ref) {
    // log_error("window list not found");
    goto err;
  }

  *count = CFArrayGetCount(window_list_ref);
  if ((*count) < 1) {
    //   log_error("window count less then one:%d",*count);
    goto out;
  }

  CFTypeRef query    = SLSWindowQueryWindows(g_connection, window_list_ref, *count);
  CFTypeRef iterator = SLSWindowQueryResultCopyWindows(query);

  if (window_list) {
    free(window_list);
  }
  window_list = calloc((*count) * 10, sizeof(uint32_t));
  while (SLSWindowIteratorAdvance(iterator)) {
    window_list[window_count++] = SLSWindowIteratorGetWindowID(iterator);
  }

  CFRelease(query);
  CFRelease(iterator);
out:
  CFRelease(window_list_ref);
err:
  CFRelease(space_list_ref);
  return(window_list);
}   /* space_window_list_for_connection */

bool window_id_is_minimized(int window_id){
  bool is_min = false;
  int  space_minimized_window_qty;
  int  spaces_qty = total_spaces();

  for (int s = 1; s < spaces_qty && is_min == false; s++) {
    uint32_t *minimized_window_list = space_minimized_window_list(s, &space_minimized_window_qty);
    for (int i = 0; i < space_minimized_window_qty && is_min == false; i++) {
      if (minimized_window_list[i] == (uint32_t)window_id) {
        is_min = true;
      }
    }
    if (minimized_window_list) {
      free(minimized_window_list);
    }
  }
  return(is_min);
}

bool space_is_user(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 0);
}

bool space_is_fullscreen(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 4);
}

bool space_is_system(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 2);
}

bool space_is_visible(uint64_t sid){
  return(sid == display_space_id(space_display_id(sid)));
}

uint64_t display_space_id(uint32_t did){
  CFStringRef uuid = display_uuid(did);

  if (!uuid) {
    return(0);
  }

  uint64_t sid = SLSManagedDisplayGetCurrentSpace(g_connection, uuid);

  CFRelease(uuid);

  return(sid);
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
}

struct Vector *get_display_ids_v(){
  struct Vector     *ids                = vector_new();
  size_t            displays_qty        = 0;
  CGDirectDisplayID *display_ids        = calloc(MAX_DISPLAYS, sizeof(CGDirectDisplayID));
  CGError           get_displays_result = CGGetActiveDisplayList(UCHAR_MAX, display_ids, &displays_qty);

  if (get_displays_result == kCGErrorSuccess) {
    for (size_t i = 0; i < displays_qty && i < MAX_DISPLAYS; i++) {
      size_t display_id = (size_t)display_ids[i];
      if (display_id > 0) {
        vector_push(ids, (void *)display_id);
      }
    }
  }
  return(ids);
}
