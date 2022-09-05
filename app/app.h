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
enum app_type_t {
  APP_TYPE_APPLE,
  APP_TYPE_ABSENT,
  APP_TYPE_EXISTANT,
  APP_TYPE_SYSTEM,
  APP_TYPE_SYSTEM_LIBRARY,
  APP_TYPE_USER_LIBRARY,
  APP_TYPE_APPLICATIONS,
  APP_TYPE_IDENTIFIED,
  APP_TYPE_UNIDENTIFIED,
  APP_TYPE_VERSIONED,
  APP_TYPE_UNVERSIONED,
  APP_TYPES_QTY,
};
const char *app_type_names[APP_TYPES_QTY + 1];
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
