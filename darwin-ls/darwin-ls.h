#pragma once
#ifndef LS_WIN_H
#define LS_WIN_H
#include "ansi-codes/ansi-codes.h"
//////////////////////////////////////
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "core-utils/core-utils.h"
#include "darwin-ls/darwin-ls-commands.h"
#include "display-utils/display-utils.h"
#include "dock-utils/dock-utils.h"
#include "focused/focused.h"
#include "font-utils/font-utils.h"
#include "log/log.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "ms/ms.h"
#include "optparse99/optparse99.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
#include "table-utils/table-utils.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
struct args_t {
  bool verbose, current_space_only;
  int  space_id, window_id;
  int  x, y;
  int  width, height;
  int  output_mode; char *output_mode_s;
};
extern struct args_t *args;
#endif