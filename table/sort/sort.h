#pragma once
#ifndef TABLE_SORT_H
#define TABLE_SORT_H
#define MAX_SORT_TYPES        32
#define WINDOW_SORT_STRUCT    window_info_t
#define CAPTURED_WINDOW_SORT_STRUCT    window_info_t
#define FONT_SORT_STRUCT      font_t
#define APP_SORT_STRUCT       app_t
#define SORT_PARAMS           const void __attribute__((unused)) * i0, const void __attribute__((unused)) * i1
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

#define SORT_FUNCTIONS()                                      \
  SORT_FUNCTION_STR(font, FONT_SORT_STRUCT *, family)         \
  SORT_FUNCTION_STR(font, FONT_SORT_STRUCT *, name)           \
  SORT_FUNCTION_STR(font, FONT_SORT_STRUCT *, style)          \
  SORT_FUNCTION_STR(font, FONT_SORT_STRUCT *, type)           \
  SORT_FUNCTION_STR(app, APP_SORT_STRUCT *, name)             \
  SORT_FUNCTION_INT(font, FONT_SORT_STRUCT *, size)           \
  SORT_FUNCTION_INT(font, FONT_SORT_STRUCT *, duplicate)      \
  SORT_FUNCTION_INT(font, FONT_SORT_STRUCT *, typefaces_qty)  \
  SORT_FUNCTION_STR(window, WINDOW_SORT_STRUCT *, name)       \
  SORT_FUNCTION_INT(window, WINDOW_SORT_STRUCT *, window_id)  \
  SORT_FUNCTION_INT(window, WINDOW_SORT_STRUCT *, pid)        \
  SORT_FUNCTION_INT(window, WINDOW_SORT_STRUCT *, display_id) \
  SORT_FUNCTION_INT(window, WINDOW_SORT_STRUCT *, space_id)   \
  SORT_FUNCTION_INT(captured_window, CAPTURED_WINDOW_SORT_STRUCT *, window_id)   \

#define SORT_FUNCTION_STR(NAME, TYPE, ITEM)        \
  int NAME ## _sort_ ## ITEM ## _asc(SORT_PARAMS); \
  int NAME ## _sort_ ## ITEM ## _desc(SORT_PARAMS);
#define SORT_FUNCTION_INT(NAME, TYPE, ITEM)        \
  int NAME ## _sort_ ## ITEM ## _asc(SORT_PARAMS); \
  int NAME ## _sort_ ## ITEM ## _desc(SORT_PARAMS);
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
  SORT_TYPE_CAPTURED_WINDOW,
  SORT_TYPE_WINDOW,
  SORT_TYPE_USB,
  SORT_TYPE_HOTKEY,
  SORT_TYPE_FONT,
  SORT_TYPES_QTY,
};
enum sort_by_type_t {
  SORT_BY_TYPE_NAME,
  SORT_BY_TYPE_ID,
  SORT_BY_TYPE_DISPLAY,
  SORT_BY_TYPE_PID,
  SORT_BY_TYPE_APP,
  SORT_BY_TYPE_TYPEFACES,
  SORT_BY_TYPE_SPACE,
  SORT_BY_TYPE_FONT,
  SORT_BY_TYPE_FAMILY,
  SORT_BY_TYPE_SIZE,
  SORT_BY_TYPE_DUPLICATE,
  SORT_BY_TYPE_HOTKEY,
  SORT_BY_TYPE_WINDOW,
  SORT_BY_TYPE_PROCESS,
  SORT_BY_TYPE_TYPE,
  SORT_BY_TYPE_MONITOR,
  SORT_BY_TYPE_STYLE,
  SORT_BY_TYPES_QTY,
};
const char *sort_by_type_colors[] = {
  AC_YELLOW, AC_BLUE, AC_MAGENTA, AC_RED, AC_CYAN, AC_GREEN, AC_WHITE
};

const char *sort_by_type_names[SORT_BY_TYPES_QTY] = {
  [SORT_BY_TYPE_NAME]      = "name",
  [SORT_BY_TYPE_HOTKEY]    = "hotkey",
  [SORT_BY_TYPE_ID]        = "id",
  [SORT_BY_TYPE_APP]       = "app",
  [SORT_BY_TYPE_DISPLAY]   = "display",
  [SORT_BY_TYPE_WINDOW]    = "window",
  [SORT_BY_TYPE_MONITOR]   = "monitor",
  [SORT_BY_TYPE_FONT]      = "font",
  [SORT_BY_TYPE_PID]       = "pid",
  [SORT_BY_TYPE_TYPEFACES] = "faces",
  [SORT_BY_TYPE_SPACE]     = "space",
  [SORT_BY_TYPE_FAMILY]    = "family",
  [SORT_BY_TYPE_SIZE]      = "size",
  [SORT_BY_TYPE_DUPLICATE] = "dupe",
  [SORT_BY_TYPE_TYPE]      = "type",
  [SORT_BY_TYPE_STYLE]     = "style",
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
struct sort_handler_t {
  sort_function functions[SORT_DIRECTIONS_QTY];
};
struct sort_t {
  bool                        enabled;
  const struct sort_handler_t *handlers[SORT_BY_TYPES_QTY];
};
static struct sort_t __attribute__((unused)) * sort_types[SORT_TYPES_QTY] = {
  [SORT_TYPE_APP]                                                         = &(struct sort_t){
    .enabled              = true,
    .handlers             =                                                                 {
      [SORT_BY_TYPE_NAME] = &(struct sort_handler_t)                                        {
        .functions =                                                                        { app_sort_name_asc, app_sort_name_desc                   },
      },
    },
  },
  [SORT_TYPE_CAPTURED_WINDOW] = &(struct sort_t)                                                     {
    .enabled               = true,
    .handlers              =                                                                {
      [SORT_BY_TYPE_SPACE] = &(struct sort_handler_t)                                       {
        .functions              =                                                           {
          [SORT_DIRECTION_ASC]  = window_sort_space_id_asc,
          [SORT_DIRECTION_DESC] = window_sort_space_id_desc,
        },
      },
      [SORT_BY_TYPE_NAME] = &(struct sort_handler_t)                                        {
        .functions =                                                                        { window_sort_name_asc,       window_sort_name_desc              },
      },
      [SORT_BY_TYPE_DISPLAY] = &(struct sort_handler_t)                                     {
        .functions =                                                                        { window_sort_display_id_asc, window_sort_space_id_desc          },
      },
      [SORT_BY_TYPE_PID] = &(struct sort_handler_t)                                         {
        .functions =                                                                        { window_sort_pid_asc,        window_sort_pid_desc               },
      },
      [SORT_BY_TYPE_ID] = &(struct sort_handler_t)                                          {
        .functions =                                                                        { window_sort_window_id_asc,  window_sort_window_id_desc         },
      },
    },
  },
  [SORT_TYPE_WINDOW] = &(struct sort_t)                                                     {
    .enabled               = true,
    .handlers              =                                                                {
      [SORT_BY_TYPE_SPACE] = &(struct sort_handler_t)                                       {
        .functions              =                                                           {
          [SORT_DIRECTION_ASC]  = window_sort_space_id_asc,
          [SORT_DIRECTION_DESC] = window_sort_space_id_desc,
        },
      },
      [SORT_BY_TYPE_NAME] = &(struct sort_handler_t)                                        {
        .functions =                                                                        { window_sort_name_asc,       window_sort_name_desc              },
      },
      [SORT_BY_TYPE_DISPLAY] = &(struct sort_handler_t)                                     {
        .functions =                                                                        { window_sort_display_id_asc, window_sort_space_id_desc          },
      },
      [SORT_BY_TYPE_PID] = &(struct sort_handler_t)                                         {
        .functions =                                                                        { window_sort_pid_asc,        window_sort_pid_desc               },
      },
      [SORT_BY_TYPE_ID] = &(struct sort_handler_t)                                          {
        .functions =                                                                        { window_sort_window_id_asc,  window_sort_window_id_desc         },
      },
    },
  },
  [SORT_TYPE_FONT] = &(struct sort_t)                                                       {
    .enabled              = true,
    .handlers             =                                                                 {
      [SORT_BY_TYPE_SIZE] = &(struct sort_handler_t)                                        {
        .functions =                                                                        { font_sort_size_asc,          font_sort_size_desc                 },
      },
      [SORT_BY_TYPE_TYPEFACES] = &(struct sort_handler_t)                                   {
        .functions =                                                                        { font_sort_typefaces_qty_asc, font_sort_family_desc               },
      },
      [SORT_BY_TYPE_TYPE] = &(struct sort_handler_t)                                        {
        .functions =                                                                        { font_sort_type_asc,          font_sort_type_desc                 },
      },
      [SORT_BY_TYPE_STYLE] = &(struct sort_handler_t)                                       {
        .functions =                                                                        { font_sort_style_asc,         font_sort_style_desc                },
      },
      [SORT_BY_TYPE_FAMILY] = &(struct sort_handler_t)                                      {
        .functions =                                                                        { font_sort_family_asc,        font_sort_family_desc               },
      },
      [SORT_BY_TYPE_NAME] = &(struct sort_handler_t)                                        {
        .functions =                                                                        { font_sort_name_asc,          font_sort_name_desc                 },
      },
      [SORT_BY_TYPE_DUPLICATE] = &(struct sort_handler_t)                                   {
        .functions =                                                                        { font_sort_duplicate_asc,     font_sort_duplicate_desc            },
      },
    },
  },
};
char *get_sort_type_by_description(enum sort_type_t type);
#endif
