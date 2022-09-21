#pragma once
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
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
#include "submodules/log.h/log.h"
#include "system-utils/system-utils.h"
#include "timestamp/timestamp.h"
#include "which/src/which.h"
#include "window-utils/window-utils.h"
#include <libproc.h>
#define CLOSE_SYSTEM_PREFERENCES                                           \
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

static bool PROCESS_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__process_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_PROCESS_UTILS") != NULL) {
    log_debug("Enabling Process Utils Debug Mode");
    PROCESS_UTILS_DEBUG_MODE = true;
  }
}

///////////////////////////////////////////////////////////////////////////////
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

int get_focused_window_id(){
    int            focused_pid = get_focused_pid();
    AXUIElementRef focused_app = AXUIElementCreateApplication(focused_pid);
    CFTypeRef      window_ref  = NULL;

    AXUIElementCopyAttributeValue(focused_app, kAXFocusedWindowAttribute, &window_ref);
    if (!window_ref) {
      return(0);
    }

    int window_id = (int)ax_window_id(window_ref);

    CFRelease(window_ref);

    return(window_id);
}


  size_t run_osascript_system_prefs(){
    size_t wid = 0;
    bool found = false;
    assert(run_osascript(CLOSE_SYSTEM_PREFERENCES) == true);
    struct Vector *pre_window_infos_v = get_window_infos_v();
    assert(run_osascript(OPEN_SYSTEM_PREFERENCES_PRIVACY_ACCESSIBILITY_WINDOW_OSASCRIPT_CMD) == true);
    struct Vector *post_window_infos_v = get_window_infos_v();
    assert(vector_size(post_window_infos_v)>vector_size(pre_window_infos_v));
    if (PROCESS_UTILS_DEBUG_MODE) {
      log_info("%lu pre windows|%lu post windows", vector_size(pre_window_infos_v), vector_size(post_window_infos_v));
    }
    for (size_t i = 0; i < vector_size(post_window_infos_v); i++) {
      for (size_t ii = 0; ii < vector_size(pre_window_infos_v); ii++) {
        if (found == false && ((struct window_info_t *)vector_get(pre_window_infos_v, ii))->window_id == ((struct window_info_t *)vector_get(post_window_infos_v, i))->window_id) {
          vector_remove(post_window_infos_v, i);
        }
      }
    }
    for (size_t i = 0; i < vector_size(post_window_infos_v); i++) {
      struct window_info_t *w = (struct window_info_t *)vector_get(post_window_infos_v, i);
      if (get_focused_pid() == w->pid && (size_t)get_focused_window_id() == (size_t)w->window_id) {
        if (PROCESS_UTILS_DEBUG_MODE) {
          log_info("New Window #%lu> %s|pid:%d|pos:%dx%d|size:%dx%d|displayid:%lu|space_id:%lu|",
                   w->window_id, w->name, w->pid, (int)w->rect.origin.x, (int)w->rect.origin.y, (int)w->rect.size.width, (int)w->rect.size.height  ,
                   w->display_id, w->space_id
                   );
        }
        if (strcmp(w->name, "System Preferences") != 0 || strcmp(w->title, "Security & Privacy") != 0) {
          continue;
        }
        if (w->window_id > wid) {
          wid = w->window_id;
        }
      }
    }
    return(wid);
  }

bool run_osascript(char *OSASCRIPT_CONTENTS){
  bool     ok       = false;
  reproc_t *process = NULL;
  int      r        = REPROC_ENOMEM;
  const char *cmd[] = { which("osascript"), NULL };

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
  if(PROCESS_UTILS_DEBUG_MODE)
    log_info("ran osascript with result %d", r);
  return(ok);
}
