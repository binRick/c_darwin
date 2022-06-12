#include "types.h"
#include <libproc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/proc_info.h>
#define DEBUG_PID_STATS    false


static const char *USAGE                  = "Usage: %s pid\n";
static const char *INVALID_PID            = "Invalid pid: %s\n";
static const char *UNABLE_TO_GET_PROC_FDS = "Unable to get open file handles for %d\n";
static const char *OUT_OF_MEMORY          = "Out of memory. Unable to allocate buffer with %d bytes\n";
static const char *OPEN_FILE              = "Open file: %s\n";
static const char *LISTENING_ON_PORT      = "Listening on port: %d\n";
static const char *OPEN_SOCKET            = "Open socket: %d -> %d\n";


void model(){
  size_t len = 0;

  if (!sysctlbyname("hw.model", NULL, &len, NULL, 0) && len) {
    char *model = malloc(len * sizeof(char));
    if (!sysctlbyname("hw.model", model, &len, NULL, 0)) {
      fprintf(stderr, "\nlen:%d\n", len);
      fprintf(stderr, "\nmodel:%s\n", model);
    }
    free(model);
  }
}


pid_stats_t * get_pid_stats(const int pid){
  pid_stats_t *ps = malloc(sizeof(pid_stats_t));

  ps->connections_qty     = 0;
  ps->open_files_qty      = 0;
  ps->fds_qty             = 0;
  ps->sockets_qty         = 0;
  ps->listening_ports_qty = 0;

  if (pid == 0) {
    printf(INVALID_PID, pid);
    return(ps);
  }

  int bufferSize = proc_pidinfo(pid, PROC_PIDLISTFDS, 0, 0, 0);

  if (bufferSize == -1) {
    printf(UNABLE_TO_GET_PROC_FDS, pid);
    return(ps);
  }

  // Get the list of open FDs
  struct proc_fdinfo *procFDInfo = (struct proc_fdinfo *)malloc(bufferSize);

  if (!procFDInfo) {
    printf(OUT_OF_MEMORY, bufferSize);
    return(ps);
  }
  proc_pidinfo(pid, PROC_PIDLISTFDS, 0, procFDInfo, bufferSize);
  ps->fds_qty = bufferSize / PROC_PIDLISTFD_SIZE;
  for (int i = 0; i < ps->fds_qty; i++) {
    if (procFDInfo[i].proc_fdtype == PROX_FDTYPE_VNODE) {
      struct vnode_fdinfowithpath vnodeInfo;
      int                         bytesUsed = proc_pidfdinfo(pid, procFDInfo[i].proc_fd, PROC_PIDFDVNODEPATHINFO, &vnodeInfo, PROC_PIDFDVNODEPATHINFO_SIZE);
      if (bytesUsed == PROC_PIDFDVNODEPATHINFO_SIZE) {
        ps->open_files_qty++;
        if (DEBUG_PID_STATS) {
          printf(OPEN_FILE, vnodeInfo.pvip.vip_path);
        }
      }
    } else if (procFDInfo[i].proc_fdtype == PROX_FDTYPE_SOCKET) { // A socket is open
      ps->sockets_qty++;
      struct socket_fdinfo socketInfo;
      int                  bytesUsed = proc_pidfdinfo(pid, procFDInfo[i].proc_fd, PROC_PIDFDSOCKETINFO, &socketInfo, PROC_PIDFDSOCKETINFO_SIZE);
      if (bytesUsed == PROC_PIDFDSOCKETINFO_SIZE) {
        if (socketInfo.psi.soi_family == AF_INET && socketInfo.psi.soi_kind == SOCKINFO_TCP) {
          int localPort  = (int)ntohs(socketInfo.psi.soi_proto.pri_tcp.tcpsi_ini.insi_lport);
          int remotePort = (int)ntohs(socketInfo.psi.soi_proto.pri_tcp.tcpsi_ini.insi_fport);
          if (remotePort == 0) { // Remote port will be 0 when the FD represents a listening socket
            ps->listening_ports_qty++;
            if (DEBUG_PID_STATS) {
              printf(LISTENING_ON_PORT, localPort);
            }
          } else { // Remote port will be non-0 when the FD represents communication with a remote socket
            ps->connections_qty++;
            if (DEBUG_PID_STATS) {
              printf(OPEN_SOCKET, localPort, remotePort);
            }
          }
        }
      }
    }
  }

  return(ps);
} /* get_pid_stats */
