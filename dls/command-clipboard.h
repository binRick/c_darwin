#pragma once
#ifndef DLS_COMMAND_CLIPBOARD_H
#define DLS_COMMAND_CLIPBOARD_H
#include "dls/dls.h"
#define COMMON_OPTIONS_CLIPBOARD
//////////////////////////////////////////
#define SUBCOMMANDS_CLIPBOARD \
  CREATE_SUBCOMMAND(COPY, ),  \
  CREATE_SUBCOMMAND(PASTE, ), \
//////////////////////////////////////////
#define ADD_CLIPBOARD_COMMANDS()                                                       \
  COMMAND(ICON_PASTE, PASTE, "paste", COLOR_PASTE, "Paste Clipboard", *_command_paste) \
  COMMAND(ICON_COPY, COPY, "copy", COLOR_COPY, "Copy Clipboard", *_command_copy)       \
//////////////////////////////////////////
#define ADD_CLIPBOARD_SUBCOMMANDS()                    \
  CREATE_SUBCOMMAND(CLIPBOARD, SUBCOMMANDS_CLIPBOARD), \
//////////////////////////////////////////
#endif
