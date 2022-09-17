#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/******************************************************/
int keylogger_exec();
int keylogger_exec_with_callback(void ( *cb )(char *));
/******************************************************/
