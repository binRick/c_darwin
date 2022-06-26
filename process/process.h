#pragma once
#define DEBUG_PID_ENV    false
#include <stdio.h>
#include <sys/sysctl.h>
////////////////////////////////////////////////////////
//#include "dbg.h"
#include "fsio.h"
#include "parson.h"
#include "socket99/socket99.h"
#include "str-replace.h"
#include "stringbuffer.h"
#include "stringfn.h"
#include "stringfn.h"
#include "tiny-regex-c/re.h"
#include "vector.h"
//////////////////////////////////////////
typedef struct {
  unsigned long pid;
  int           window_id;
  struct Vector *env_v;
  char          *listen_on;
} kitty_process_t;
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
struct Vector *get_kitty_processes();
struct Vector *connect_kitty_processes(struct Vector *KittyProcesses_v);
char *get_process_cwd(int PID);
void connect_kitty_port(int PORT);

//////////////////////////////////////////
