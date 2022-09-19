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

bool run_osascript(char *OSASCRIPT_CONTENTS){
  bool     ok       = false;
  reproc_t *process = NULL;
  int      r        = REPROC_ENOMEM;

  process = reproc_new();
  if (process == NULL) {
    goto finish;
  }
  const char *cmd[] = { "/usr/bin/osascript", NULL };

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

  log_info("ran osascript with result %d", r);

  return(ok);
}
