#pragma once
#ifndef LS_WIN_COMMANDS_H
#define LS_WIN_COMMANDS_H
#include "darwin-ls/darwin-ls.h"
#include "icon-utils/icon-utils.h"
#include "output-utils/output-utils.h"
////////////////////////////////////////////
typedef struct optparse_opt (^common_option_b)(struct args_t *args);
#include "ansi-codes/ansi-codes.h"
#define COLOR_FOCUS      "\x1b[38;2;151;252;50m" AC_UNDERLINE
#define COLOR_SHOW       "\x1b[38;2;50;252;142m" AC_UNDERLINE
#define COLOR_LIST       "\x1b[38;2;50;175;252m" AC_UNDERLINE
#define COLOR_SPACE      "\x1b[38;2;97;252;50m" AC_UNDERLINE
#define COLOR_INFO       "\x1b[38;2;189;50;252m" AC_UNDERLINE
#define COLOR_DEBUG      "\x1b[38;2;252;50;127m" AC_UNDERLINE
#define COLOR_WINDOW     "\x1b[38;2;252;163;50m" AC_UNDERLINE
#define COLOR_MOVE       "\x1b[38;2;252;83;50m" AC_UNDERLINE
#define COLOR_START      "\x1b[38;2;62;252;50m" AC_UNDERLINE
#define COLOR_CAPTURE    "\x1b[38;2;50;252;50m" AC_UNDERLINE
#define COLOR_GET        "\x1b[38;2;252;50;252m" AC_UNDERLINE
#define COLOR_SET        "\x1b[38;2;252;163;252m" AC_UNDERLINE
#define COLOR_HELP       "\x1b[38;2;50;50;252m" AC_UNDERLINE
enum check_command_type_t {
  CHECK_COMMAND_WINDOW_ID = 1,
  CHECK_COMMAND_WIDTH,
  CHECK_COMMAND_HEIGHT,
  CHECK_COMMAND_WIDTH_GROUP,
  CHECK_COMMAND_HEIGHT_GROUP,
  CHECK_COMMAND_OUTPUT_MODE,
  CHECK_COMMAND_OUTPUT_FILE,
  CHECK_COMMAND_INPUT_FILE,
  CHECK_COMMAND_OUTPUT_ICNS_FILE,
  CHECK_COMMAND_INPUT_ICNS_FILE,
  CHECK_COMMAND_INPUT_PNG_FILE,
  CHECK_COMMAND_OUTPUT_PNG_FILE,
  CHECK_COMMAND_APPLICATION_PATH,
  CHECK_COMMAND_ICON_SIZE,
  CHECK_COMMAND_RESIZE_FACTOR,
  CHECK_COMMAND_XML_FILE,
  CHECK_COMMAND_TYPES_QTY,
};
enum common_option_group_t {
  COMMON_OPTION_GROUP_WIDTH_OR_HEIGHT = 1,
  COMMON_OPTION_GROUPS_QTY,
};
enum common_option_name_t {
  COMMON_OPTION_SPACE_ID = 1,
  COMMON_OPTION_WINDOW_ID,
  COMMON_OPTION_WINDOW_WIDTH,
  COMMON_OPTION_WINDOW_HEIGHT,
  COMMON_OPTION_WINDOW_WIDTH_GROUP,
  COMMON_OPTION_WINDOW_HEIGHT_GROUP,
  COMMON_OPTION_WINDOW_X,
  COMMON_OPTION_WINDOW_Y,
  COMMON_OPTION_VERBOSE,
  COMMON_OPTION_OUTPUT_MODE,
  COMMON_OPTION_HELP,
  COMMON_OPTION_CURRENT_SPACE,
  COMMON_OPTION_OUTPUT_FILE,
  COMMON_OPTION_INPUT_FILE,
  COMMON_OPTION_OUTPUT_ICNS_FILE,
  COMMON_OPTION_INPUT_ICNS_FILE,
  COMMON_OPTION_OUTPUT_PNG_FILE,
  COMMON_OPTION_INPUT_PNG_FILE,
  COMMON_OPTION_APPLICATION_PATH,
  COMMON_OPTION_ICON_SIZE,
  COMMON_OPTION_XML_FILE,
  COMMON_OPTION_RESIZE_FACTOR,
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
  COMMAND_HTTPSERVER,
  COMMAND_STICKY_WINDOW,
  COMMAND_MENU_BAR,
  COMMAND_DOCK,
  COMMAND_APPS,
  COMMAND_PROCESSES,
  COMMAND_USB_DEVICES,
  COMMAND_MONITORS,
  COMMAND_FONTS,
  COMMAND_KITTYS,
  COMMAND_ALACRITTYS,
  COMMAND_CAPTURE_WINDOW,
  COMMAND_SAVE_APP_ICON_PNG,
  COMMAND_SET_APP_ICON_PNG,
  COMMAND_SAVE_APP_ICON_ICNS,
  COMMAND_SET_APP_ICON_ICNS,
  COMMAND_ICON_INFO,
  COMMAND_GRAYSCALE_PNG_FILE,
  COMMAND_PARSE_XML_FILE,
  COMMAND_APP_PLIST_INFO_PATH,
  COMMAND_APP_ICNS_PATH,
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
common_option_b     common_options_b[COMMON_OPTION_NAMES_QTY + 1];

#endif
