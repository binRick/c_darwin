#pragma once
#ifndef MISSION_CONTROL_UTILS_H
#define MISSION_CONTROL_UTILS_H

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
#include "app/utils/utils.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"

///////////////////////////////////////////////////
int mission_control_index(uint64_t sid);
#endif
