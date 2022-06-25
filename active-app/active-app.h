#pragma once
#include <assert.h>
#include <Carbon/Carbon.h>
#include <fnmatch.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
typedef struct focused_t focused_t;
struct focused_t {
  char  *name;
  pid_t pid;
};
struct screen_size {
  int x, y, w, h;
};
#include "stringfn.h"
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


focused_t * get_focused_process();
bool set_focused_process(focused_t *);
bool set_focused_pid(int pid);
int get_focused_pid();
ProcessSerialNumber get_focused_ProcessSerialNumber();
bool set_focused_ProcessSerialNumber(ProcessSerialNumber PSN);
ProcessSerialNumber PID2PSN(pid_t pid);
pid_t PSN2PID(ProcessSerialNumber psn);
int get_frontmost_application();
struct screen_size get_window_size();
