#pragma once
#ifndef APP_H
#define APP_H
//////////////////////////////////////
#include "timelib/timelib.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
struct app_t {
  char          *name;
  char          *version;
  char          *arch;
  char          *path;
  char          *last_modified_s;
  char          *obtained_from;
  char          *info;
  char          **signed_by;

  timelib_time  *last_modified_time;
  unsigned long last_modified_timestamp;
  bool          path_exists;
};
#endif
