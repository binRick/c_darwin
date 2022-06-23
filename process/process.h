#pragma once
#define DEBUG_PID_ENV    false
#include <stdio.h>
#include <sys/sysctl.h>
////////////////////////////////////////////////////////
#include "submodules/meson_deps/submodules/c_stringfn/include/stringfn.h"
#include "submodules/meson_deps/submodules/c_vector/include/vector.h"
#include "submodules/meson_deps/submodules/dbg.h/dbg.h"
//////////////////////////////////////////
typedef struct {
  char *key;
  char *val;
} process_env_t;
//////////////////////////////////////////
struct Vector *get_process(int pid);
struct Vector *get_all_pids();
char *get_pid_cwd(int pid);

//////////////////////////////////////////
