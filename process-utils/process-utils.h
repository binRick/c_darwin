#pragma once
#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H
#include <Carbon/Carbon.h>
#include <fnmatch.h>
#include <libproc.h>
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

pid_t PSN2PID(ProcessSerialNumber psn);
ProcessSerialNumber PID2PSN(pid_t pid);
int get_focused_pid();
#endif
