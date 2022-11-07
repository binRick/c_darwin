#pragma once
#ifndef DLS_COMMAND_display_H
#define DLS_COMMAND_display_H
#include "dls/dls.h"
#define COMMON_OPTIONS_DISPLAY_LIST             \
  common_options_b[COMMON_OPTION_OFFSET](args), \
  common_options_b[COMMON_OPTION_LIMIT](args),

#define COMMON_OPTIONS_DISPLAY \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args),

#define SUBCOMMANDS_DISPLAY \
  CREATE_SUBCOMMAND(DISPLAY_LIST, ),

#define ADD_DISPLAY_COMMANDS()                                           \
  COMMAND(ICON_DISPLAY, DISPLAY, "display", COLOR_DISPLAY, "Display", 0) \
  COMMAND(ICON_LIST, DISPLAY_LIST, "ls", COLOR_LIST, "List", *_command_list_display)

#define ADD_DISPLAY_SUBCOMMANDS() \
  CREATE_SUBCOMMAND(DISPLAY, SUBCOMMANDS_DISPLAY),

#endif
