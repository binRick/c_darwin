#pragma once
#include <stdio.h>
#define DEBUG_PID_ENV    false
#include <sys/sysctl.h>
////////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_vector/vector/vector.h"
#include "kitty/kitty.h"
#include "stringfn.h"
//////////////////////////////////////////
int *get_all_pids();
int *get_app_pids();
int *get_window_pids();
struct kinfo_proc *proc_info_for_pid(pid_t pid);
int get_kinfo_proc(pid_t pid, struct kinfo_proc *kp);
char * pid_path(pid_t pid);
struct Vector *get_all_processes();
struct Vector *get_process_env(int process);
char *get_my_cwd();
int get_process_ppid(int pid);
struct Vector *get_process_ppids(int pid);
struct Vector *get_child_pids(int PID);

//////////////////////////////////////////
