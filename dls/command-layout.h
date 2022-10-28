#pragma once
#ifndef DLS_COMMAND_LAYOUT_H
#define DLS_COMMAND_LAYOUT_H
#include "dls/dls.h"
#define COMMON_OPTIONS_LAYOUT \
  COMMON_OPTIONS_UI
#define COMMON_OPTIONS_LAYOUT_TEST
#define COMMON_OPTIONS_LAYOUT_NAMES
#define COMMON_OPTIONS_LAYOUT_TEST
#define COMMON_OPTIONS_LAYOUT_LIST
#define COMMON_OPTIONS_LAYOUT_NAMES
#define COMMON_OPTIONS_LAYOUT_SHOW \
  common_options_b[COMMON_OPTION_LAYOUT_NAME](args),
#define COMMON_OPTIONS_LAYOUT_RENDER \
  common_options_b[COMMON_OPTION_LAYOUT_NAME](args),
#define COMMON_OPTIONS_LAYOUT_APPLY \
  common_options_b[COMMON_OPTION_LAYOUT_NAME](args),
#define SUBCOMMANDS_LAYOUT            \
  CREATE_SUBCOMMAND(LAYOUT_NAMES, ),  \
  CREATE_SUBCOMMAND(LAYOUT_LIST, ),   \
  CREATE_SUBCOMMAND(LAYOUT_APPLY, ),  \
  CREATE_SUBCOMMAND(LAYOUT_SHOW, ),   \
  CREATE_SUBCOMMAND(LAYOUT_RENDER, ), \
  CREATE_SUBCOMMAND(LAYOUT_TEST, ),
#define ADD_LAYOUT_COMMANDS()                                                                        \
  COMMAND(ICON_LAYOUT, LAYOUT, "layout", COLOR_LAYOUT, "Layout Manager", 0)                          \
  COMMAND(ICON_LIST, LAYOUT_LIST, "ls", COLOR_LIST, "List Layouts", *_command_layout_list)           \
  COMMAND(ICON_NAME, LAYOUT_NAMES, "names", COLOR_NAME, "List Layout Names", *_command_layout_names) \
  COMMAND(ICON_TEST, LAYOUT_TEST, "test", COLOR_TEST, "Test Layout", *_command_layout_test)          \
  COMMAND(ICON_APPLY, LAYOUT_APPLY, "apply", COLOR_APPLY, "Apply Layout", *_command_layout_apply)    \
  COMMAND(ICON_SHOW, LAYOUT_SHOW, "show", COLOR_SHOW, "Show Layout", *_command_layout_show)          \
  COMMAND(ICON_RENDER, LAYOUT_RENDER, "render", COLOR_RENDER, "Render Layout", *_command_layout_render)
#define ADD_LAYOUT_SUBCOMMANDS() \
  CREATE_SUBCOMMAND(LAYOUT, SUBCOMMANDS_LAYOUT),
#endif
