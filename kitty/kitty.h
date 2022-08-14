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
#include "subprocess.h/subprocess.h"
////////////////////////////////////////////////////////////////////////
#define __KITTY_GET_COLORS_CMD__               "\x1bP@kitty-cmd{\"cmd\":\"get-colors\",\"version\":[0,25,2],\"no_response\":false}\x1b\\"
#define KITTY_LS_CMD                           "\x1bP@kitty-cmd{\"cmd\":\"ls\",\"version\":[0,25,2],\"no_response\":false}\x1b\\"
/*
 * const char KITTY_LS_NO_ENV_CMD[]                 = "\x1bP@kitty-cmd{\"cmd\":\"ls\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"all_env_vars\":false}}\x1b\\";
 * const char KITTY_GET_TEXT[]                      = "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false}\x1b\\";
 * const char KITTY_GET_ANSI_TEXT[]                 = "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":true}}\x1b\\";
 */
#define KITTY_LS_ALL_ENV_CMD                   "\x1bP@kitty-cmd{\"cmd\":\"ls\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"all_env_vars\":true}}\x1b\\"
#define KITTY_QUERY_TERMINAL_CMD               "\x1bP@kitty-cmd{\"cmd\":\"kitten\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"kitten\":\"query_terminal\"}}\x1b\\"
#define KITTY_GET_ANSI_CURSOR_TEXT             "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":true,\"cursor\":false}}\x1b\\"
#define KITTY_GET_ANSI_CURSOR_ALL_TEXT         "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":true,\"cursor\":false,\"extent\":\"all\"}}\x1b\\"
#define KITTY_GET_ANSI_CURSOR_SCREEN_TEXT      "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":true,\"cursor\":false,\"extent\":\"screen\"}}\x1b\\"
#define KITTY_GET_ANSI_CURSOR_LAST_CMD_TEXT    "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":true,\"cursor\":false,\"extent\":\"last_non_empty_output\"}}\x1b\\"
#define KITTY_GET_NO_ANSI_TEXT                 "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":false}}\x1b\\"
#define KITTY_SELF_LS                          "@ ls"                 "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":false}}\x1b\\"

////////////////////////////////////////////////////////////////////////
struct Vector *get_kitty_procs(const char *KITTY_LS_RESPONSE);
struct kitty_process_communication_result_t *kitty_process_communication(struct Vector *CMD_VECTOR);
bool kitty_set_tab_title(char *TAB_TITLE);
char *kitty_get_text();
bool kitty_set_window_title(char *TITLE);
char *kitty_get_ls();
bool kitty_set_layout(char *LAYOUT);
bool kitty_set_tab_color(char *MODE, char *COLOR);
char *kitty_get_colors();
bool kitty_send_text(char *TEXT);
char *kitty_get_last_cmd_output();
char *kitty_ls_kittens();
char *kitty_query_terminal();
char *kitty_list_fonts();

#define READ_BUFFER_SIZE    1024 * 16
struct kitty_process_communication_result_t {
  struct subprocess_s  *subprocess;
  int                  subprocess_result;
  int                  subprocess_join_result;
  int                  subprocess_exited;
  char                 *READ_STDOUT, *READ_STDERR, *kitty_exec_path;
  char                 **kitty_command;
  struct  StringBuffer *STDOUT_BUFFER, *STDERR_BUFFER;
  size_t               stdout_bytes_read, stderr_bytes_read;
  char                 stdout_buffer[READ_BUFFER_SIZE], stderr_buffer[READ_BUFFER_SIZE];
};
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
typedef struct {
  char *key;
  char *val;
} process_env_t;
struct kitty_foreground_process_t {
  int  pid;
  char *cwd, *cmdline;
} kitty_foreground_process_t;
enum kitty_connect_types_T {
  KITTY_CONNECT_TYPE_TCP,
  KITTY_CONNECT_TYPE_LOCAL,
  KITTY_CONNECT_TYPES_QTY,
};
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
/////////////////////////////////////////////////////////////////////////////////////
struct Vector *get_kitty_listen_ons();
struct Vector *get_all_processes();
struct Vector *get_process_env(int PID);
struct Vector *get_process_cmdline(int PID);
struct Vector *get_child_pids(int pid);
int get_kitty_pid_windowid(int PID);
struct Vector *get_kitty_pids();
bool kitty_draw_image(void);
bool kitty_set_font_size(int FONT_SIZE);
static bool kitty_run_at_command(char *COMMAND);
struct Vector *connect_kitty_processes(struct Vector *KittyProcesses_v);

char *kitty_tcp_cmd(const char *HOST, const int PORT, const char *KITTY_MSG);
char *kitty_cmd_data(const char *CMD_OUTPUT);
bool kitty_clear_screen(void);
char *get_process_cwd(int PID);
void kitty_command(const char *HOST, const int PORT, const char *KITTY_MSG);
char *kitty_get_color(const char *COLOR_TYPE, const char *HOST, const int PORT);
struct Vector *kitty_get_color_types(const char *HOST, const int PORT);
