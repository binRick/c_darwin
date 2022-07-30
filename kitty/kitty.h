#pragma once
////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
////////////////////////////////////////////////////////////////////////
#include "c_vector/include/vector.h"
#include "parson/parson.h"
#include "process/process.h"
////////////////////////////////////////////////////////////////////////
#define __KITTY_GET_COLORS_CMD__    "\x1bP@kitty-cmd{\"cmd\":\"get-colors\",\"version\":[0,25,2],\"no_response\":false}\x1b\\"
////////////////////////////////////////////////////////////////////////
struct Vector *get_kitty_procs(const char *KITTY_LS_RESPONSE);

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
////////////////////////////////////////////////////////////////////////
kitty_process_t *get_kitty_process_t(const size_t PID);
kitty_listen_on_t *parse_kitten_listen_on(char *KITTY_LISTEN_ON);

////////////////////////////////////////////////////////////////////////
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

typedef struct {
  char *key;
  char *val;
} process_env_t;
struct kitty_foreground_process_t {
  int  pid;
  char *cwd, *cmdline;
} kitty_foreground_process_t;

struct kitty_window_t {
  int           id, lines, pid, columns, foreground_processes_qty;
  bool          is_focused, is_self;
  char          *cwd, *title;
  struct Vector *foreground_processes;
} kitty_window_t;

struct kitty_tab_t {
  int           id, windows_qty;
  bool          is_focused;
  char          *layout, *title;
  struct Vector *windows_v;
} kitty_tab_t;

struct kitty_proc_t {
  int           id, window_id, tabs_qty;
  bool          is_focused;
  struct Vector *tabs_v;
} kitty_proc_t;

