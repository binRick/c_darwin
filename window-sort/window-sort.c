#pragma once
#ifndef WINDOW_SORT_C
#define WINDOW_SORT_C
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "window-sort/window-sort.h"
static bool WINDOW_SORT_DEBUG_MODE = false;

///////////////////////////////////////////////////////////////////////
static size_t get_max_window_sort_name_len(){
  size_t s = 0;

  for (size_t i = 0; i < WINDOW_SORT_TYPES_QTY; i++) {
    s = (strlen(window_sort_type_names[i]) > s) ? strlen(window_sort_type_names[i]) : s;
  }
  return(s);
}

char *get_window_sort_types_description(){
  struct StringBuffer *sb = stringbuffer_new();
  char                *s, *spaces;
  size_t              max_key_len = get_max_window_sort_name_len();

  asprintf(&s, AC_GREEN AC_UNDERLINE "Window Sort Types" AC_RESETALL ": \n");
  stringbuffer_append_string(sb, s);
  for (size_t i = 0; i < WINDOW_SORT_TYPES_QTY; i++) {
    spaces = "";
    if (i > 0 && i < (WINDOW_SORT_TYPES_QTY)) {
      stringbuffer_append_string(sb, "\n ");
    }

    for (size_t ii = 0; ii < max_key_len - strlen(window_sort_type_names[i]); ii++) {
      asprintf(&spaces, "%s%s", spaces, " ");
    }
    asprintf(&s, "\t\t\t\t\t    " AC_YELLOW AC_ITALIC "%s" AC_RESETALL "%s : %s",
             window_sort_type_names[i],
             spaces,
             window_sort_type_descriptions[i]
             );
    stringbuffer_append_string(sb, s);
  }
  s = stringbuffer_to_string(sb);
  stringbuffer_release(sb);
  return(s);
}

window_sort_function get_window_sort_function_from_key(const char *name, const char *direction){
  for (size_t i = 0; i < WINDOW_SORT_OPTIONS_QTY; i++) {
    if (WINDOW_SORT_DEBUG_MODE) {
      log_debug("sort type: #%lu|name:%s|desc:%s|# direction:#%d|key:%s|name:%s|",
                i,
                window_sort_type_names[window_sorts[i].type],
                window_sort_type_descriptions[window_sorts[i].type],
                window_sorts[i].direction,
                window_sort_direction_keys[window_sorts[i].direction],
                window_sort_direction_names[window_sorts[i].direction]
                );
    }
    if (strcmp(name, window_sort_type_names[window_sorts[i].type]) == 0 && strcmp(direction, window_sort_direction_keys[window_sorts[i].direction]) == 0) {
      return((window_sort_function)(window_sorts[i].function));
    }
  }
  return((window_sort_function)(window_sorts[0].function));
}

int window_sort_pid_asc(WINDOW_SORT_PARAMS){
  return(window_sorts[WINDOW_SORT_OPTION_PID_ASC].block(w0, w1));
}

int window_sort_pid_desc(WINDOW_SORT_PARAMS){
  return(window_sorts[WINDOW_SORT_OPTION_PID_DESC].block(w0, w1));
}

int window_sort_window_id_asc(WINDOW_SORT_PARAMS){
  return(window_sorts[WINDOW_SORT_OPTION_WINDOW_ID_ASC].block(w0, w1));
}

int window_sort_window_id_desc(WINDOW_SORT_PARAMS){
  return(window_sorts[WINDOW_SORT_OPTION_WINDOW_ID_DESC].block(w0, w1));
}
////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__window_sort(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_window_sort") != NULL) {
    log_debug("Enabling window-sort Debug Mode");
    WINDOW_SORT_DEBUG_MODE = true;
  }
}
#endif
