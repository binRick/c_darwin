#pragma once
//////////////////////////////////////
#include "string-utils/string-utils.h"
#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
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
struct args_t {
  bool verbose, current_space_only;
  int  space_id, window_id;
  int  x, y;
  int  width, height;
};
static const struct args_t *args = &(struct args_t){
  .verbose            = false,
  .current_space_only = false,
  .space_id           = 0, .window_id = 0,
};
