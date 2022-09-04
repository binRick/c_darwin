#pragma once
#ifndef LS_WIN_COMMANDS_H
#define LS_WIN_COMMANDS_H
#include "darwin-ls/darwin-ls.h"
////////////////////////////////////////////
typedef struct optparse_opt (^common_option_b)(struct args_t *args);
#include "ansi-codes/ansi-codes.h"
#define COLOR_FOCUS     "\x1b[38;2;151;252;50m" AC_UNDERLINE
#define COLOR_SHOW      "\x1b[38;2;50;252;142m" AC_UNDERLINE
#define COLOR_LIST      "\x1b[38;2;50;175;252m" AC_UNDERLINE
#define COLOR_SPACE     "\x1b[38;2;97;252;50m" AC_UNDERLINE
#define COLOR_INFO      "\x1b[38;2;189;50;252m" AC_UNDERLINE
#define COLOR_DEBUG     "\x1b[38;2;252;50;127m" AC_UNDERLINE
#define COLOR_WINDOW    "\x1b[38;2;252;163;50m" AC_UNDERLINE
#define COLOR_MOVE      "\x1b[38;2;252;83;50m" AC_UNDERLINE
#define COLOR_START     "\x1b[38;2;62;252;50m" AC_UNDERLINE
#define COLOR_HELP      "\x1b[38;2;50;186;252m" AC_UNDERLINE
const enum output_mode_type_t DEFAULT_OUTPUT_MODE;
enum output_mode_type_t {
  OUTPUT_MODE_TEXT = 1,
  OUTPUT_MODE_TABLE,
  OUTPUT_MODE_JSON,
  OUTPUT_MODES_QTY,
};
enum check_command_type_t {
  CHECK_COMMAND_WINDOW_ID = 1,
  CHECK_COMMAND_WIDTH,
  CHECK_COMMAND_HEIGHT,
  CHECK_COMMAND_OUTPUT_MODE,
  CHECK_COMMAND_TYPES_QTY,
};
enum common_option_name_t {
  COMMON_OPTION_SPACE_ID = 1,
  COMMON_OPTION_WINDOW_ID,
  COMMON_OPTION_WINDOW_WIDTH,
  COMMON_OPTION_WINDOW_HEIGHT,
  COMMON_OPTION_WINDOW_X,
  COMMON_OPTION_WINDOW_Y,
  COMMON_OPTION_VERBOSE,
  COMMON_OPTION_OUTPUT_MODE,
  COMMON_OPTION_HELP,
  COMMON_OPTION_CURRENT_SPACE,
  COMMON_OPTION_NAMES_QTY,
};
enum command_type_t {
  COMMAND_MOVE_WINDOW = 1,
  COMMAND_RESIZE_WINDOW,
  COMMAND_FOCUS_WINDOW,
  COMMAND_SET_WINDOW_SPACE,
  COMMAND_SET_WINDOW_ALL_SPACES,
  COMMAND_SET_SPACE,
  COMMAND_SET_SPACE_INDEX,
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
  COMMAND_APPS,
  COMMAND_TYPES_QTY,
};
enum color_types_t {
  COLOR_TYPE_FOCUS,
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
const char          *output_modes[OUTPUT_MODES_QTY + 1];
common_option_b     common_options_b[COMMON_OPTION_NAMES_QTY + 1];

#endif
