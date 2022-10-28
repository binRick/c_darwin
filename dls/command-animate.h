#pragma once
#ifndef DLS_ANIMATE_COMMANDS_H
#define DLS_ANIMATE_COMMANDS_H
#include "capture/type/type.h"
#include "core/core.h"
#include "dls/meta.h"
#include "layout/utils/utils.h"
#define ICON_ANIMATE     "🐧"
#define COLOR_ANIMATE    "\x1b[38;2;151;252;50m"
#define COMMON_OPTIONS_ANIMATE                              \
  COMMON_OPTIONS_BASE                                       \
  COMMON_OPTIONS_CAPTURE_TYPE                               \
  COMMON_OPTIONS_UI                                         \
  COMMON_OPTIONS_CAPTURE_RESULT_OPTIONS                     \
  COMMON_OPTIONS_CAPTURE_OPTIONS                            \
  COMMON_OPTIONS_SIZE                                       \
    common_options_b[COMMON_OPTION_DURATION_SECONDS](args), \
    common_options_b[COMMON_OPTION_FRAME_RATE](args),
#define COMMON_OPTIONS_ANIMATE_WINDOW \
  COMMON_OPTIONS_ANIMATE
#define COMMON_OPTIONS_ANIMATE_SPACE \
  COMMON_OPTIONS_ANIMATE
#define COMMON_OPTIONS_ANIMATE_DISPLAY \
  COMMON_OPTIONS_ANIMATE
#define SUBCOMMANDS_ANIMATE             \
  CREATE_SUBCOMMAND(ANIMATE_WINDOW, ),  \
  CREATE_SUBCOMMAND(ANIMATE_DISPLAY, ), \
  CREATE_SUBCOMMAND(ANIMATE_SPACE, ),
#define ADD_ANIMATE_COMMANDS()                                                                      \
  COMMAND(ICON_ANIMATE, ANIMATE, "animate", COLOR_ANIMATE, "Animated Capture", 0)                   \
  COMMAND(ICON_WINDOW, ANIMATE_WINDOW, "window", COLOR_WINDOW, "Animate Window", *_command_animate) \
  COMMAND(ICON_SPACE, ANIMATE_SPACE, "space", COLOR_SPACE, "Animate Space", *_command_animate)      \
  COMMAND(ICON_DISPLAY, ANIMATE_DISPLAY, "display", COLOR_DISPLAY, "Animate Display", *_command_animate)
#define ADD_ANIMATE_SUBCOMMANDS() \
  CREATE_SUBCOMMAND(ANIMATE, SUBCOMMANDS_ANIMATE),

#endif
