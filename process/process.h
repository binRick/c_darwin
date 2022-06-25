#pragma once
#define DEBUG_PID_ENV    false
#include <stdio.h>
#include <sys/sysctl.h>
////////////////////////////////////////////////////////
#include "dbg.h"
#include "stringfn.h"
#include "vector.h"
//////////////////////////////////////////
typedef struct {
  char *key;
  char *val;
} process_env_t;
//////////////////////////////////////////
int *get_all_pids();
int *get_app_pids();
int *get_window_pids();
struct Vector *get_all_processes();
struct Vector *get_process_env(int PID);
struct Vector *get_process_cmdline(int PID);
char *get_process_cwd(int PID);

//////////////////////////////////////////
