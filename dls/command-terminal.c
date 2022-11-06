#pragma once
#ifndef DLS_COMMAND_TERMINAL_C
#define DLS_COMMAND_TERMINAL_C
#include "dls/command-terminal.h"
int _command_terminal_kitty(void){
  log_info("term kitty");
  exit(EXIT_SUCCESS);
}
#endif
