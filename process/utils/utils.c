#pragma once
#ifndef PROCESS_UTILS_C
#define PROCESS_UTILS_C
#include "log/log.h"
#include "process/process.h"
#include <CoreFoundation/CoreFoundation.h>
#include <dlfcn.h>
#include <pthread.h>
////////////////////////
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <libproc.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
/////////////////////////
#include "active-app/active-app.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "core/utils/utils.h"
#include "frameworks/frameworks.h"
#include "libfort/lib/fort.h"
#include "log/log.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "reproc/reproc/include/reproc/export.h"
#include "reproc/reproc/include/reproc/reproc.h"
#include "space/utils/utils.h"
#include "string-utils/string-utils.h"
#include "submodules/log/log.h"
#include "timestamp/timestamp.h"
#include "which/src/which.h"
#include "wildcardcmp/wildcardcmp.h"
#include "window/utils/utils.h"
//#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
#include <libproc.h>
#include <sys/resource.h>
#define SYSTEM_PREFERENCES_SECURITY_APP_NAME       "System Preferences"
#define SYSTEM_PREFERENCES_SECURITY_WINDOW_NAME    "Security & Privacy"
#define OSASCRIPT_PATH                             "/usr/bin:/usr/local/bin"
static bool       PROCESS_UTILS_DEBUG_MODE         = false;
static bool       PROCESS_UTILS_VERBOSE_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__process_utils(void);
static const char *EXCLUDED_WINDOW_INFO_NAMES[] = {
  "Window Server",
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

void get_cputime(){
  struct rusage r_usage;

  if (getrusage(RUSAGE_SELF, &r_usage))
    log_error("Failed to get rusage");
  else{
    struct kinfo_proc info;
    int               mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, (int)getpid() };
    size_t            len   = sizeof info;
    memset(&info, 0, len);
    sysctl(mib, (sizeof(mib) / sizeof(int)), &info, &len, NULL, 0);

    struct timeval wall_start = info.kp_proc.p_starttime;

    struct timeval wall_now;
    gettimeofday(&wall_now, NULL);
    double         start     = wall_start.tv_sec + (wall_start.tv_usec / (float)1000000);
    double         stop      = wall_now.tv_sec + (wall_now.tv_usec / (float)1000000);
    double         wall_time = stop - start;

    printf("Total User CPU = %ld.%d\n",
           r_usage.ru_utime.tv_sec,
           r_usage.ru_utime.tv_usec);
    printf("Total System CPU = %ld.%d\n",
           r_usage.ru_stime.tv_sec,
           r_usage.ru_stime.tv_usec);
    double cpu_time   = r_usage.ru_utime.tv_sec + r_usage.ru_utime.tv_usec / (float)1000000 + r_usage.ru_stime.tv_sec + r_usage.ru_stime.tv_usec / (float)1000000;
    double percentage = cpu_time / wall_time;
    log_debug("time:%f", cpu_time);
    log_debug("%%:%f", percentage);
    log_debug("start:%f", start);
    log_debug("time:%f", wall_time);
  }
}

static bool get_application_has_accessibility_access_1(){
  if (AXAPIEnabled() != 0)
    return(true);

  if (AXIsProcessTrusted() != 0)
    return(true);

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

  if (pid > 1)
    return((int)pid);

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
    if (I->open_ports_v)
      vector_release(I->open_ports_v);
    if (I->open_files_v)
      vector_release(I->open_files_v);
    if (I->open_connections_v) {
      for (size_t i = 0; i < vector_size(I->open_connections_v); i++) {
        struct open_connection_t *oc = (struct open_connection_t *)vector_get(I->open_connections_v, i);
        if (oc)
          free(oc);
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
    if (I)
      vector_push(PI, (void *)I);
  }
  return(PI);
}

struct process_info_t *get_process_info(int pid){
  struct process_info_t *I = calloc(1, sizeof(struct process_info_t));

  I->pid = pid;
  if (I->pid == 0) {
    log_error("Invalid PID %d", I->pid);
    return(NULL);
  }
  I->success            = false;
  I->open_ports_v       = vector_new();
  I->open_connections_v = vector_new();
  I->open_files_v       = vector_new();
  I->env_v        = vector_new();
 // I->env_v       = get_process_env(I->pid);
  I->child_pids_v = get_child_pids(I->pid);
  I->ppid         = get_process_ppid(I->pid);
  I->ppids_v      = get_process_ppids(I->pid);
  I->started      = timestamp();

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
      if (bytesUsed == PROC_PIDFDVNODEPATHINFO_SIZE)
        vector_push(I->open_files_v, (void *)(char *)vnodeInfo.pvip.vip_path);
    } else if (procFDInfo[i].proc_fdtype == PROX_FDTYPE_SOCKET) {
      struct socket_fdinfo socketInfo;
      int                  bytesUsed = proc_pidfdinfo(I->pid, procFDInfo[i].proc_fd, PROC_PIDFDSOCKETINFO, &socketInfo, PROC_PIDFDSOCKETINFO_SIZE);
      if (bytesUsed == PROC_PIDFDSOCKETINFO_SIZE)
        if (socketInfo.psi.soi_family == AF_INET && socketInfo.psi.soi_kind == SOCKINFO_TCP) {
          int localPort  = (int)ntohs(socketInfo.psi.soi_proto.pri_tcp.tcpsi_ini.insi_lport);
          int remotePort = (int)ntohs(socketInfo.psi.soi_proto.pri_tcp.tcpsi_ini.insi_fport);
          if (remotePort == 0)
            vector_push(I->open_ports_v, (void *)(size_t)localPort);
          else {
            struct open_connection_t *open_connection = calloc(1, sizeof(struct open_connection_t));
            open_connection->local_port  = localPort;
            open_connection->remote_port = remotePort;
            vector_push(I->open_connections_v, (void *)open_connection);
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
    log_error("Failed to copy Attribute for focused pid %d", focused_pid);
    return(0);
  }
  int window_id = (int)ax_window_id(window_ref);

  CFRelease(window_ref);
  return(window_id);
}

#define WINDOW_DICTIONARY_INIT_WINDOW_LIST()                                                                                                 \
  int focused_window_id = get_focused_window_id();                                                                                           \
  CFArrayRef    windows_list    = CGWindowListCopyWindowInfo(kCGWindowListExcludeDesktopElements | kCGWindowListOptionAll, kCGNullWindowID); \
  size_t        windows_count   = (size_t)CFArrayGetCount(windows_list);                                                                     \
  struct Vector *window_infos_v = vector_new();                                                                                              \
  int           r               = -1;

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
  ProcessSerialNumber psn               = {};

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

#define WINDOW_DICTIONARY_SET_ITEMS()                                                                                                  \
  psn = PID2PSN(pid);                                                                                                                  \
  CFNumberGetValue(layer_ref, CFNumberGetType(layer_ref), &layer);                                                                     \
  CFNumberGetValue(memory_usage_ref, CFNumberGetType(memory_usage_ref), &memory_usage);                                                \
  CGRect bounds;                                                                                                                       \
  CFNumberGetValue(store_type_ref, CFNumberGetType(store_type_ref), &store_type);                                                      \
  CFNumberGetValue(sharing_state_ref, CFNumberGetType(sharing_state_ref), &sharing_state);                                             \
  CFNumberGetValue(window_id_ref, CFNumberGetType(window_id_ref), &window_id);                                                         \
  CFNumberGetValue(pid_ref, CFNumberGetType(pid_ref), &pid);                                                                           \
  if (is_onscreen_ref) is_onscreen = CFBooleanGetValue(is_onscreen_ref);                                                               \
  CGRectMakeWithDictionaryRepresentation(window_bounds, &bounds);                                                                      \
  name                 = cfstring_copy(name_ref);                                                                                      \
  if (title_ref) title = cfstring_copy(title_ref);                                                                                     \
  is_focused           = ((size_t)focused_window_id == (size_t)window_id) ? true : false;                                              \
  int      connection;                                                                                                                 \
  uint64_t set_tags      = 1;                                                                                                          \
  uint64_t clear_tags    = 0;                                                                                                          \
  int      did           = 0;                                                                                                          \
  int      space_count   = 0;                                                                                                          \
  int      display_count = 0;                                                                                                          \
  if (true) {                                                                                                                          \
    CGGetActiveDisplayList(0, NULL, &display_count);                                                                                   \
    if (PROCESS_UTILS_VERBOSE_DEBUG_MODE)                                                                                              \
    log_info("qty displays:%d", display_count);                                                                                        \
    CFArrayRef display_spaces_ref   = SLSCopyManagedDisplaySpaces(g_connection);                                                       \
    int        display_spaces_count = CFArrayGetCount(display_spaces_ref);                                                             \
    uint32_t   options              = true ? 0x7 : 0x2;                                                                                \
    for (int i = 0; i < display_spaces_count; ++i) {                                                                                   \
      CFDictionaryRef display_ref  = CFArrayGetValueAtIndex(display_spaces_ref, i);                                                    \
      CFArrayRef      spaces_ref   = CFDictionaryGetValue(display_ref, CFSTR("Spaces"));                                               \
      int             spaces_count = CFArrayGetCount(spaces_ref);                                                                      \
      if (PROCESS_UTILS_VERBOSE_DEBUG_MODE)                                                                                            \
      log_info("qty spaces:%d", spaces_count);                                                                                         \
      uint64_t space_list[spaces_count];                                                                                               \
      for (int j = 0; j < spaces_count; ++j) {                                                                                         \
        CFDictionaryRef space_ref = CFArrayGetValueAtIndex(spaces_ref, j);                                                             \
        CFNumberRef     sid_ref   = CFDictionaryGetValue(space_ref, CFSTR("id64"));                                                    \
        CFNumberGetValue(sid_ref, CFNumberGetType(sid_ref), &space_list[j]);                                                           \
        if (PROCESS_UTILS_VERBOSE_DEBUG_MODE)                                                                                          \
        log_info("space #%lu> ", space_list[j]);                                                                                       \
      }                                                                                                                                \
      CFArrayRef space_list_ref  = cfarray_of_cfnumbers(space_list, sizeof(uint64_t), spaces_count, kCFNumberSInt64Type);              \
      CFArrayRef window_list_ref = SLSCopyWindowsWithOptionsAndTags(g_connection, 0, space_list_ref, options, &set_tags, &clear_tags); \
      size_t     qty             = CFArrayGetCount(window_list_ref);                                                                   \
      if (PROCESS_UTILS_VERBOSE_DEBUG_MODE)                                                                                            \
      log_info("qty windows:%lu", qty);                                                                                                \
      CFTypeRef query        = SLSWindowQueryWindows(g_connection, window_list_ref, qty);                                              \
      CFTypeRef iterator     = SLSWindowQueryResultCopyWindows(query);                                                                 \
      int       window_count = 0;                                                                                                      \
      while (SLSWindowIteratorAdvance(iterator)) {                                                                                     \
        uint32_t   wid         = SLSWindowIteratorGetWindowID(iterator);                                                               \
        CFArrayRef window_list = CGWindowListCopyWindowInfo(kCGWindowListOptionAll | kCGWindowListOptionIncludingWindow, wid);         \
        int        wqty        = CFArrayGetCount(window_list);                                                                         \
        for (int I = 0; I < CFArrayGetCount(window_list); I++) {                                                                       \
          CFDictionaryRef     windict = CFArrayGetValueAtIndex(window_list, I);                                                        \
          CFNumberRef         owner_pid_ref = CFDictionaryGetValue(windict, kCGWindowOwnerPID);                                        \
          int                 owner_pid = 0, connection = 0;                                                                           \
          CFNumberGetValue(owner_pid_ref, CFNumberGetType(owner_pid_ref), &owner_pid);                                                 \
          ProcessSerialNumber tempPSN;                                                                                                 \
          GetProcessForPID(owner_pid, &tempPSN);                                                                                       \
          SLSGetConnectionIDForPSN(g_connection, &(tempPSN), &connection);                                                             \
          AXUIElementRef a;                                                                                                            \
          AXUIElementRef w;                                                                                                            \
          a = AXUIElementCreateApplication(owner_pid);                                                                                 \
          int            lvl = 0, lyr = 0;                                                                                             \
          SLSGetWindowLevel(connection, wid, &(lvl));                                                                                  \
          CFStringRef    nr = CFDictionaryGetValue(windict, kCGWindowName);                                                            \
          CFNumberRef    lr = CFDictionaryGetValue(windict, kCGWindowLayer);                                                           \
          CFNumberGetValue(lr, kCFNumberIntType, &lyr);                                                                                \
          char           *n              = cfstring_copy(nr);                                                                          \
          CFBooleanRef   is_onscreen_ref = CFDictionaryGetValue(windict, kCGWindowIsOnscreen);                                         \
          bool           os              = false;                                                                                      \
          if (is_onscreen_ref)                                                                                                         \
          os = CFBooleanGetValue(is_onscreen_ref);                                                                                     \
          if (PROCESS_UTILS_VERBOSE_DEBUG_MODE)                                                                                        \
          log_info("window #%lu> display:%d|pid:%d|conn:%d|lvl:%d|lyr:%d\n\t|name:%s|onscreen:%s|",                                    \
                   wid,                                                                                                                \
                   i,                                                                                                                  \
                   owner_pid, connection, lvl, lyr, n, os?"Yes":"No"                                                                   \
                   );                                                                                                                  \
        }                                                                                                                              \
      }                                                                                                                                \
    }                                                                                                                                  \
  }                                                                                                                                    \

#define WINDOW_DICTIONARY_VALIDATE_ID(ID)  \
  {                                        \
    r = ((size_t)window_id == (size_t)ID); \
  }                                        \
  if (r != 0) { free(i);  continue; }
#define WINDOW_DICTIONARY_VALIDATE_NAME(NAME)             \
  {                                                       \
    char *n = cfstring_copy(name_ref);                    \
    if (stringfn_ends_with(NAME, ".app"))                 \
    NAME = stringfn_substring(NAME, 0, strlen(NAME) - 4); \
    if (stringfn_ends_with(n, ".app"))                    \
    n = stringfn_substring(n, 0, strlen(n) - 4);          \
/*   if(!stringfn_ends_with(n,'|'))\
 * //      asprintf(&n,"%s|",n);\
 * //    if(!stringfn_ends_with(NAME,'|'))\
 * //      asprintf(&NAME,"%s|",NAME);*/                               \
    r = strcmp(stringfn_to_lowercase(n), stringfn_to_lowercase(NAME)); \
/*    struct StringFNStrings sp[2]; \
 *  sp[0] = stringfn_split(n,'|');\
 *  sp[1] = stringfn_split(NAME,'|');\
 *  for(int i = 0; r != 0 && i <sp[0].count;i++){\
 *    for(int q = 0; r != 0 && q <sp[1].count;q++){\
 *      r = strcmp(stringfn_to_lowercase(sp[0].strings[i]), stringfn_to_lowercase(sp[1].strings[q])); \
 *    }\
 *  }*/             \
    if (n) free(n); \
  }                 \
  if (r != 0) { free(i);  continue; }

#define WINDOW_DICTIONARY_VALIDATE_INFO_ITEMS()           \
  if (                                                    \
    !name                                                 \
    || strlen(name) < 1                                   \
    || pid < 1                                            \
    || !title                                             \
    || ((int)bounds.size.width * bounds.size.height <= 1) \
    ) {                                                   \
    free(i);                                              \
    continue;                                             \
  }

#define WINDOW_DICTIONARY_SET_WINDOW_INFO(BRIEF)                                                                         \
  i->window_id                                       = (size_t)window_id;                                                \
  i->window                                          = window;                                                           \
  i->pid                                             = (pid_t)pid;                                                       \
  i->psn                                             = &psn;                                                             \
  i->name                                            = (name != NULL) ? strdup(name) : "";                               \
  i->title                                           = (title != NULL) ? strdup(title) : "";                             \
  i->memory_usage                                    = (size_t)memory_usage;                                             \
  i->pid_app_list_qty                                = 0;                                                                \
  i->app_window_ids_v                                = vector_new();                                                     \
  i->app_window                                      = NULL;                                                             \
  i->is_minimized                                    = false;                                                            \
  i->is_fullscreen                                   = false;                                                            \
  i->can_minimize                                    = false;                                                            \
  i->durs[WINDOW_INFO_DUR_TYPE_IS_MINIMIZED].started = timestamp();                                                      \
  i->durs[WINDOW_INFO_DUR_TYPE_IS_MINIMIZED].dur     = timestamp() - i->durs[WINDOW_INFO_DUR_TYPE_IS_MINIMIZED].started; \
  i->layer                                           = (int)layer;                                                       \
  i->sharing_state                                   = (int)sharing_state;                                               \
  i->store_type                                      = (int)store_type;                                                  \
  i->is_onscreen                                     = is_onscreen;                                                      \
  i->is_focused                                      = is_focused;                                                       \
  i->rect                                            = (CGRect)bounds;                                                   \
  i->display_id                                      = (size_t)get_window_id_display_id(i->window_id);                   \
  i->space_id                                        = 0;                                                                \
  int wid = (int)(i->window_id);                                                                                         \
  i->durs[WINDOW_INFO_DUR_TYPE_SPACE_ID].started = timestamp();                                                          \
  if (true || BRIEF == false) {                                                                                          \
    CFArrayRef window_list_ref = cfarray_of_cfnumbers(&wid, sizeof(int), 1, kCFNumberSInt32Type);                        \
    CFArrayRef space_list_ref  = SLSCopySpacesForWindows(g_connection, 0x7, window_list_ref);                            \
    int        space_qty       = CFArrayGetCount(space_list_ref);                                                        \
    int        space_id        = 0;                                                                                      \
    if (space_qty == 1) {                                                                                                \
      CFNumberRef id_ref = CFArrayGetValueAtIndex(space_list_ref, 0);                                                    \
      CFNumberGetValue(id_ref, CFNumberGetType(id_ref), &space_id);                                                      \
      i->space_id = (size_t)(space_id);                                                                                  \
    }                                                                                                                    \
    if (PROCESS_UTILS_VERBOSE_DEBUG_MODE) {                                                                              \
      log_info("space qty:%d|space id:%d|wid:%lu", space_qty, space_id, i->window_id);                                   \
    }                                                                                                                    \
  }                                                                                                                      \
  i->durs[WINDOW_INFO_DUR_TYPE_SPACE_ID].dur = timestamp() - i->durs[WINDOW_INFO_DUR_TYPE_SPACE_ID].started;             \
  i->dur                                     = timestamp() - i->started;                                                 \
  i->durs[WINDOW_INFO_DUR_TYPE_TOTAL].dur    = timestamp() - i->durs[WINDOW_INFO_DUR_TYPE_TOTAL].started;

#define WINDOW_INFO_VECTOR_PUSH_LOGIC(BRIEF)                                                                                     \
  bool is_excluded = false;                                                                                                      \
  for (size_t ei = 0; ei < (size_t)EXCLUDED_WINDOW_INFO_NAMES_QTY && is_excluded == false; ei++) {                               \
    if (wildcardcmp(EXCLUDED_WINDOW_INFO_NAMES[ei], i->name) == 1) is_excluded = true;                                           \
  }                                                                                                                              \
  if (is_excluded == false) {                                                                                                    \
    if (true || BRIEF == false) {                                                                                                \
      if ((size_t)(i->pid) > (size_t)1) {                                                                                        \
        i->app = AXUIElementCreateApplication(i->pid);                                                                           \
        if (i->app) {                                                                                                            \
          i->level = 0;                                                                                                          \
          SLSGetWindowLevel(g_connection, i->window_id, &(i->level));                                                            \
          AXUIElementCopyAttributeValue(i->app, kAXWindowsAttribute, (CFTypeRef *)&(i->pid_app_list));                           \
          i->pid_app_list_qty = CFArrayGetCount(i->pid_app_list);                                                                \
          if (PROCESS_UTILS_DEBUG_MODE) {                                                                                        \
            log_info("%s> PID %d has %lu apps", i->name, i->pid, i->pid_app_list_qty);                                           \
          }                                                                                                                      \
          for (size_t x = 0; x < i->pid_app_list_qty; x++) {                                                                     \
            AXUIElementRef appWindow = CFArrayGetValueAtIndex(i->pid_app_list, x);                                               \
            CGWindowID     wid;                                                                                                  \
            _AXUIElementGetWindow(appWindow, &(wid));                                                                            \
            if ((size_t)wid != (size_t)(i->window_id)) continue;                                                                 \
            vector_push(i->app_window_ids_v, (void *)(size_t)wid);                                                               \
            if ((size_t)wid == (size_t)(i->window_id)) {                                                                         \
              i->app_window = appWindow;                                                                                         \
              CFBooleanRef is_fullscreen_ref;                                                                                    \
              const void   *role = NULL;                                                                                         \
              if (AXUIElementCopyAttributeValue(i->app_window, kAXRoleAttribute, &role) == kAXErrorSuccess) {                    \
                i->role = cfstring_copy(role);                                                                                   \
              }                                                                                                                  \
              const void *srole = NULL;                                                                                          \
              if (AXUIElementCopyAttributeValue(i->app_window, kAXSubroleAttribute, &srole) == kAXErrorSuccess) {                \
                i->sub_role = cfstring_copy(srole);                                                                              \
              }                                                                                                                  \
              if (PROCESS_UTILS_DEBUG_MODE) {                                                                                    \
                log_info("Window #%lu role: %s / sub role: %s", i->window_id, i->role, i->sub_role);                             \
              }                                                                                                                  \
              if (AXUIElementCopyAttributeValue(i->app_window, CFSTR("AXFullScreen"), &is_fullscreen_ref) == kAXErrorSuccess) {  \
                i->is_fullscreen = CFBooleanGetValue(is_fullscreen_ref);                                                         \
                CFRelease(is_fullscreen_ref);                                                                                    \
              }                                                                                                                  \
              if (AXUIElementIsAttributeSettable(i->app_window, kAXMinimizedAttribute, &(i->can_minimize)) == kAXErrorSuccess) { \
                CFBooleanRef is_minimized_ref;                                                                                   \
                if (AXUIElementCopyAttributeValue(i->app_window, kAXMinimizedAttribute, &is_minimized_ref) == kAXErrorSuccess) { \
                  i->is_minimized = CFBooleanGetValue(is_minimized_ref);                                                         \
                  CFRelease(is_minimized_ref);                                                                                   \
                }                                                                                                                \
              }                                                                                                                  \
            }                                                                                                                    \
          }                                                                                                                      \
        }                                                                                                                        \
      }                                                                                                                          \
    }                                                                                                                            \
    if (i->pid_app_list_qty == 0) {                                                                                              \
      continue;                                                                                                                  \
    }                                                                                                                            \
    if (!i->app_window) {                                                                                                        \
      continue;                                                                                                                  \
    }                                                                                                                            \
    if (!i->sub_role || strcmp(i->sub_role, "AXStandardWindow") != 0) {                                                          \
      continue;                                                                                                                  \
    }                                                                                                                            \
    vector_push(window_infos_v, (void *)i);                                                                                      \
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
    if ((size_t)(wi->pid) == (size_t)pid)
      vector_push(pid_window_infos_v, (void *)wi);
    else
      free(wi);
  }
  return(pid_window_infos_v);
}

struct Vector *get_display_id_window_ids_v(uint32_t display_id){
  struct Vector        *v       = vector_new();
  struct Vector        *windows = get_window_infos_v();
  struct window_info_t *w;

  for (size_t i = 0; i < vector_size(windows); i++) {
    w = (struct window_info_t *)vector_get(windows, i);
    if ((size_t)(w->display_id) == (size_t)display_id)
      vector_push(v, (void *)w->display_id);
  }
  return(v);
}

struct Vector *get_window_infos_brief_by_id(size_t ID){
  unsigned long started = timestamp();

  WINDOW_DICTIONARY_INIT_WINDOW_LIST()
  for (size_t i = 0; i < windows_count; ++i) {
    CFDictionaryRef window_info = CFArrayGetValueAtIndex(windows_list, i);
    if (!window_info)
      continue;
    WINDOW_DICTIONARY_INIT_ITEMS()
    WINDOW_DICTIONARY_INFO_REFS(window_info)
    WINDOW_DICTIONARY_VALIDATE_ID(ID)
    WINDOW_DICTIONARY_VALIDATE_INFO_REFS()
    WINDOW_DICTIONARY_SET_ITEMS()
    WINDOW_DICTIONARY_VALIDATE_INFO_ITEMS()
    WINDOW_DICTIONARY_SET_WINDOW_INFO(true)
    WINDOW_INFO_VECTOR_PUSH_LOGIC(true)
    WINDOW_DICTIONARY_FREE_ITEMS()
  }
  CFRelease(windows_list);
  if (PROCESS_UTILS_DEBUG_MODE)
    log_debug("Collected %lu Brief Window Infos in %s for ID %lu",
              vector_size(window_infos_v),
              milliseconds_to_string(timestamp() - started),
              ID
              );
  return(window_infos_v);
}
struct Vector *get_window_infos_brief_named_v(char *NAMED){
  unsigned long started = timestamp();

  WINDOW_DICTIONARY_INIT_WINDOW_LIST()
  for (size_t i = 0; i < windows_count; ++i) {
    CFDictionaryRef window_info = CFArrayGetValueAtIndex(windows_list, i);
    if (!window_info)
      continue;
    WINDOW_DICTIONARY_INIT_ITEMS()
    WINDOW_DICTIONARY_INFO_REFS(window_info)
    WINDOW_DICTIONARY_VALIDATE_NAME(NAMED)
    WINDOW_DICTIONARY_VALIDATE_INFO_REFS()
    WINDOW_DICTIONARY_SET_ITEMS()
    WINDOW_DICTIONARY_VALIDATE_INFO_ITEMS()
    WINDOW_DICTIONARY_SET_WINDOW_INFO(true)
    WINDOW_INFO_VECTOR_PUSH_LOGIC(true)
    WINDOW_DICTIONARY_FREE_ITEMS()
  }
  CFRelease(windows_list);
  if (PROCESS_UTILS_DEBUG_MODE)
    log_debug("Collected %lu Brief Window Infos in %s for name %s",
              vector_size(window_infos_v),
              milliseconds_to_string(timestamp() - started),
              NAMED
              );
  return(window_infos_v);
}
struct Vector *get_window_infos_brief_v(){
  unsigned long started = timestamp();

  WINDOW_DICTIONARY_INIT_WINDOW_LIST()
  for (size_t i = 0; i < windows_count; ++i) {
    CFDictionaryRef window_info = CFArrayGetValueAtIndex(windows_list, i);
    if (!window_info)
      continue;
    WINDOW_DICTIONARY_INIT_ITEMS()
    WINDOW_DICTIONARY_INFO_REFS(window_info)
    WINDOW_DICTIONARY_VALIDATE_INFO_REFS()
    WINDOW_DICTIONARY_SET_ITEMS()
    WINDOW_DICTIONARY_VALIDATE_INFO_ITEMS()
    WINDOW_DICTIONARY_SET_WINDOW_INFO(true)
    WINDOW_INFO_VECTOR_PUSH_LOGIC(true)
    WINDOW_DICTIONARY_FREE_ITEMS()
  }
  CFRelease(windows_list);
  if (PROCESS_UTILS_DEBUG_MODE)
    log_debug("Collected %lu Brief Window Infos in %s",
              vector_size(window_infos_v),
              milliseconds_to_string(timestamp() - started)
              );
  return(window_infos_v);
}

struct Vector *get_window_infos_v(){
  unsigned long started = timestamp();

  WINDOW_DICTIONARY_INIT_WINDOW_LIST()
  for (size_t i = 0; i < windows_count; ++i) {
    CFDictionaryRef window_info = CFArrayGetValueAtIndex(windows_list, i);
    if (!window_info)
      continue;
    WINDOW_DICTIONARY_INIT_ITEMS()
    WINDOW_DICTIONARY_INFO_REFS(window_info)
    WINDOW_DICTIONARY_VALIDATE_INFO_REFS()
    WINDOW_DICTIONARY_SET_ITEMS()
    WINDOW_DICTIONARY_VALIDATE_INFO_ITEMS()
    WINDOW_DICTIONARY_SET_WINDOW_INFO(false)
    WINDOW_INFO_VECTOR_PUSH_LOGIC(false)
    WINDOW_DICTIONARY_FREE_ITEMS()
  }
  CFRelease(windows_list);
  if (PROCESS_UTILS_DEBUG_MODE)
    log_debug("Collected %lu Window Infos in %s",
              vector_size(window_infos_v),
              milliseconds_to_string(timestamp() - started)
              );
  return(window_infos_v);
}

int open_system_preferences_get_window_id(){
  int                  wid = 0;
  bool                 found = false;
  struct Vector        *pre_window_infos_v, *post_window_infos_v;
  struct window_info_t *w;
  {
    if (!run_osascript(CLOSE_SYSTEM_PREFERENCES)) {
      log_error("Failed to open system preferences");
      return(-1);
    }
    pre_window_infos_v = get_window_infos_brief_named_v(SYSTEM_PREFERENCES_SECURITY_APP_NAME);
    if (PROCESS_UTILS_DEBUG_MODE)
      log_debug("\n%s\n", OPEN_SYSTEM_PREFERENCES_PRIVACY_ACCESSIBILITY_WINDOW_OSASCRIPT_CMD);
    if (!run_osascript(OPEN_SYSTEM_PREFERENCES_PRIVACY_ACCESSIBILITY_WINDOW_OSASCRIPT_CMD)) {
      log_error("Failed to open system preferences");
      return(-1);
    }
    post_window_infos_v = get_window_infos_brief_named_v(SYSTEM_PREFERENCES_SECURITY_APP_NAME);
    size_t retries = 5, retry_interval = 2000, retried = 0;
    while ((retried < retries) && (vector_size(post_window_infos_v) <= vector_size(pre_window_infos_v))) {
      post_window_infos_v = get_window_infos_brief_named_v(SYSTEM_PREFERENCES_SECURITY_APP_NAME);
      retried++;
      usleep(1000 * retry_interval);
    }
    if (PROCESS_UTILS_DEBUG_MODE)
      log_info("%lu pre windows|%lu post windows", vector_size(pre_window_infos_v), vector_size(post_window_infos_v));
    if (vector_size(post_window_infos_v) <= vector_size(pre_window_infos_v)) {
      log_error("Failed to find any new windows");
      return(-1);
    }
  }
  {
    for (size_t i = 0; i < vector_size(post_window_infos_v); i++)
      for (size_t ii = 0; ii < vector_size(pre_window_infos_v); ii++)
        if (found == false && ((struct window_info_t *)vector_get(pre_window_infos_v, ii))->window_id == ((struct window_info_t *)vector_get(post_window_infos_v, i))->window_id)
          vector_remove(post_window_infos_v, i);
  }
  unsigned long ts                = timestamp();
  pid_t         focused_pid       = get_focused_pid();
  size_t        focused_window_id = (size_t)get_focused_window_id();

  if (PROCESS_UTILS_DEBUG_MODE)
    log_info("got focused in %s", milliseconds_to_string(timestamp() - ts));
  {
    for (size_t i = 0; i < vector_size(post_window_infos_v); i++) {
      w = (struct window_info_t *)vector_get(post_window_infos_v, i);
      if (focused_pid == w->pid && focused_window_id == (size_t)w->window_id) {
        if (PROCESS_UTILS_DEBUG_MODE)
          log_info("New Window #%lu> %s|pid:%d|pos:%dx%d|size:%dx%d|displayid:%lu|space_id:%lu|", w->window_id, w->name, w->pid, (int)w->rect.origin.x, (int)w->rect.origin.y, (int)w->rect.size.width, (int)w->rect.size.height, w->display_id, w->space_id);
        if (strcmp(w->name, SYSTEM_PREFERENCES_SECURITY_APP_NAME) != 0 || strcmp(w->title, SYSTEM_PREFERENCES_SECURITY_WINDOW_NAME) != 0)
          continue;
        wid = (w->window_id > wid) ? w->window_id : wid;
      }
    }
  }

  return((int)wid);
} /* run_osascript_system_prefs */

bool process_utils_move_directory_contents(const char *SOURCE_DIRECTORY, const char *DESTINATION_DIRECTORY){
  unsigned long ts       = timestamp();
  bool          ok       = false;
  reproc_t      *process = NULL;
  int           r        = REPROC_ENOMEM;
  const char    *mv;

  mv = which("mv");
  if (!mv) {
    char *PATH = getenv("PATH");
    setenv("PATH", "/usr/bin:/usr/local/bin:/usr/local/opt/coreutils/libexec/gnubin/mv", 1);
    mv = which("mv");
    setenv("PATH", PATH, 1);
  }
  assert(mv);

  const char *cmd[] = { mv, SOURCE_DIRECTORY, DESTINATION_DIRECTORY, NULL };

  process = reproc_new();
  if (process == NULL) {
    log_error("reproc new failed");
    goto finish;
  }
  r = reproc_start(process, cmd, (reproc_options){ .redirect.err.type = REPROC_REDIRECT_STDOUT, .deadline = 1000 });
  if (r < 0) {
    log_error("reproc start failed with code %d", r);
    goto finish;
  }
  r = reproc_close(process, REPROC_STREAM_IN);
  assert(r == 0);
  r = reproc_wait(process, REPROC_INFINITE);
  if (r < 0)
    goto finish;
  ok = true;

finish:
  reproc_destroy(process);

  if (r < 0)
    fprintf(stderr, AC_RED "%s" AC_RESETALL "\n", reproc_strerror(r));
  if (PROCESS_UTILS_DEBUG_MODE)
    log_info("ran move command with result %d in %s", r, milliseconds_to_string(timestamp() - ts));
  return(ok);
} /* process_utils_move_directory_contents */

bool run_osascript(char *OSASCRIPT_CONTENTS){
  unsigned long ts       = timestamp();
  bool          ok       = false;
  reproc_t      *process = NULL;
  int           r        = REPROC_ENOMEM;
  const char    *osascript;

  osascript = which("osascript");
  if (!osascript) {
    char *PATH = getenv("PATH");
    setenv("PATH", OSASCRIPT_PATH, 1);
    osascript = which("osascript");
    setenv("PATH", PATH, 1);
  }
  assert(osascript);

  const char *cmd[] = { osascript, NULL };

  process = reproc_new();
  if (process == NULL) {
    log_error("reproc new failed");
    goto finish;
  }
  r = reproc_start(process, cmd, (reproc_options){ .redirect.err.type = REPROC_REDIRECT_STDOUT, .deadline = 1000 });
  if (r < 0) {
    log_error("reproc start failed with code %d", r);
    goto finish;
  }
  if (PROCESS_UTILS_DEBUG_MODE)
    log_info("%s", OSASCRIPT_CONTENTS);
  r = reproc_write(process, (uint8_t *)OSASCRIPT_CONTENTS, strlen(OSASCRIPT_CONTENTS));
  r = reproc_close(process, REPROC_STREAM_IN);
  assert(r == 0);
  r = reproc_wait(process, REPROC_INFINITE);
  if (r < 0)
    goto finish;
  ok = true;

finish:
  reproc_destroy(process);

  if (r < 0)
    fprintf(stderr, AC_RED "%s" AC_RESETALL "\n", reproc_strerror(r));
  if (PROCESS_UTILS_DEBUG_MODE)
    log_info("ran osascript with result %d in %s", r, milliseconds_to_string(timestamp() - ts));
  return(ok);
} /* run_osascript */
static void __attribute__((constructor)) __constructor__process_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_PROCESS_UTILS") != NULL) {
    log_debug("Enabling Process Utils Debug Mode");
    PROCESS_UTILS_DEBUG_MODE = true;
  }
}

static bool PROCESS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__process(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_PROCESS") != NULL) {
    log_debug("Enabling Process Debug Mode");
    PROCESS_DEBUG_MODE = true;
  }
}
#define GET_PID(proc)       (proc)->kp_proc.p_pid
#define IS_RUNNING(proc)    (((proc)->kp_proc.p_stat & SRUN) != 0)
#define ERROR_CHECK(fun) \
  do {                   \
    if (fun) {           \
      goto ERROR;        \
    }                    \
  } while (0)

char *get_pid_cwd(pid_t pid){
  char                      *cwd = NULL;
  struct proc_vnodepathinfo vpi;

  proc_pidinfo(pid, PROC_PIDVNODEPATHINFO, 0, &vpi, sizeof(vpi));
  if (strlen(vpi.pvi_cdir.vip_path) > 0)
    cwd = strdup(vpi.pvi_cdir.vip_path);
  return(cwd);
}

char *pid_path(pid_t pid){
  char *pathbuf = malloc(PATH_MAX);

  proc_pidpath(pid, pathbuf, sizeof(pathbuf));
  return(pathbuf);
}

struct Vector *get_process_ppids(int pid){
  int           _pid = pid;
  struct Vector *v   = vector_new();

  while (true) {
    int _ppid = get_process_ppid(_pid);
    vector_push(v, (void *)(size_t)_ppid);
    if (_ppid <= 1)
      break;
    _pid = _ppid;
  }

  return(v);
}

int get_kinfo_proc(pid_t pid, struct kinfo_proc *kp) {
  int    mib[4];
  size_t len;

  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PID;
  mib[3] = pid;

  len = sizeof(struct kinfo_proc);
  if (sysctl(mib, 4, kp, &len, NULL, 0) == -1)
    return(-1);

  if (len == 0)
    return(-1);

  return(0);
}

int get_process_ppid(int pid) {
  struct kinfo_proc kp;

  if (get_kinfo_proc((long)pid, &kp) == -1)
    return(-1);

  return((int)(long)kp.kp_eproc.e_ppid);
}

struct kinfo_proc *proc_info_for_pid(pid_t pid) {
  struct kinfo_proc *list = NULL;
  int               mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, pid };
  size_t            size  = 0;

  ERROR_CHECK(sysctl(mib, sizeof(mib) / sizeof(*mib), NULL, &size, NULL, 0));
  list = malloc(size);
  ERROR_CHECK(sysctl(mib, sizeof(mib) / sizeof(*mib), list, &size, NULL, 0));
  return(list);

ERROR:
  if (list)
    free(list);
  return(NULL);
}

static int get_argmax() {
  int    argmax, mib[] = { CTL_KERN, KERN_ARGMAX };
  size_t size = sizeof(argmax);

  if (sysctl(mib, 2, &argmax, &size, NULL, 0) == 0)
    return(argmax);

  return(0);
}

struct Vector *get_process_cmdline(int process){
  struct Vector *cmdline_v = vector_new();
  int           mib[3], nargs;
  char          *procargs = NULL, *arg_ptr, *arg_end, *curr_arg;
  size_t        len, argmax;

  if (process < 1)
    return(NULL);

  argmax = get_argmax();
  if (!argmax)
    return(NULL);

  procargs = (char *)malloc(argmax);
  if (procargs == NULL)
    return(NULL);

  mib[0] = CTL_KERN;
  mib[1] = KERN_PROCARGS2;
  mib[2] = (pid_t)process;
  if (sysctl(mib, 3, procargs, &argmax, NULL, 0) < 0) {
    free(procargs);
    return(NULL);
  }

  arg_end = &procargs[argmax];
  memcpy(&nargs, procargs, sizeof(nargs));
  arg_ptr  = procargs + sizeof(nargs);
  len      = strlen(arg_ptr);
  arg_ptr += len + 1;
  if (arg_ptr == arg_end) {
    free(procargs);
    return(NULL);
  }

  for ( ; arg_ptr < arg_end; arg_ptr++)
    if (*arg_ptr != '\0')
      break;

  curr_arg = arg_ptr;
  while (arg_ptr < arg_end && nargs > 0)
    if (*arg_ptr++ == '\0') {
      vector_push(cmdline_v, curr_arg);
      curr_arg = arg_ptr;
      nargs--;
    }
  if (procargs)
    free(procargs);
  return(cmdline_v);
} /* get_process_cmdline */

struct Vector *get_all_processes(){
  struct Vector *processes_v      = vector_new();
  pid_t         num               = proc_listallpids(NULL, 0);
  size_t        processes_len     = sizeof(pid_t) * num * 2;
  pid_t         *processes_buffer = malloc(processes_len);
  size_t        processes_qty     = proc_listallpids(processes_buffer, processes_len);

  for (size_t i = 0; i < processes_qty; i++)
    if ((long long)processes_buffer[i] > 0)
      vector_push(processes_v, (void *)(long long)processes_buffer[i]);
  free(processes_buffer);
  return(processes_v);
}

struct Vector *get_process_env(int process){
  struct Vector *process_env_v = vector_new();
  if (process <= 1)
    return(process_env_v);

  struct Vector          *vector = vector_new();
  struct StringFNStrings EnvSplit;
  int                    env_res = -1, nargs;
  char                   *procenv = NULL, *procargs, *arg_ptr, *arg_end, *env_start, *s;
  size_t                 argmax;

  argmax   = get_argmax();
  procargs = (char *)malloc(argmax);
  arg_end  = &procargs[argmax];
  memcpy(&nargs, procargs, sizeof(nargs));
  arg_ptr = procargs + sizeof(nargs);
  arg_ptr = memchr(arg_ptr, '\0', arg_end - arg_ptr);

  for ( ; arg_ptr < arg_end; arg_ptr++)
    if (*arg_ptr != '\0')
      break;
  while (arg_ptr < arg_end && nargs > 0)
    if (*arg_ptr++ == '\0')
      nargs--;
  env_start = arg_ptr;
  procenv   = calloc(1, arg_end - arg_ptr);

  while (*arg_ptr != '\0' && arg_ptr < arg_end) {
    s = memchr(arg_ptr + 1, '\0', arg_end - arg_ptr);
    if (s == NULL)
      break;
    memcpy(procenv + (arg_ptr - env_start), arg_ptr, s - arg_ptr);
    arg_ptr = s + 1;
    if (strlen(arg_ptr) < 3)
      continue;
    EnvSplit = stringfn_split(arg_ptr, '=');
    if (EnvSplit.count > 1)
      vector_push(vector, arg_ptr);
  }

  for (size_t i = 0; i < vector_size(vector); i++) {
    EnvSplit = stringfn_split(vector_get(vector, i), '=');
    process_env_t *pe = malloc(sizeof(process_env_t));
    pe->key = strdup(EnvSplit.strings[0]);
    pe->val = strdup(stringfn_join(EnvSplit.strings, "=", 1, EnvSplit.count - 1));
    if (false)
      fprintf(stderr,
              AC_RESETALL AC_BLUE "%s=>%s\n" AC_RESETALL, pe->key, pe->val);
    vector_push(process_env_v, pe);
  }
  if (true == DEBUG_PID_ENV)
    fprintf(stderr,
            "process:%d\n"
            "argmax:%lu\n"
            "env_res:%d\n"
            "nargs:%d\n"
            "progargs:%s\n"
            "arg_ptr:%s\n"
            "arg_end:%s\n"
            "vectors:%lu\n"
            "process env vector len:%lu\n",
            process,
            argmax,
            env_res,
            nargs,
            procargs,
            arg_ptr,
            arg_end,
            vector_size(vector),
            vector_size(process_env_v)
            );
  if (procargs != NULL)
    free(procargs);
  if (procenv != NULL)
    free(procenv);
  if (vector_size(vector) > 0)
    stringfn_release_strings_struct(EnvSplit);
  vector_release(vector);
  return(process_env_v);
} /* get_process_env */

char *get_my_cwd(){
  return(get_pid_cwd(getpid()));
}

struct Vector *get_child_pids(int PID){
  struct Vector *v      = vector_new();
  struct Vector *pids_v = get_all_processes();

  for (size_t i = 0; i < vector_size(pids_v); i++) {
    int pid = (int)(long long)vector_get(pids_v, i);
    if (pid < 2)
      continue;
    int ppid   = (int)(long)get_process_ppid(pid);
    int pppid  = (int)(long)get_process_ppid(ppid);
    int ppppid = (int)(long)get_process_ppid(pppid);
    if (ppid == PID || pppid == PID || ppppid == PID)
      vector_push(v, (void *)(long)pid);
  }
  return(v);
}

static bool darwin_pthread_setname_np(const char *name) {
  char namebuf[64];

  strncpy(namebuf, name, sizeof(namebuf) - 1);
  namebuf[sizeof(namebuf) - 1] = '\0';
  return(pthread_setname_np(namebuf) != 0);
}

typedef enum {
  kLSDefaultSessionID = -2,
} LSSessionID;
CFTypeRef LSGetCurrentApplicationASN(void);
OSStatus LSSetApplicationInformationItem(LSSessionID, CFTypeRef, CFStringRef, CFStringRef, CFDictionaryRef *);
CFDictionaryRef LSApplicationCheckIn(LSSessionID, CFDictionaryRef);
void LSSetApplicationLaunchServicesServerConnectionStatus(uint64_t, void *);

typedef struct {
  void        *application_services_handle;

  CFBundleRef launch_services_bundle;
  typeof(LSGetCurrentApplicationASN) * pLSGetCurrentApplicationASN;
  typeof(LSSetApplicationInformationItem) * pLSSetApplicationInformationItem;
  typeof(LSApplicationCheckIn) * pLSApplicationCheckIn;
  typeof(LSSetApplicationLaunchServicesServerConnectionStatus) * pLSSetApplicationLaunchServicesServerConnectionStatus;

  CFStringRef *display_name_key_ptr;
} launch_services_t;

static bool launch_services_init(launch_services_t *it) {
  enum {
    has_nothing,
    has_application_services_handle
  }    state = has_nothing;
  bool ret   = false;

  it->application_services_handle = dlopen("/System/Library/Frameworks/"
                                           "ApplicationServices.framework/"
                                           "Versions/Current/ApplicationServices",
                                           RTLD_LAZY | RTLD_LOCAL);
  if (!it->application_services_handle)
    goto done;
  ++state;

  it->launch_services_bundle = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.LaunchServices"));
  if (!it->launch_services_bundle)
    goto done;

#define LOAD_METHOD(name)                                                            \
  *(void **)(&it->p ## name) =                                                       \
    CFBundleGetFunctionPointerForName(it->launch_services_bundle, CFSTR("_" #name)); \
  if (!it->p ## name) goto done;

  LOAD_METHOD(LSGetCurrentApplicationASN)
  LOAD_METHOD(LSSetApplicationInformationItem)
  LOAD_METHOD(LSApplicationCheckIn)
  LOAD_METHOD(LSSetApplicationLaunchServicesServerConnectionStatus)

#undef LOAD_METHOD

  it->display_name_key_ptr =
    CFBundleGetDataPointerForName(it->launch_services_bundle, CFSTR("_kLSDisplayNameKey"));
  if (!it->display_name_key_ptr || !*it->display_name_key_ptr)
    goto done;

  ret = true;

done:
  switch (state) {
  case has_application_services_handle: if (!ret)
      dlclose(it->application_services_handle);
  case has_nothing:;
  }
  return(ret);
} /* launch_services_init */

static inline void launch_services_destroy(launch_services_t *it) {
  dlclose(it->application_services_handle);
}

static bool launch_services_set_process_title(const launch_services_t *it, const char *title) {
  enum {
    has_nothing,
    has_cf_title
  }           state = has_nothing;
  bool        ret   = false;

  static bool checked_in = false;

  CFTypeRef   asn;
  CFStringRef cf_title;

  if (!checked_in) {
    it->pLSSetApplicationLaunchServicesServerConnectionStatus(0, NULL);
    it->pLSApplicationCheckIn(kLSDefaultSessionID, CFBundleGetInfoDictionary(CFBundleGetMainBundle()));
    checked_in = true;
  }

  asn = it->pLSGetCurrentApplicationASN();
  if (!asn)
    goto done;

  cf_title = CFStringCreateWithCString(NULL, title, kCFStringEncodingUTF8);
  if (!cf_title)
    goto done;
  ++state;
  if (it->pLSSetApplicationInformationItem(kLSDefaultSessionID,
                                           asn,
                                           *it->display_name_key_ptr,
                                           cf_title,
                                           NULL) != noErr)
    goto done;
  ret = true;
done:
  switch (state) {
  case has_cf_title: CFRelease(cf_title);
  case has_nothing:;
  }

  return(ret);
} /* launch_services_set_process_title */

bool darwin_set_process_title(const char *title) {
  enum {
    has_nothing,
    has_launch_services
  }                 state = has_nothing;
  bool              ret   = false;
  launch_services_t launch_services;
  if (!launch_services_init(&launch_services))
    goto done;
  ++state;
  if (!launch_services_set_process_title(&launch_services, title))
    goto done;
  (void)darwin_pthread_setname_np(title);
  ret = true;
done:
  switch (state) {
  case has_launch_services: launch_services_destroy(&launch_services);
  case has_nothing:;
  }
  return(ret);
}

bool run_cmd_in_parent(char **cmd_a){
  bool     ok       = false;
  reproc_t *process = NULL;
  int      r        = REPROC_ENOMEM;

  process = reproc_new();
  if (process == NULL)
    goto finish;

  r = reproc_start(process, cmd_a,
                   (reproc_options){
    .redirect.parent = true,
    .deadline        = 15000,
  });
  if (r < 0)
    goto finish;

  r = reproc_wait(process, REPROC_INFINITE);
  if (r < 0)
    goto finish;
  ok = true;

finish:
  reproc_destroy(process);

  if (r < 0)
    fprintf(stderr, AC_RED "%s" AC_RESETALL "\n", reproc_strerror(r));

  return(ok);
}

#endif
