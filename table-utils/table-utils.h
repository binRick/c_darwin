#pragma once
#ifndef TABLE_UTILS_H
#define TABLE_UTILS_H
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
struct list_table_t {
  struct Vector *windows_v;
  bool          current_space_only;
  int           space_id;
};
int list_windows_table(void *ARGS);
int list_spaces_table(void *ARGS);
int list_displays_table(void *ARGS);

#endif
