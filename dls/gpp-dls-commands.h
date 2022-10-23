#pragma once
#ifndef GPP_DLS_COMMANDS_H
#define GPP_DLS_COMMANDS_H
#include "gpp-dls-options.h"

enum dls_command_id_t {
  DLS_COMMAND_ID_LAYOUT_LS,
  DLS_COMMAND_ID_LAYOUT_NAMES,
  DLS_COMMAND_ID_LAYOUT_APPLY,
  DLS_COMMAND_ID_LAYOUT_SHOW,
  DLS_COMMAND_ID_LAYOUT_RENDER,
  DLS_COMMAND_ID_LAYOUT_TEST,
  DLS_COMMAND_ID_LAYOUT,
  DLS_COMMAND_ID_CAPTURE_WINDOW,
  DLS_COMMAND_ID_CAPTURE_SPACE,
  DLS_COMMAND_ID_CAPTURE_DISPLAY,
  DLS_COMMAND_ID_CAPTURE,
  DLS_COMMAND_IDS_QTY,
};

struct dls_command_t {
  enum dls_command_id_t commands[1024];
  enum dls_option_id_t options[1024];
  size_t options_qty, commands_qty;
  char *name;
  char *desc;
  struct optparse_cmd *(^cmds)(void);
  struct optparse_opt *(^opts)(void);
};

void get_dls_command(enum dls_command_id_t id);
void get_cmd(void);

#endif
