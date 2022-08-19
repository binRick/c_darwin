#pragma once
//////////////////////////////////////
#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <inttypes.h>
#include <inttypes.h>
#include <libgen.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <poll.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
//////////////////////////////////////
static char *RECTANGLE_APP_PATH;
static char *RECTANGLE_CONFIG_KEY;
//////////////////////////////////////
bool rectangle_run();
bool rectangle_kill();
int rectangle_get_pid();
bool rectangle_get_todo_mode_enabled();
bool rectangle_set_todo_mode_enabled();
char *rectangle_run_cmd(char *CMD);
bool rectangle_set_todo_mode_disabled();
char *rectangle_get_config();
char *rectangle_get_todo_app();
int rectangle_get_todo_width();
bool rectangle_set_todo_width(const int NEW_TODO_WIDTH);
bool rectangle_set_todo_app(const char *NEW_TODO_APP);
//////////////////////////////////////
