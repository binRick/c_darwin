#pragma once
#ifndef WINDOW_SORT_C
#define WINDOW_SORT_C
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "font-utils/font-utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "window/sort/sort.h"
static bool WINDOW_SORT_DEBUG_MODE = false;

///////////////////////////////////////////////////////////////////////
static size_t get_max_app_sort_name_len(){
  size_t s = 0;

  for (size_t i = 0; i < APP_SORT_TYPES_QTY; i++) {
    s = (strlen(app_sort_type_names[i]) > s) ? strlen(app_sort_type_names[i]) : s;
  }
  return(s);
}

static size_t get_max_window_sort_name_len(){
  size_t s = 0;

  for (size_t i = 0; i < WINDOW_SORT_TYPES_QTY; i++) {
    s = (strlen(window_sort_type_names[i]) > s) ? strlen(window_sort_type_names[i]) : s;
  }
  return(s);
}

static size_t get_max_font_sort_name_len(){
  size_t s = 0;

  for (size_t i = 0; i < FONT_SORT_TYPES_QTY; i++) {
    s = (strlen(font_sort_type_names[i]) > s) ? strlen(font_sort_type_names[i]) : s;
  }
  return(s);
}

char *get_font_sort_types_description(){
  struct StringBuffer *sb = stringbuffer_new();
  char                *s, *spaces;
  size_t              max_key_len = get_max_font_sort_name_len();

  asprintf(&s, AC_GREEN AC_UNDERLINE "Font Sort Types" AC_RESETALL ": \n");
  stringbuffer_append_string(sb, s);
  for (size_t i = 0; i < FONT_SORT_TYPES_QTY; i++) {
    spaces = "";
    if (i > 0 && i < (FONT_SORT_TYPES_QTY)) {
      stringbuffer_append_string(sb, "\n ");
    }

    for (size_t ii = 0; ii < max_key_len - strlen(font_sort_type_names[i]); ii++) {
      asprintf(&spaces, "%s%s", spaces, " ");
    }
    asprintf(&s, "\t\t\t\t\t    " AC_YELLOW AC_ITALIC "%s" AC_RESETALL "%s : %s",
             font_sort_type_names[i],
             spaces,
             font_sort_type_descriptions[i]
             );
    stringbuffer_append_string(sb, s);
  }
  s = stringbuffer_to_string(sb);
  stringbuffer_release(sb);
  return(s);
}

char *get_app_sort_types_description(){
  struct StringBuffer *sb = stringbuffer_new();
  char                *s, *spaces;
  size_t              max_key_len = get_max_app_sort_name_len();

  asprintf(&s, AC_GREEN AC_UNDERLINE "App Sort Types" AC_RESETALL ": \n");
  stringbuffer_append_string(sb, s);
  for (size_t i = 0; i < APP_SORT_TYPES_QTY; i++) {
    spaces = "";
    if (i > 0 && i < (APP_SORT_TYPES_QTY)) {
      stringbuffer_append_string(sb, "\n ");
    }

    for (size_t ii = 0; ii < max_key_len - strlen(app_sort_type_names[i]); ii++) {
      asprintf(&spaces, "%s%s", spaces, " ");
    }
    asprintf(&s, "\t\t\t\t\t    " AC_YELLOW AC_ITALIC "%s" AC_RESETALL "%s : %s",
             app_sort_type_names[i],
             spaces,
             app_sort_type_descriptions[i]
             );
    stringbuffer_append_string(sb, s);
  }
  s = stringbuffer_to_string(sb);
  stringbuffer_release(sb);
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

app_sort_function get_app_sort_function_from_key(const char *name, const char *direction){
  for (size_t i = 0; i < FONT_SORT_OPTIONS_QTY; i++) {
    if (strcmp(name, app_sort_type_names[app_sorts[i].type]) == 0 && strcmp(direction, sort_direction_keys[app_sorts[i].direction]) == 0) {
      return((app_sort_function)(app_sorts[i].function));
    }
  }
  return((app_sort_function)(app_sorts[0].function));
}

font_sort_function get_font_sort_function_from_key(const char *name, const char *direction){
  for (size_t i = 0; i < FONT_SORT_OPTIONS_QTY; i++) {
    bool match = (strcmp(name, font_sort_type_names[font_sorts[i].type]) == 0 && strcmp(direction, sort_direction_keys[font_sorts[i].direction]) == 0) ? true : false;
    if (match == true) {
      return((font_sort_function)(font_sorts[i].function));
    }
  }
  return((font_sort_function)(font_sorts[0].function));
}

window_sort_function get_window_sort_function_from_key(const char *name, const char *direction){
  for (size_t i = 0; i < WINDOW_SORT_OPTIONS_QTY; i++) {
    if (strcmp(name, window_sort_type_names[sorts[i].type]) == 0 && strcmp(direction, sort_direction_keys[sorts[i].direction]) == 0) {
      return((window_sort_function)(sorts[i].function));
    }
  }
  return((window_sort_function)(sorts[0].function));
}

int font_sort_type_asc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_TYPE_ASC].block(w0, w1));
}

int font_sort_type_desc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_TYPE_DESC].block(w0, w1));
}

int font_sort_family_asc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_FAMILY_ASC].block(w0, w1));
}

int font_sort_family_desc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_FAMILY_DESC].block(w0, w1));
}

int font_sort_dupe_asc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_DUPE_ASC].block(w0, w1));
}

int font_sort_dupe_desc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_DUPE_DESC].block(w0, w1));
}

int font_sort_style_asc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_STYLE_ASC].block(w0, w1));
}

int font_sort_style_desc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_STYLE_DESC].block(w0, w1));
}

int font_sort_name_asc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_NAME_ASC].block(w0, w1));
}

int font_sort_name_desc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_NAME_DESC].block(w0, w1));
}

int font_sort_faces_asc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_FACES_ASC].block(w0, w1));
}

int font_sort_faces_desc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_FACES_DESC].block(w0, w1));
}

int font_sort_size_asc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_SIZE_ASC].block(w0, w1));
}

int font_sort_size_desc(FONT_SORT_PARAMS){
  return(font_sorts[FONT_SORT_TYPE_SIZE_DESC].block(w0, w1));
}

int window_sort_pid_asc(WINDOW_SORT_PARAMS){
  return(sorts[WINDOW_SORT_OPTION_PID_ASC].block(w0, w1));
}

int window_sort_pid_desc(WINDOW_SORT_PARAMS){
  return(sorts[WINDOW_SORT_OPTION_PID_DESC].block(w0, w1));
}

int window_sort_window_id_asc(WINDOW_SORT_PARAMS){
  return(sorts[WINDOW_SORT_OPTION_WINDOW_ID_ASC].block(w0, w1));
}

int window_sort_window_id_desc(WINDOW_SORT_PARAMS){
  return(sorts[WINDOW_SORT_OPTION_WINDOW_ID_DESC].block(w0, w1));
}
////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__sort(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_sort") != NULL) {
    log_debug("Enabling sort Debug Mode");
    WINDOW_SORT_DEBUG_MODE = true;
  }
}
#endif
