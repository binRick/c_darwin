#pragma once
#ifndef KITTY_CMDS_DEFINED
#define KITTY_CMDS_DEFINED
#include "c_vector/include/vector.h"
#include "parson/parson.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define __KITTY_GET_COLORS_CMD__    "\x1bP@kitty-cmd{\"cmd\":\"get-colors\",\"version\":[0,25,2],\"no_response\":false}\x1b\\"

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


struct Vector *get_kitty_procs(const char *KITTY_LS_RESPONSE);

#endif
