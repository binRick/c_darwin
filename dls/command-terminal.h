#ifndef DLS_COMMAND_TERMINAL_H
#define DLS_COMMAND_TERMINAL_H
#include "dls/dls.h"
int _command_terminal_kitty(void);
int _command_terminal_kitty_pids(void);
#define COMMON_OPTIONS_TERMINAL \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
#define COMMON_OPTIONS_TERMINAL_KITTY \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
#define COMMON_OPTIONS_TERMINAL_KITTY_PIDS \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
#define COMMON_OPTIONS_TERMINAL_KITTY_LIST \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
#define COLOR_TERMINAL    "\x1b[38;2;151;252;252m"
#define COLOR_KITTY       "\x1b[38;2;252;151;252m"
#define COLOR_PID         "\x1b[38;2;252;151;252m"
#define ICON_KITTY        "🙀"
#define ICON_PID          "🙀"
#define ICON_TERMINAL     "💻"
#define SUBCOMMANDS_TERMINAL_KITTY_LIST
#define SUBCOMMANDS_TERMINAL_KITTY_PIDS
#define SUBCOMMANDS_TERMINAL_KITTY          \
  CREATE_SUBCOMMAND(TERMINAL_KITTY_LIST, ), \
  CREATE_SUBCOMMAND(TERMINAL_KITTY_PIDS, ),
#define SUBCOMMANDS_TERMINAL                                     \
  CREATE_SUBCOMMAND(TERMINAL_KITTY, SUBCOMMANDS_TERMINAL_KITTY), \

#define ADD_TERMINAL_COMMANDS()                                                                  \
  COMMAND(ICON_TERMINAL, TERMINAL, "terminal", COLOR_TERMINAL, "Terminals", 0)                   \
  COMMAND(ICON_KITTY, TERMINAL_KITTY, "kitty", COLOR_KITTY, "Kitty", 0)                          \
  COMMAND(ICON_LIST, TERMINAL_KITTY_LIST, "ls", COLOR_LIST, "List", *_command_terminal_kitty)    \
  COMMAND(ICON_PID, TERMINAL_KITTY_PIDS, "ps", COLOR_PID, "PIDs", *_command_terminal_kitty_pids) \

#define ADD_TERMINAL_SUBCOMMANDS()                   \
  CREATE_SUBCOMMAND(TERMINAL, SUBCOMMANDS_TERMINAL), \

#endif
