#pragma once
#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H
#include <Carbon/Carbon.h>
#include <fnmatch.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
///////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app-utils/app-utils.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
///////////////////////////////////////////////////
struct open_connection_t {
  int local_port, remote_port;
};
struct process_info_t {
  int           pid;
  bool          success;
  struct Vector *open_files_v, *open_connections_v, *open_ports_v;
  unsigned long started, dur;
};
struct process_info_t *get_process_info(int pid);
void process_info_release(struct process_info_t *I);
pid_t PSN2PID(ProcessSerialNumber psn);
ProcessSerialNumber PID2PSN(pid_t pid);
int get_focused_pid();
struct Vector *get_all_process_infos_v();
bool run_osascript(char *OSASCRIPT_CONTENTS);
size_t run_osascript_system_prefs();
int get_focused_window_id();
#endif
