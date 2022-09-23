#pragma once
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "frameworks/frameworks.h"
#include "frameworks/frameworks.h"
#include "libfort/lib/fort.h"
#include "log/log.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process-utils/process-utils.h"
#include "process/process.h"
#include "reproc/reproc/include/reproc/export.h"
#include "reproc/reproc/include/reproc/reproc.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
#include "submodules/log.h/log.h"
#include "system-utils/system-utils.h"
#include "timestamp/timestamp.h"
#include "which/src/which.h"
#include "wildcardcmp/wildcardcmp.h"
#include "window-utils/window-utils.h"
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
#include <libproc.h>
#define SYSTEM_PREFERENCES_SECURITY_APP_NAME       "System Preferences"
#define SYSTEM_PREFERENCES_SECURITY_WINDOW_NAME    "Security & Privacy"
extern struct Vector *get_window_infos_v();
static bool       PROCESS_UTILS_DEBUG_MODE         = false;
static bool       PROCESS_UTILS_VERBOSE_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__process_utils(void);
static const char *EXCLUDED_WINDOW_INFO_NAMES[] = {
  "com.apple.appkit.xpc.open",
  "OSDUIHelper",
  "com.apple.*",
  "QuickLookUIService",
  "LocalAuthentication*",
  "Control Center",
  "AccountProfileRemoteViewService",
  "Analytics & Improvements",
  "Dock",
  "Finder",
  "Installer Progress",
  "Photos",
  "Preview",
  "Spotlight",
  "SystemUIServer",
  "TextInputMenuAgent",
  "TextInputSwitcher",
  "Window Server",
  "com.apple.preference.security.*",
  "Install macOS*",
  "universalaccessd",
  "TextInputMenuAgent",
  "Spotlight",
  "Finder",
  "AccountProfileRemote",
  "Analytics & Improvements",
  "AccountProfileRemoteViewService",
  "Apple Advertising",
  "com.apple.preference*",
  "PrivacyAnalytics",
};
static size_t     EXCLUDED_WINDOW_INFO_NAMES_QTY = (sizeof(EXCLUDED_WINDOW_INFO_NAMES) / sizeof(EXCLUDED_WINDOW_INFO_NAMES[0]));
#define SET_FILE                                           \
  "tell application \"System Events\"\n"                   \
  "    keystroke \"G\" using {command down, shift down}\n" \
  "    delay 1\n"                                          \
  "    keystroke \"/path/to/file\"\n"                      \
  "    delay 1\n"                                          \
  "    keystroke return\n"                                 \
  "    delay 1\n"                                          \
  "    keystroke return\n"                                 \
  "    delay 1\n"                                          \
  "end tell\n"

#define CLOSE_SYSTEM_PREFERENCES                                         \
  "if running of application \"System Preferences\" then\n"              \
  "    try\n"                                                            \
  "        tell application \"System Preferences\" to quit\n"            \
  "    on error\n"                                                       \
  "        do shell script \"killall 'System Preferences'\"\n"           \
  "    end try\n"                                                        \
  "    delay 0.1\n"                                                      \
  "end if\n"                                                             \
  "repeat while running of application \"System Preferences\" is true\n" \
  "    delay 0.1\n"                                                      \
  "end repeat\n"
#define OPEN_SYSTEM_PREFERENCES_PRIVACY_ACCESSIBILITY_WINDOW_OSASCRIPT_CMD \
  "tell application \"System Preferences\"\n"                              \
  "    set securityPane to pane id \"com.apple.preference.security\"\n"    \
  "    tell securityPane to reveal anchor \"Privacy_Accessibility\"\n"     \
  "    activate\n"                                                         \
  "end tell\n"

///////////////////////////////////////////////////////////////////////////////
#define CLOSE_SYSTEM_PREFERENCES_AND_OPEN_SYSTEM_PREFERENCES_PRIVACY_ACCESSIBILITY_WINDOW_OSASCRIPT_CMD \
  CLOSE_SYSTEM_PREFERENCES OPEN_SYSTEM_PREFERENCES_PRIVACY_ACCESSIBILITY_WINDOW_OSASCRIPT_CMD

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

int get_window_id_display_id(size_t window_id){
  CFStringRef _uuid = SLSCopyManagedDisplayForWindow(CGSMainConnectionID(), window_id);
  CFUUIDRef   uuid  = CFUUIDCreateFromString(NULL, _uuid);

  CFRelease(_uuid);
  int id = CGDisplayGetDisplayIDFromUUID(uuid);

  CFRelease(uuid);
  return(id);
}

static bool get_application_has_accessibility_access_0(){
  bool            result;
  const void      *keys[]   = { kAXTrustedCheckOptionPrompt };
  const void      *values[] = { kCFBooleanTrue };

  CFDictionaryRef options;

  options = CFDictionaryCreate(kCFAllocatorDefault,
                               keys, values, sizeof(keys) / sizeof(*keys),
                               &kCFCopyStringDictionaryKeyCallBacks,
                               &kCFTypeDictionaryValueCallBacks);

  result = AXIsProcessTrustedWithOptions(options);
  CFRelease(options);

  return(result);
}

static bool get_application_has_accessibility_access_1(){
  if (AXAPIEnabled() != 0) {
    return(true);
  }
  if (AXIsProcessTrusted() != 0) {
    return(true);
  }
  return(false);
}

bool focus_pid(pid_t pid) {
  ProcessSerialNumber psn;

  GetProcessForPID(pid, &psn);
  SetFrontProcessWithOptions(&psn, kSetFrontProcessFrontWindowOnly);

  AXValueRef     temp;
  CGPoint        windowPosition;
  AXUIElementRef frontMostApp;
  AXUIElementRef frontMostWindow;

  frontMostApp = AXUIElementCreateApplication(pid);
  AXUIElementSetAttributeValue(frontMostApp, kAXFrontmostAttribute, kCFBooleanTrue);   // make application frontmost

  AXUIElementCopyAttributeValue(frontMostApp, kAXFocusedWindowAttribute, (CFTypeRef *)&frontMostWindow);
  AXUIElementSetAttributeValue(frontMostWindow, kAXMainAttribute, kCFBooleanTrue);   // make window frontmost

  AXUIElementCopyAttributeValue(frontMostWindow, kAXPositionAttribute, (CFTypeRef *)&temp);
  AXValueGetValue(temp, kAXValueCGPointType, &windowPosition);
  CFRelease(temp);
  windowPosition.x = 0;
  windowPosition.y = -33;
  temp             = AXValueCreate(kAXValueCGPointType, &windowPosition);
  AXUIElementSetAttributeValue(frontMostWindow, kAXPositionAttribute, temp);
  CFRelease(temp);

  CFRelease(frontMostWindow);
  CFRelease(frontMostApp);
  return(((get_focused_pid() == pid) ? true : false));
}

int get_focused_pid(){
  ProcessSerialNumber psn;

  GetFrontProcess(&psn);
  pid_t pid = PSN2PID(psn);

  if (pid > 1) {
    return((int)pid);
  }

  CFArrayRef window_list = CGWindowListCopyWindowInfo(
    kCGWindowListExcludeDesktopElements | kCGWindowListOptionAll,
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

void process_info_release(struct process_info_t *I){
  if (I) {
    if (I->open_ports_v) {
      vector_release(I->open_ports_v);
    }
    if (I->open_files_v) {
      vector_release(I->open_files_v);
    }
    if (I->open_connections_v) {
      for (size_t i = 0; i < vector_size(I->open_connections_v); i++) {
        struct open_connection_t *oc = (struct open_connection_t *)vector_get(I->open_connections_v, i);
        if (oc) {
          free(oc);
        }
      }
      vector_release(I->open_connections_v);
    }
  }
}

struct Vector *get_all_process_infos_v(){
  struct Vector *pids_v = get_all_processes();
  struct Vector *PI     = vector_new();

  for (size_t i = 0; i < vector_size(pids_v); i++) {
    struct process_info_t *I = get_process_info((int)(size_t)vector_get(pids_v, i));
    if (I) {
      vector_push(PI, (void *)I);
    }
  }
  return(PI);
}

struct process_info_t *get_process_info(int pid){
  struct process_info_t *I = calloc(1, sizeof(struct process_info_t));

  I->pid                = pid;
  I->success            = false;
  I->open_ports_v       = vector_new();
  I->open_connections_v = vector_new();
  I->open_files_v       = vector_new();
  I->started            = timestamp();
  if (I->pid == 0) {
    log_error("Invalid PID %d", I->pid);
    return(NULL);
  }

  int bufferSize = proc_pidinfo(I->pid, PROC_PIDLISTFDS, 0, 0, 0);

  if (bufferSize == -1) {
    log_error("Failed to get PID %d Open File Handles", I->pid);
    return(NULL);
  }

  struct proc_fdinfo *procFDInfo = (struct proc_fdinfo *)malloc(bufferSize);

  if (!procFDInfo) {
    log_error("Failed Allocate memory for PID %d info of size %d", I->pid, bufferSize);
    return(NULL);
  }
  proc_pidinfo(I->pid, PROC_PIDLISTFDS, 0, procFDInfo, bufferSize);
  int numberOfProcFDs = bufferSize / PROC_PIDLISTFD_SIZE;

  for (int i = 0; i < numberOfProcFDs; i++) {
    if (procFDInfo[i].proc_fdtype == PROX_FDTYPE_VNODE) {
      struct vnode_fdinfowithpath vnodeInfo;
      int                         bytesUsed = proc_pidfdinfo(I->pid, procFDInfo[i].proc_fd, PROC_PIDFDVNODEPATHINFO, &vnodeInfo, PROC_PIDFDVNODEPATHINFO_SIZE);
      if (bytesUsed == PROC_PIDFDVNODEPATHINFO_SIZE) {
        vector_push(I->open_files_v, (void *)(char *)vnodeInfo.pvip.vip_path);
      }
    } else if (procFDInfo[i].proc_fdtype == PROX_FDTYPE_SOCKET) {
      struct socket_fdinfo socketInfo;
      int                  bytesUsed = proc_pidfdinfo(I->pid, procFDInfo[i].proc_fd, PROC_PIDFDSOCKETINFO, &socketInfo, PROC_PIDFDSOCKETINFO_SIZE);
      if (bytesUsed == PROC_PIDFDSOCKETINFO_SIZE) {
        if (socketInfo.psi.soi_family == AF_INET && socketInfo.psi.soi_kind == SOCKINFO_TCP) {
          int localPort  = (int)ntohs(socketInfo.psi.soi_proto.pri_tcp.tcpsi_ini.insi_lport);
          int remotePort = (int)ntohs(socketInfo.psi.soi_proto.pri_tcp.tcpsi_ini.insi_fport);
          if (remotePort == 0) {
            vector_push(I->open_ports_v, (void *)(size_t)localPort);
          } else {
            struct open_connection_t *open_connection = calloc(1, sizeof(struct open_connection_t));
            open_connection->local_port  = localPort;
            open_connection->remote_port = remotePort;
            vector_push(I->open_connections_v, (void *)open_connection);
          }
        }
      }
    }
  }
  I->success = true;
  I->dur     = timestamp() - I->started;
  return(I);
} /* get_process_info */

pid_t ax_window_pid(AXUIElementRef ref){
  return(*(pid_t *)((void *)ref + 0x10));
}

uint32_t ax_window_id(AXUIElementRef ref){
  uint32_t wid = 0;

  _AXUIElementGetWindow(ref, &wid);
  return(wid);
}

int get_focused_window_id(){
  int            focused_pid = get_focused_pid();
  AXUIElementRef focused_app = AXUIElementCreateApplication(focused_pid);
  CFTypeRef      window_ref  = NULL;

  errno = 0;
  AXUIElementCopyAttributeValue(focused_app, kAXFocusedWindowAttribute, &window_ref);
  if (!window_ref) {
    log_error("Failed to copy Attribute for app");
    return(0);
  }
  int window_id = (int)ax_window_id(window_ref);

  CFRelease(window_ref);
  return(window_id);
}

#define WINDOW_DICTIONARY_INIT_WINDOW_LIST()                                                                                                   \
  int focused_pid = get_focused_pid();                                                                                                         \
  int           focused_window_id = get_focused_window_id();                                                                                   \
  CFArrayRef    windows_list      = CGWindowListCopyWindowInfo(kCGWindowListExcludeDesktopElements | kCGWindowListOptionAll, kCGNullWindowID); \
  size_t        windows_count     = (size_t)CFArrayGetCount(windows_list);                                                                     \
  struct Vector *window_infos_v   = vector_new();

#define WINDOW_DICTIONARY_INFO_REFS(DICT)                                                    \
  CFStringRef name_ref = CFDictionaryGetValue(DICT, kCGWindowOwnerName);                     \
  CFNumberRef         pid_ref           = CFDictionaryGetValue(DICT, kCGWindowOwnerPID);     \
  CFStringRef         title_ref         = CFDictionaryGetValue(DICT, kCGWindowName);         \
  CFNumberRef         layer_ref         = CFDictionaryGetValue(DICT, kCGWindowLayer);        \
  CFNumberRef         window_id_ref     = CFDictionaryGetValue(DICT, kCGWindowNumber);       \
  CFDictionaryRef     window_bounds     = CFDictionaryGetValue(DICT, kCGWindowBounds);       \
  CFNumberRef         memory_usage_ref  = CFDictionaryGetValue(DICT, kCGWindowMemoryUsage);  \
  CFNumberRef         sharing_state_ref = CFDictionaryGetValue(DICT, kCGWindowSharingState); \
  CFNumberRef         store_type_ref    = CFDictionaryGetValue(DICT, kCGWindowStoreType);    \
  CFBooleanRef        is_onscreen_ref   = CFDictionaryGetValue(DICT, kCGWindowIsOnscreen);   \
  CFDictionaryRef     window            = CFDictionaryCreateCopy(NULL, DICT);                \
  CFBooleanRef        is_minimized_ref;                                                      \
  ProcessSerialNumber psn = {};

#define WINDOW_DICTIONARY_VALIDATE_INFO_REFS() \
  if (!name_ref || !title_ref || !pid_ref || !layer_ref || !window_id_ref) continue;

#define WINDOW_DICTIONARY_INIT_ITEMS()                               \
  uint64_t window_id = 0, sharing_state = 0, store_type = 0;         \
  long long int        layer = 0, memory_usage = 0, pid = 0;         \
  bool                 is_onscreen = false, is_focused;              \
  char                 *name = NULL, *title = NULL;                  \
  struct window_info_t *i = calloc(1, sizeof(struct window_info_t)); \
  i->durs[WINDOW_INFO_DUR_TYPE_TOTAL].started = timestamp();         \
  i->started                                  = timestamp();

#define WINDOW_DICTIONARY_SET_ITEMS()                                                      \
  psn = PID2PSN(pid);                                                                      \
  CFNumberGetValue(layer_ref, CFNumberGetType(layer_ref), &layer);                         \
  CFNumberGetValue(memory_usage_ref, CFNumberGetType(memory_usage_ref), &memory_usage);    \
  CGRect bounds;                                                                           \
  CFNumberGetValue(store_type_ref, CFNumberGetType(store_type_ref), &store_type);          \
  CFNumberGetValue(sharing_state_ref, CFNumberGetType(sharing_state_ref), &sharing_state); \
  CFNumberGetValue(window_id_ref, CFNumberGetType(window_id_ref), &window_id);             \
  CFNumberGetValue(pid_ref, CFNumberGetType(pid_ref), &pid);                               \
  if (is_onscreen_ref) is_onscreen = CFBooleanGetValue(is_onscreen_ref);                   \
  CGRectMakeWithDictionaryRepresentation(window_bounds, &bounds);                          \
  name                 = cfstring_copy(name_ref);                                          \
  if (title_ref) title = cfstring_copy(title_ref);                                         \
  is_focused           = (                                                                 \
    (size_t)focused_pid == (size_t)pid                                                     \
    && (size_t)focused_window_id == (size_t)window_id                                      \
    )                                                                                      \
                            ? true : false;

#define WINDOW_DICTIONARY_VALIDATE_INFO_ITEMS() \
  if (!name || strlen(name) < 1 || pid < 1 || !title) { free(i);  continue; }

#define WINDOW_DICTIONARY_SET_WINDOW_INFO()                                                                          \
  i->window_id                                       = (size_t)window_id;                                            \
  i->window                                          = window;                                                       \
  i->pid                                             = (pid_t)pid;                                                   \
  i->psn                                             = &psn;                                                         \
  i->name                                            = (name != NULL) ? strdup(name) : "";                           \
  i->title                                           = (title != NULL) ? strdup(title) : "";                         \
  i->memory_usage                                    = (size_t)memory_usage;                                         \
  i->is_minimized                                    = false;                                                        \
  i->durs[WINDOW_INFO_DUR_TYPE_IS_MINIMIZED].started = timestamp();                                                  \
  errno                                              = 0;                                                            \
  AXError err = AXUIElementCopyAttributeValue(i->window, kAXMinimizedAttribute, &is_minimized_ref);                  \
  if (err == kAXErrorSuccess)                                                                                        \
  i->is_minimized                                = CFBooleanGetValue(is_minimized_ref);                              \
  i->durs[WINDOW_INFO_DUR_TYPE_IS_MINIMIZED].dur = timestamp() - i->durs[WINDOW_INFO_DUR_TYPE_IS_MINIMIZED].started; \
  i->layer                                       = (int)layer;                                                       \
  i->sharing_state                               = (int)sharing_state;                                               \
  i->store_type                                  = (int)store_type;                                                  \
  i->is_onscreen                                 = is_onscreen;                                                      \
  i->is_focused                                  = is_focused;                                                       \
  i->rect                                        = (CGRect)bounds;                                                   \
  i->display_id                                  = (size_t)get_window_id_display_id(i->window_id);                   \
  int        wid = (int)(i->window_id);                                                                              \
  i->durs[WINDOW_INFO_DUR_TYPE_SPACE_ID].started = timestamp();                                                      \
  CFArrayRef window_list_ref = cfarray_of_cfnumbers(&wid, sizeof(int), 1, kCFNumberSInt32Type);                      \
  CFArrayRef space_list_ref  = SLSCopySpacesForWindows(g_connection, 0x7, window_list_ref);                          \
  int        space_qty       = CFArrayGetCount(space_list_ref);                                                      \
  int        space_id        = 0;                                                                                    \
  if (space_qty == 1) {                                                                                              \
    CFNumberRef id_ref = CFArrayGetValueAtIndex(space_list_ref, 0);                                                  \
    CFNumberGetValue(id_ref, CFNumberGetType(id_ref), &space_id);                                                    \
    i->space_id = (size_t)(space_id);                                                                                \
  }                                                                                                                  \
  i->durs[WINDOW_INFO_DUR_TYPE_SPACE_ID].dur = timestamp() - i->durs[WINDOW_INFO_DUR_TYPE_SPACE_ID].started;         \
  i->dur                                     = timestamp() - i->started;                                             \
  i->durs[WINDOW_INFO_DUR_TYPE_TOTAL].dur    = timestamp() - i->durs[WINDOW_INFO_DUR_TYPE_TOTAL].started;            \
  if (PROCESS_UTILS_VERBOSE_DEBUG_MODE)                                                                              \
  log_info("space qty:%d|space id:%d|wid:%lu", space_qty, space_id, i->window_id);
#define WINDOW_INFO_VECTOR_PUSH_LOGIC()                                                    \
  bool is_excluded = false;                                                                \
  for (size_t ei = 0; ei < EXCLUDED_WINDOW_INFO_NAMES_QTY && is_excluded == false; ei++) { \
    if (wildcardcmp(EXCLUDED_WINDOW_INFO_NAMES[ei], i->name) == 1) is_excluded = true;     \
  }                                                                                        \
                                                                                           \
  if (is_excluded == false) {                                                              \
    vector_push(window_infos_v, (void *)i);                                                \
  }

#define WINDOW_DICTIONARY_PRINT_ITEMS(FILE_DESCRIPTOR)                        \
  fprintf(FILE_DESCRIPTOR,                                                    \
          " name:%s,window id:%lld,pid:%lld,title:%s,layer:%lld,"             \
          "  size:%dx%d,pos:%dx%d,mem:%lld,sharingstate:%lld,storetype:%lld," \
          "  onscreen:%s,"                                                    \
          "\n",                                                               \
          name,                                                               \
          window_id,                                                          \
          pid,                                                                \
          title,                                                              \
          layer,                                                              \
          (int)bounds.size.height, (int)bounds.size.width,                    \
          (int)bounds.origin.x, (int)bounds.origin.y,                         \
          memory_usage,                                                       \
          sharing_state,                                                      \
          store_type,                                                         \
          is_onscreen?"Yes":"No"                                              \
          );

#define WINDOW_DICTIONARY_FREE_ITEMS() \
  if (name) free(name);                \
  if (title) free(title);
struct Vector *get_window_pid_infos(pid_t pid){
  struct Vector *window_infos_v = get_window_infos_v(), *pid_window_infos_v = vector_new();

  for (size_t i = 0; i < vector_size(window_infos_v); i++) {
    struct window_info_t *wi = (struct window_info_t *)vector_get(window_infos_v, i);
    if (wi->pid == pid) {
      vector_push(pid_window_infos_v, (void *)wi);
    }else{
      free(wi);
    }
  }
  return(pid_window_infos_v);
}

struct Vector *get_display_id_window_ids_v(uint32_t display_id){
  struct Vector        *v       = vector_new();
  struct Vector        *windows = get_window_infos_v();
  struct window_info_t *w;

  for (size_t i = 0; i < vector_size(windows); i++) {
    w = (struct window_info_t *)vector_get(windows, i);
    if (w->display_id == display_id) {
      vector_push(v, (void *)w->display_id);
    }
  }
  return(v);
}
struct Vector *get_window_infos_v(){
  unsigned long started = timestamp();

  WINDOW_DICTIONARY_INIT_WINDOW_LIST()
  for (size_t i = 0; i < windows_count; ++i) {
    CFDictionaryRef window_info = CFArrayGetValueAtIndex(windows_list, i);
    if (!window_info) {
      continue;
    }
    WINDOW_DICTIONARY_INIT_ITEMS()
    WINDOW_DICTIONARY_INFO_REFS(window_info)
    WINDOW_DICTIONARY_VALIDATE_INFO_REFS()
    WINDOW_DICTIONARY_SET_ITEMS()
    WINDOW_DICTIONARY_VALIDATE_INFO_ITEMS()
    WINDOW_DICTIONARY_SET_WINDOW_INFO()
    WINDOW_INFO_VECTOR_PUSH_LOGIC()
    WINDOW_DICTIONARY_FREE_ITEMS()
  }
  CFRelease(windows_list);
  if (PROCESS_UTILS_DEBUG_MODE) {
    log_debug("Collected %lu Window Infos in %s",
              vector_size(window_infos_v),
              milliseconds_to_string(timestamp() - started)
              );
  }
  return(window_infos_v);
}

size_t run_osascript_system_prefs(){
  size_t               wid = 0;
  bool                 found = false;
  struct Vector        *pre_window_infos_v, *post_window_infos_v;
  struct window_info_t *w;
  {
    assert(run_osascript(CLOSE_SYSTEM_PREFERENCES) == true);
    pre_window_infos_v = get_window_infos_v();
    assert(run_osascript(OPEN_SYSTEM_PREFERENCES_PRIVACY_ACCESSIBILITY_WINDOW_OSASCRIPT_CMD) == true);
    post_window_infos_v = get_window_infos_v();
    if (PROCESS_UTILS_DEBUG_MODE) {
      log_info("%lu pre windows|%lu post windows", vector_size(pre_window_infos_v), vector_size(post_window_infos_v));
    }
    assert(vector_size(post_window_infos_v) > vector_size(pre_window_infos_v));
  }
  {
    for (size_t i = 0; i < vector_size(post_window_infos_v); i++) {
      for (size_t ii = 0; ii < vector_size(pre_window_infos_v); ii++) {
        if (found == false && ((struct window_info_t *)vector_get(pre_window_infos_v, ii))->window_id == ((struct window_info_t *)vector_get(post_window_infos_v, i))->window_id) {
          vector_remove(post_window_infos_v, i);
        }
      }
    }
  }
  pid_t  focused_pid       = get_focused_pid();
  size_t focused_window_id = (size_t)get_focused_window_id();
  {
    for (size_t i = 0; i < vector_size(post_window_infos_v); i++) {
      w = (struct window_info_t *)vector_get(post_window_infos_v, i);
      if (focused_pid == w->pid && focused_window_id == (size_t)w->window_id) {
        if (PROCESS_UTILS_DEBUG_MODE) {
          log_info("New Window #%lu> %s|pid:%d|pos:%dx%d|size:%dx%d|displayid:%lu|space_id:%lu|", w->window_id, w->name, w->pid, (int)w->rect.origin.x, (int)w->rect.origin.y, (int)w->rect.size.width, (int)w->rect.size.height, w->display_id, w->space_id);
        }
        if (strcmp(w->name, SYSTEM_PREFERENCES_SECURITY_APP_NAME) != 0 || strcmp(w->title, SYSTEM_PREFERENCES_SECURITY_WINDOW_NAME) != 0) {
          continue;
        }
        wid = (w->window_id > wid) ? w->window_id : wid;
      }
    }
  }

  return(wid);
} /* run_osascript_system_prefs */

bool run_osascript(char *OSASCRIPT_CONTENTS){
  bool       ok       = false;
  reproc_t   *process = NULL;
  int        r        = REPROC_ENOMEM;
  const char *cmd[]   = { which("osascript"), NULL };

  process = reproc_new();
  if (process == NULL) {
    goto finish;
  }
  r = reproc_start(process, cmd, (reproc_options){ .redirect.err.type = REPROC_REDIRECT_STDOUT, .deadline = 1000 });
  if (r < 0) {
    goto finish;
  }
  r = reproc_write(process, (uint8_t *)OSASCRIPT_CONTENTS, strlen(OSASCRIPT_CONTENTS));
  r = reproc_close(process, REPROC_STREAM_IN);
  assert(r == 0);
  r = reproc_wait(process, REPROC_INFINITE);
  if (r < 0) {
    goto finish;
  }
  ok = true;

finish:
  reproc_destroy(process);

  if (r < 0) {
    fprintf(stderr, AC_RED "%s" AC_RESETALL "\n", reproc_strerror(r));
  }
  if (PROCESS_UTILS_DEBUG_MODE) {
    log_info("ran osascript with result %d", r);
  }
  return(ok);
}
static void __attribute__((constructor)) __constructor__process_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_PROCESS_UTILS") != NULL) {
    log_debug("Enabling Process Utils Debug Mode");
    PROCESS_UTILS_DEBUG_MODE = true;
  }
}
#include "window-info/window-info.h"
