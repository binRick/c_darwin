#pragma once
#ifndef LS_WIN_COMMANDS_H
#define LS_WIN_COMMANDS_H
#include "ls-win/ls-win.h"

////////////////////////////////////////////
enum check_command_type_t {
  CHECK_COMMAND_WINDOW_ID = 1,
  CHECK_COMMAND_WIDTH,
  CHECK_COMMAND_HEIGHT,
  CHECK_COMMAND_TYPES_QTY,
};
enum command_type_t {
  COMMAND_MOVE_WINDOW = 1,
  COMMAND_RESIZE_WINDOW,
  COMMAND_FOCUS_WINDOW,
  COMMAND_SET_WINDOW_SPACE,
  COMMAND_SET_WINDOW_ALL_SPACES,
  COMMAND_SET_SPACE,
  COMMAND_FOCUSED_WINDOW,
  COMMAND_FOCUSED_PID,
  COMMAND_FOCUSED_SPACE,
  COMMAND_WINDOWS,
  COMMAND_SPACES,
  COMMAND_DISPLAYS,
  COMMAND_DEBUG_ARGS,
  COMMAND_FOCUSED_SERVER,
  COMMAND_FOCUSED_CLIENT,
  COMMAND_STICKY_WINDOW,
  COMMAND_MENU_BAR,
  COMMAND_DOCK,
  COMMAND_TYPES_QTY,
};
enum color_types_t {
  COLOR_FOCUS,
  COLOR_TYPES_QTY,
};
struct item_color_t {
  const char *color;
};
struct cmd_t {
  void (*fxn)(void);
};
struct check_cmd_t {
  void (*fxn)(void);
  int  arg_data_type;
};

struct cmd_t        cmds[COMMAND_TYPES_QTY + 1];
struct check_cmd_t  check_cmds[CHECK_COMMAND_TYPES_QTY + 1];
struct item_color_t item_colors[COLOR_TYPES_QTY + 1];

#endif
