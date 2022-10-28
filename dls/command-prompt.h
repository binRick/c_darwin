#pragma once
#ifndef DLS_COMMAND_PROMPT_H
#define DLS_COMMAND_PROMPT_H
#include "dls/dls.h"
#define COMMON_OPTIONS_PROMPT
#define COMMON_OPTIONS_PROMPT_BESTLINE        \
  common_options_b[COMMON_OPTION_HELP](args), \
  common_options_b[COMMON_OPTION_PROMPT_COMMANDS](args),
#define SUBCOMMANDS_PROMPT \
  CREATE_SUBCOMMAND(PROMPT_BESTLINE, ),
#define ADD_PROMPT_COMMANDS()                                       \
  COMMAND(ICON_PROMPT, PROMPT, "prompt", COLOR_PROMPT, "Prompt", 0) \
  COMMAND(ICON_PROMPT, PROMPT_BESTLINE, "bestline", COLOR_PROMPT, "Bestline", *_command_prompt_bestline)
#define ADD_PROMPT_SUBCOMMANDS() \
  CREATE_SUBCOMMAND(PROMPT, SUBCOMMANDS_PROMPT),
#endif
