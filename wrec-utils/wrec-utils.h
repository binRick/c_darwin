#pragma once
#include <stdio.h>
#define DEBUG_PID_ENV    false
#include <ctype.h>
#include <libproc.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/sysctl.h>
#include <sys/time.h>
////////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_vector/include/vector.h"
#include "djbhash/src/djbhash.h"
#include "fsio.h"
#include "parson.h"
#include "str-replace.h"
#include "string-utils/string-utils.h"
#include "stringbuffer.h"
#include "stringfn.h"
#include "submodules/libfort/src/fort.h"
#include "tiny-regex-c/re.h"
#include "window-utils/window-utils.h"
//////////////////////////////////////////
enum resize_type_t {
  RESIZE_BY_WIDTH,
  RESIZE_BY_HEIGHT,
  RESIZE_BY_FACTOR,
  RESIZE_BY_NONE,
  RESIZE_TYPES_QTY
} resize_type_t;
struct img_data {
  size_t  width;
  size_t  height;
  uint8_t *rgba;
};
//////////////////////////////////////////
int list_windows();
int capture_window();
//////////////////////////////////////////
