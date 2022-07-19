#pragma once
#include <stdio.h>
#define DEBUG_PID_ENV    false
#include <sys/sysctl.h>
////////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_vector/include/vector.h"
#include "djbhash/src/djbhash.h"
#include "fsio.h"
#include "kitty/kitty.h"
#include "parson.h"
#include "socket99/socket99.h"
#include "str-replace.h"
#include "stringbuffer.h"
#include "stringfn.h"
#include "tiny-regex-c/re.h"
//////////////////////////////////////////
typedef struct {
  char *protocol;
  char *host;
  int  port;
} kitty_listen_on_t;
typedef struct {
  unsigned long pid;
  int           window_id;
  char          *listen_on;
  char          *install_dir;
  struct Vector *env_v;
} kitty_process_t;
typedef struct {
  char *key;
  char *val;
} process_env_t;
//////////////////////////////////////////
int *get_all_pids();
int *get_app_pids();
int *get_window_pids();
kitty_process_t *get_kitty_process_t(const int PID);
kitty_listen_on_t *parse_kitten_listen_on(char *KITTY_LISTEN_ON);
struct Vector *get_kitty_listen_ons();
struct Vector *get_all_processes();
struct Vector *get_process_env(int PID);
struct Vector *get_process_cmdline(int PID);
struct Vector *get_kitty_pids();
struct Vector *connect_kitty_processes(struct Vector *KittyProcesses_v);
char *kitty_tcp_cmd(const char *HOST, const int PORT, const char *KITTY_MSG);
char *kitty_cmd_data(const char *CMD_OUTPUT);
char *get_process_cwd(int PID);
void kitty_command(const char *HOST, const int PORT, const char *KITTY_MSG);
char *kitty_get_color(const char *COLOR_TYPE, const char *HOST, const int PORT);
struct Vector *kitty_get_color_types(const char *HOST, const int PORT);
struct kinfo_proc *proc_info_for_pid(pid_t pid);
int get_kinfo_proc(pid_t pid, struct kinfo_proc *kp);

//////////////////////////////////////////
//////////////////////////////////////////
