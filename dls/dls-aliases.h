#pragma once
#ifndef DLS_ALIASES_H
#define DLS_ALIASES_H
enum alias_type_t {
  ALIAS_TYPE_WINDOW,
  ALIAS_TYPE_SPACE,
  ALIAS_TYPE_DISPLAY,
  ALIAS_TYPE_PROCESS,
  ALIAS_TYPES_QTY,
};
#include "dls/dls.h"
////////////////////////////////////////////
enum alias_name_type_t;
char *dls_get_alias_type_name(enum alias_type_t type);
char *dls_get_alias_name(char *);
char *dls_get_alias_wildcard_glob_name(char *text);
bool dls_get_arg_is_ordered(char *arg1, char *arg2);
#endif
