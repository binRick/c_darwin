#pragma once
#ifndef DLS_EXTRACT_COMMANDS_H
#define DLS_EXTRACT_COMMANDS_H
#include "dls/meta.h"
void _command_extract_dev(void);
void _command_extract_image(void);
#define DLS_EXTRACT_COMMANDS_H
#define COMMON_OPTIONS_EXTRACT \
  COMMON_OPTIONS_BASE          \
    COMMON_OPTIONS_UI
#define SUBCOMMANDS_EXTRACT             \
  CREATE_SUBCOMMAND(EXTRACT_IMAGE, ),   \
  CREATE_SUBCOMMAND(EXTRACT_WINDOW, ),  \
  CREATE_SUBCOMMAND(EXTRACT_DISPLAY, ), \
  CREATE_SUBCOMMAND(EXTRACT_SPACE, ),   \
  CREATE_SUBCOMMAND(EXTRACT_DEV, ),
#define COMMON_OPTIONS_EXTRACT_WINDOW
#define COMMON_OPTIONS_EXTRACT_IMAGE                \
  common_options_b[COMMON_OPTION_HELP](args),       \
  common_options_b[COMMON_OPTION_CLEAR_SCREEN](args), \
  common_options_b[COMMON_OPTION_QUIET_MODE](args), \
  common_options_b[COMMON_OPTION_INPUT_FILE](args), \
  common_options_b[COMMON_OPTION_SEARCH_WORDS](args),\
  common_options_b[COMMON_OPTION_GLOBS](args), \
///////////////////////////////////////////////////////
#define COMMON_OPTIONS_EXTRACT_SPACE
#define COMMON_OPTIONS_EXTRACT_DEV
#define COMMON_OPTIONS_EXTRACT_DISPLAY
#define ADD_EXTRACT_COMMANDS()                                                                   \
  COMMAND(ICON_EXTRACT, EXTRACT, "extract", COLOR_DROID, "Extract", 0)                           \
  COMMAND(ICON_IMAGE, EXTRACT_IMAGE, "image", COLOR_IMAGE, "Image", *_command_extract_image)     \
  COMMAND(ICON_WINDOW, EXTRACT_WINDOW, "window", COLOR_WINDOW, "Window", *_command_extract)      \
  COMMAND(ICON_SPACE, EXTRACT_SPACE, "space", COLOR_SPACE, "Space", *_command_extract)           \
  COMMAND(ICON_DISPLAY, EXTRACT_DISPLAY, "display", COLOR_DISPLAY, "Display", *_command_extract) \
  COMMAND(ICON_DISPLAY, EXTRACT_DEV, "dev", COLOR_DISPLAY, "Dev", *_command_extract_dev)
#define ADD_EXTRACT_SUBCOMMANDS() \
  CREATE_SUBCOMMAND(EXTRACT, SUBCOMMANDS_EXTRACT),
#endif
