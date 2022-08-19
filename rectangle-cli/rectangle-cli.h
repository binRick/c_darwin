#pragma once
//////////////////////////////////////
#include <assert.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
//////////////////////////////////////
#include "active-app/active-app.h"
#include "ansi-codes/ansi-codes.h"
#include "c_vector/include/vector.h"
#include "cargs/include/cargs.h"
#include "rectangle/rectangle.h"
//////////////////////////////////////
struct ctx_t {
  bool verbose, todo_switch;
  char *app, *mode;
  int  width;
};
//////////////////////////////////////
static int parse_args(int argc, char *argv[]);
int configure_todo();
int info_todo();
//////////////////////////////////////
