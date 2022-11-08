#pragma once
#ifndef DLS_COMMAND_KEYLOGGER_C
#define DLS_COMMAND_KEYLOGGER_C
#include "dls/command-keylogger.h"

void _command_keylogger_run(void){
  log_info("kl run");
  exit(EXIT_SUCCESS);
}
#endif
