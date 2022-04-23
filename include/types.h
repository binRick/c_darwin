#ifndef TYPES_H
#define TYPES_H
#include <Carbon/Carbon.h>
#include <fnmatch.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>

typedef struct pid_stats_t pid_stats_t;
struct pid_stats_t {
  int fds_qty;
  int open_files_qty;
  int connections_qty;
  int sockets_qty;
  int listening_ports_qty;
};
/**********************************/
typedef struct focused_t focused_t;
struct focused_t {
  char  *name;
  pid_t pid;
};


#endif
