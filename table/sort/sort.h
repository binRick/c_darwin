#pragma once
#ifndef TABLE_SORT_H
#define TABLE_SORT_H
#define MAX_SORT_TYPES 32
#define WINDOW_SORT_STRUCT    window_info_t
#define FONT_SORT_STRUCT      font_t
#define APP_SORT_STRUCT       app_t
#define SORT_PARAMS    const void *i0, const void *i1
//////////////////////////////////////
#include "app/app.h"
#include "app/utils/utils.h"
#include "font-utils/font-utils.h"
#include "window/info/info.h"
#include "window/utils/utils.h"
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
typedef int (*sort_function)(SORT_PARAMS);


#define SORT_FUNCTIONS()\
SORT_FUNCTION_STR(font,FONT_SORT_STRUCT *,family)\
SORT_FUNCTION_STR(font,FONT_SORT_STRUCT *,name)\
SORT_FUNCTION_STR(font,FONT_SORT_STRUCT *,style)\
SORT_FUNCTION_STR(font,FONT_SORT_STRUCT *,type)\
SORT_FUNCTION_STR(app,APP_SORT_STRUCT *,name)\
SORT_FUNCTION_INT(font,FONT_SORT_STRUCT *,size)\
SORT_FUNCTION_INT(font,FONT_SORT_STRUCT *,duplicate)\
SORT_FUNCTION_INT(font,FONT_SORT_STRUCT *,typefaces_qty)\
SORT_FUNCTION_INT(window,WINDOW_SORT_STRUCT *,window_id)\
SORT_FUNCTION_INT(window,WINDOW_SORT_STRUCT *,pid)\
SORT_FUNCTION_INT(window,WINDOW_SORT_STRUCT *,display_id)\
SORT_FUNCTION_INT(window,WINDOW_SORT_STRUCT *,space_id)\

#define SORT_FUNCTION_STR(NAME,TYPE,ITEM)\
int NAME##_sort_##ITEM##_asc(SORT_PARAMS);\
int NAME##_sort_##ITEM##_desc(SORT_PARAMS);
#define SORT_FUNCTION_INT(NAME,TYPE,ITEM)\
int NAME##_sort_##ITEM##_asc(SORT_PARAMS);\
int NAME##_sort_##ITEM##_desc(SORT_PARAMS);
SORT_FUNCTIONS()
#undef SORT_FUNCTION_INT
#undef SORT_FUNCTION_STR

enum sort_direction_t {
  SORT_DIRECTION_ASC,
  SORT_DIRECTION_DESC,
  SORT_DIRECTIONS_QTY,
};
enum sort_type_t {
  SORT_TYPE_PROCESS,
  SORT_TYPE_APP,
  SORT_TYPE_KITTY,
  SORT_TYPE_SPACE,
  SORT_TYPE_DISPLAY,
  SORT_TYPE_MONITOR,
  SORT_TYPE_WINDOW,
  SORT_TYPE_USB,
  SORT_TYPE_HOTKEY,
  SORT_TYPE_FONT,
  SORT_TYPES_QTY,
};
sort_function get_sort_type_function_from_key(enum sort_type_t type, const char *name, const char *direction);

static const char *sort_direction_keys[] = {
  [SORT_DIRECTION_ASC]  = "asc",
  [SORT_DIRECTION_DESC] = "desc",
};
static const char *sort_direction_names[] = {
  [SORT_DIRECTION_ASC]  = "Ascending",
  [SORT_DIRECTION_DESC] = "Descending",
};
enum sort_direction_t get_sort_key_direction(char *name);
enum app_sort_type_t {
  APP_SORT_TYPE_NAME,
  APP_SORT_TYPES_QTY,
};
enum font_sort_type_t {
  FONT_SORT_TYPE_SIZE,
  FONT_SORT_TYPE_FACES,
  FONT_SORT_TYPE_NAME,
  FONT_SORT_TYPE_FAMILY,
  FONT_SORT_TYPE_STYLE,
  FONT_SORT_TYPE_TYPE,
  FONT_SORT_TYPE_DUPE,
  FONT_SORT_TYPES_QTY,
};
enum app_sort_option_t {
  APP_SORT_TYPE_NAME_ASC,
  APP_SORT_TYPE_NAME_DESC,
  APP_SORT_OPTIONS_QTY,
};
enum font_sort_option_t {
  FONT_SORT_TYPE_SIZE_ASC,
  FONT_SORT_TYPE_SIZE_DESC,
  FONT_SORT_TYPE_FACES_ASC,
  FONT_SORT_TYPE_FACES_DESC,
  FONT_SORT_TYPE_NAME_ASC,
  FONT_SORT_TYPE_NAME_DESC,
  FONT_SORT_TYPE_FAMILY_ASC,
  FONT_SORT_TYPE_FAMILY_DESC,
  FONT_SORT_TYPE_STYLE_ASC,
  FONT_SORT_TYPE_STYLE_DESC,
  FONT_SORT_TYPE_TYPE_ASC,
  FONT_SORT_TYPE_TYPE_DESC,
  FONT_SORT_TYPE_DUPE_ASC,
  FONT_SORT_TYPE_DUPE_DESC,
  FONT_SORT_OPTIONS_QTY,
};
enum window_sort_option_t {
  WINDOW_SORT_TYPE_WINDOW_ID_ASC,
  WINDOW_SORT_TYPE_WINDOW_ID_DESC,
  WINDOW_SORT_TYPE_DISPLAY_ID_ASC,
  WINDOW_SORT_TYPE_DISPLAY_ID_DESC,
  WINDOW_SORT_TYPE_SPACE_ID_ASC,
  WINDOW_SORT_TYPE_SPACE_ID_DESC,
  WINDOW_SORT_TYPE_PID_ASC,
  WINDOW_SORT_TYPE_PID_DESC,
  WINDOW_SORT_TYPE_QTY,
};
struct sort_handler_t {
  enum sort_direction_t direction;
  char *name, *description;
  sort_function          function;
};
struct sort_t {
  bool enabled;
  const struct sort_handler_t *handlers[MAX_SORT_TYPES];
};
static struct sort_t __attribute__((unused))*sort_types[] = {
  [SORT_TYPE_APP] = &(struct sort_t){
    .enabled = true,
    .handlers = {
      [APP_SORT_TYPE_NAME_ASC] =    &(struct sort_handler_t){
        .direction = SORT_DIRECTION_ASC,
        .name = "name",
        .description = "Application Name",
        .function               = app_sort_name_asc,
      },
      [APP_SORT_TYPE_NAME_DESC] =    &(struct sort_handler_t){
        .direction = SORT_DIRECTION_DESC,
        .name = "name",
        .description = "Application Name",
        .function               = app_sort_name_desc,
      },
    },
  },
  [SORT_TYPE_WINDOW] = &(struct sort_t){
    .enabled = true,
    .handlers = {
      [WINDOW_SORT_TYPE_SPACE_ID_ASC] =    &(struct sort_handler_t){
        .direction = SORT_DIRECTION_ASC,
        .name = "space",
        .description = "Space",
        .function               = window_sort_space_id_asc,
      },
      [WINDOW_SORT_TYPE_SPACE_ID_DESC] =    &(struct sort_handler_t){
        .direction = SORT_DIRECTION_DESC,
        .name = "space",
        .description = "Space",
        .function               = window_sort_space_id_desc,
      },
      [WINDOW_SORT_TYPE_DISPLAY_ID_ASC] =    &(struct sort_handler_t){
        .direction = SORT_DIRECTION_ASC,
        .name = "display",
        .description = "Display ID",
        .function               = window_sort_display_id_asc,
      },
      [WINDOW_SORT_TYPE_DISPLAY_ID_DESC] =    &(struct sort_handler_t){
        .direction = SORT_DIRECTION_DESC,
        .name = "display",
        .description = "Display ID",
        .function               = window_sort_display_id_desc,
      },
      [WINDOW_SORT_TYPE_PID_ASC] =    &(struct sort_handler_t){
        .direction = SORT_DIRECTION_ASC,
        .name = "pid",
        .description = "PID",
        .function               = window_sort_pid_asc,
      },
      [WINDOW_SORT_TYPE_PID_DESC] =    &(struct sort_handler_t){
        .direction = SORT_DIRECTION_DESC,
        .name = "pid",
        .description = "PID",
        .function               = window_sort_pid_desc,
      },
      [WINDOW_SORT_TYPE_WINDOW_ID_ASC] =    &(struct sort_handler_t){
        .direction = SORT_DIRECTION_ASC,
        .name = "id",
        .description = "Window ID",
        .function               = window_sort_window_id_asc,
      },
      [WINDOW_SORT_TYPE_WINDOW_ID_DESC] =    &(struct sort_handler_t){
        .direction = SORT_DIRECTION_DESC,
        .name = "id",
        .description = "Window ID",
        .function               = window_sort_window_id_desc,
      },
    },
  },
  [SORT_TYPE_FONT] = &(struct sort_t){
    .enabled = true,
    .handlers = {
      [FONT_SORT_TYPE_SIZE_ASC] =    &(struct sort_handler_t){
        .direction = SORT_DIRECTION_ASC,
        .name = "size",
        .description = "Font Size",
        .function               = font_sort_size_asc,
      },
      [FONT_SORT_TYPE_SIZE_DESC] =    &(struct sort_handler_t){
        .name = "size",
        .description = "Font Size",
        .direction = SORT_DIRECTION_DESC,
        .function                = font_sort_size_desc,
      },
      [FONT_SORT_TYPE_FACES_ASC] =    &(struct sort_handler_t){
        .name = "faces",
        .description = "Font Faces",
        .direction = SORT_DIRECTION_ASC,
        .function                = font_sort_typefaces_qty_asc,
      },
      [FONT_SORT_TYPE_FACES_DESC] =    &(struct sort_handler_t){
        .name = "faces",
        .description = "Font Faces",
        .direction = SORT_DIRECTION_DESC,
        .function                 = font_sort_typefaces_qty_desc,
      },
      [FONT_SORT_TYPE_TYPE_ASC] =    &(struct sort_handler_t){
        .name = "type",
        .description = "Font Type",
        .direction = SORT_DIRECTION_ASC,
        .function               = font_sort_type_asc,
      },
      [FONT_SORT_TYPE_TYPE_DESC] =    &(struct sort_handler_t){
        .name = "type",
        .description = "Font Type",
        .direction = SORT_DIRECTION_DESC,
        .function                = font_sort_type_desc,
      },
      [FONT_SORT_TYPE_STYLE_ASC] =    &(struct sort_handler_t){
        .name = "style",
        .description = "Font Style",
        .function                = font_sort_style_asc,
      },
      [FONT_SORT_TYPE_STYLE_DESC] =    &(struct sort_handler_t){
        .name = "style",
        .description = "Font Style",
        .direction = SORT_DIRECTION_DESC,
        .function                 = font_sort_style_desc,
      },
      [FONT_SORT_TYPE_FAMILY_ASC] =    &(struct sort_handler_t){
        .name = "family",
        .description = "Font Family",
        .direction = SORT_DIRECTION_ASC,
        .function                 = font_sort_family_asc,
      },
      [FONT_SORT_TYPE_FAMILY_DESC] =    &(struct sort_handler_t){
        .name = "family",
        .description = "Font Family",
        .direction = SORT_DIRECTION_DESC,
        .function                  = font_sort_family_desc,
      },
      [FONT_SORT_TYPE_NAME_ASC] =    &(struct sort_handler_t){
        .name = "name",
        .description = "Font Name",
        .function               = font_sort_name_asc,
      },
      [FONT_SORT_TYPE_NAME_DESC] =    &(struct sort_handler_t){
        .name = "name",
        .description = "Font Name",
        .direction = SORT_DIRECTION_DESC,
        .function                = font_sort_name_desc,
      },
      [FONT_SORT_TYPE_DUPE_ASC] =    &(struct sort_handler_t){
        .name = "dupe",
        .description = "Font Duplicate",
        .function               = font_sort_duplicate_asc,
      },
      [FONT_SORT_TYPE_DUPE_DESC] =    &(struct sort_handler_t){
        .name = "dupe",
        .description = "Font Duplicate",
        .direction = SORT_DIRECTION_DESC,
        .function                = font_sort_duplicate_desc,
      },
  },
  },
};
#endif
