#pragma once
#ifndef DLS_ALIASES_C
#define DLS_ALIASES_C
#define MAX_ALIASES    10
#include "core/utils/utils.h"
#include "dls/dls-aliases.h"
#include "wildcardcmp/wildcardcmp.h"
////////////////////////////////////////////

static char *alias_names[] = {
  [ALIAS_TYPE_WINDOW]  = "window",
  [ALIAS_TYPE_SPACE]   = "space",
  [ALIAS_TYPE_DISPLAY] = "display",
  [ALIAS_TYPE_PROCESS] = "process",
};

static char *alias_name_globs[ALIAS_TYPES_QTY][MAX_ALIASES] = {
  [ALIAS_TYPE_WINDOW]  = { "win",  "w", NULL, },
  [ALIAS_TYPE_SPACE]   = { "sp",   "s", NULL, },
  [ALIAS_TYPE_DISPLAY] = { "disp", "d", NULL, },
  [ALIAS_TYPE_PROCESS] = { "proc", "ps", NULL, },
};
static const struct dls_ordered_arg_t {
  char *before, *after;
} dls_ordered_args[] = {
  { .before = "-D", .after = "-A", },
};
static const size_t dls_ordered_args_qty = (sizeof(dls_ordered_args)/sizeof(dls_ordered_args[0]));

bool dls_get_arg_is_ordered(char *arg1,char *arg2){
  if(!arg1||!arg2)return false;
  for(size_t i=0;i<dls_ordered_args_qty;i++){
    if(
        strcmp(arg2,dls_ordered_args[i].after)==0
        &&
        strcmp(arg1,dls_ordered_args[i].before)==0
    ){
      return true;
    }
  }
  return false;
};
char *dls_get_alias_type_name(enum alias_type_t type){
  return(alias_names[type]);
}

char **dls_get_alias_type_globs(enum alias_type_t type){
  return(alias_name_globs[type]);
}

char *dls_get_alias_wildcard_glob_name(char *text){
  for (int n = 0; n < ALIAS_TYPES_QTY; n++) {
    for (int i = 0; i < MAX_ALIASES && alias_name_globs[n][i] != NULL; i++) {
      if (strcmp(alias_name_globs[n][i], text) == 0) {
        return(dls_get_alias_type_name(n));
      }
    }
  }
}

#endif
