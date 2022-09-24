#pragma once
#ifndef WINDOW_SORT_H
#define WINDOW_SORT_H
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
#define WINDOW_SORT_STRUCT    window_info_t
#define FONT_SORT_STRUCT      font_t
#define APP_SORT_STRUCT       app_t
#define WINDOW_SORT_PARAMS    const struct WINDOW_SORT_STRUCT *w0, const struct WINDOW_SORT_STRUCT *w1
#define APP_SORT_PARAMS       const struct APP_SORT_STRUCT *w0, const struct APP_SORT_STRUCT *w1
#define FONT_SORT_PARAMS      const struct FONT_SORT_STRUCT *w0, const struct FONT_SORT_STRUCT *w1
typedef int (*font_sort_function)(FONT_SORT_PARAMS);
typedef int (^font_sort_block)(FONT_SORT_PARAMS);
typedef int (*app_sort_function)(APP_SORT_PARAMS);
typedef int (^app_sort_block)(APP_SORT_PARAMS);
typedef int (*window_sort_function)(WINDOW_SORT_PARAMS);
typedef int (^window_sort_block)(WINDOW_SORT_PARAMS);
enum sort_direction_t {
  SORT_DIRECTION_ASC,
  SORT_DIRECTION_DESC,
  SORT_DIRECTIONS_QTY,
};
static const char *sort_direction_keys[] = {
  [SORT_DIRECTION_ASC]  = "asc",
  [SORT_DIRECTION_DESC] = "desc",
};
static const char *sort_direction_names[] = {
  [SORT_DIRECTION_ASC]  = "Ascending",
  [SORT_DIRECTION_DESC] = "Descending",
};
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
enum window_sort_type_t {
  WINDOW_SORT_TYPE_WINDOW_ID,
  WINDOW_SORT_TYPE_PID,
  WINDOW_SORT_TYPES_QTY,
};
static const char *font_sort_type_names[] = {
  [FONT_SORT_TYPE_SIZE]   = "size",
  [FONT_SORT_TYPE_FACES]  = "faces",
  [FONT_SORT_TYPE_NAME]   = "name",
  [FONT_SORT_TYPE_FAMILY] = "family",
  [FONT_SORT_TYPE_STYLE]  = "style",
  [FONT_SORT_TYPE_TYPE]   = "type",
  [FONT_SORT_TYPE_DUPE]   = "duplicate",
};
static const char *font_sort_type_descriptions[] = {
  [FONT_SORT_TYPE_SIZE]   = "Font Size",
  [FONT_SORT_TYPE_FACES]  = "Font Faces",
  [FONT_SORT_TYPE_NAME]   = "Font Name",
  [FONT_SORT_TYPE_FAMILY] = "Font Family",
  [FONT_SORT_TYPE_STYLE]  = "Font Style",
  [FONT_SORT_TYPE_TYPE]   = "Font Type",
  [FONT_SORT_TYPE_DUPE]   = "Duplicate Font",
};
static const char *app_sort_type_names[] = {
  [APP_SORT_TYPE_NAME] = "name",
};
static const char *app_sort_type_descriptions[] = {
  [APP_SORT_TYPE_NAME] = "Name",
};
static const char *window_sort_type_names[] = {
  [WINDOW_SORT_TYPE_PID]       = "pid",
  [WINDOW_SORT_TYPE_WINDOW_ID] = "window-id",
};
static const char *window_sort_type_descriptions[] = {
  [WINDOW_SORT_TYPE_PID]       = "PID",
  [WINDOW_SORT_TYPE_WINDOW_ID] = "Window ID",
};
enum app_sort_option_t {
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
  WINDOW_SORT_OPTION_WINDOW_ID_ASC,
  WINDOW_SORT_OPTION_WINDOW_ID_DESC,
  WINDOW_SORT_OPTION_PID_ASC,
  WINDOW_SORT_OPTION_PID_DESC,
  WINDOW_SORT_OPTIONS_QTY,
};
struct font_sort_t {
  const enum font_sort_type_t type;
  const enum sort_direction_t direction;
  font_sort_block             block;
  font_sort_function          function;
};
struct app_sort_t {
  const enum app_sort_type_t  type;
  const enum sort_direction_t direction;
  app_sort_block              block;
  app_sort_function           function;
};
struct window_sort_t {
  const enum window_sort_type_t type;
  const enum sort_direction_t   direction;
  window_sort_block             block;
  window_sort_function          function;
};
int window_sort_pid_desc(WINDOW_SORT_PARAMS);
int window_sort_pid_asc(WINDOW_SORT_PARAMS);
int window_sort_window_id_desc(WINDOW_SORT_PARAMS);
int window_sort_window_id_asc(WINDOW_SORT_PARAMS);
char *get_window_sort_types_description();
char *get_font_sort_types_description();
char *get_app_sort_types_description();
int font_sort_dupe_asc(FONT_SORT_PARAMS);
int font_sort_dupe_desc(FONT_SORT_PARAMS);
int font_sort_type_asc(FONT_SORT_PARAMS);
int font_sort_type_desc(FONT_SORT_PARAMS);
int font_sort_style_asc(FONT_SORT_PARAMS);
int font_sort_style_desc(FONT_SORT_PARAMS);
int font_sort_name_asc(FONT_SORT_PARAMS);
int font_sort_name_desc(FONT_SORT_PARAMS);
int font_sort_family_asc(FONT_SORT_PARAMS);
int font_sort_family_desc(FONT_SORT_PARAMS);
int font_sort_faces_asc(FONT_SORT_PARAMS);
int font_sort_faces_desc(FONT_SORT_PARAMS);
int font_sort_size_asc(FONT_SORT_PARAMS);
int font_sort_size_desc(FONT_SORT_PARAMS);
window_sort_function get_window_sort_function_from_key(const char *name, const char *direction);
font_sort_function get_font_sort_function_from_key(const char *name, const char *direction);
app_sort_function get_app_sort_function_from_key(const char *name, const char *direction);
static const struct font_sort_t   font_sorts[] = {
  [FONT_SORT_TYPE_SIZE_ASC] =    {
    .type                   = FONT_SORT_TYPE_SIZE,
    .direction              = SORT_DIRECTION_ASC,
    .function               = font_sort_size_asc,
    .block                  = ^ int (FONT_SORT_PARAMS){
      int r                 = ((w0->size > w1->size) ? 1 : (w0->size < w1->size) ? -1 : 0);
      return(r);
    },
  },
  [FONT_SORT_TYPE_SIZE_DESC] =   {
    .type                    = FONT_SORT_TYPE_SIZE,
    .direction               = SORT_DIRECTION_DESC,
    .function                = font_sort_size_desc,
    .block                   = ^ int (FONT_SORT_PARAMS){
      int r                  = ((w0->size < w1->size) ? 1 : (w0->size > w1->size) ? -1 : 0);
      return(r);
    },
  },
  [FONT_SORT_TYPE_FACES_ASC] =   {
    .type                    = FONT_SORT_TYPE_FACES,
    .direction               = SORT_DIRECTION_ASC,
    .function                = font_sort_faces_asc,
    .block                   = ^ int (FONT_SORT_PARAMS){
      int r                  = ((w0->typefaces_qty > w1->typefaces_qty) ? 1 : (w0->typefaces_qty < w1->typefaces_qty) ? -1 : 0);
      return(r);
    },
  },
  [FONT_SORT_TYPE_FACES_DESC] =  {
    .type                     = FONT_SORT_TYPE_FACES,
    .direction                = SORT_DIRECTION_DESC,
    .function                 = font_sort_faces_desc,
    .block                    = ^ int (FONT_SORT_PARAMS){
      int r                   = ((w0->typefaces_qty < w1->typefaces_qty) ? 1 : (w0->typefaces_qty > w1->typefaces_qty) ? -1 : 0);
      return(r);
    },
  },
  [FONT_SORT_TYPE_TYPE_ASC] =    {
    .type                   = FONT_SORT_TYPE_TYPE,
    .direction              = SORT_DIRECTION_ASC,
    .function               = font_sort_type_asc,
    .block                  = ^ int (FONT_SORT_PARAMS){
      int r                 = (strcmp(w0->type, w1->type) * 1);
      return(r);
    },
  },
  [FONT_SORT_TYPE_TYPE_DESC] =   {
    .type                    = FONT_SORT_TYPE_TYPE,
    .direction               = SORT_DIRECTION_DESC,
    .function                = font_sort_type_desc,
    .block                   = ^ int (FONT_SORT_PARAMS){
      int r                  = (strcmp(w0->type, w1->type) * -1);
      return(r);
    },
  },
  [FONT_SORT_TYPE_STYLE_ASC] =   {
    .type                    = FONT_SORT_TYPE_STYLE,
    .direction               = SORT_DIRECTION_ASC,
    .function                = font_sort_style_asc,
    .block                   = ^ int (FONT_SORT_PARAMS){
      int r                  = (strcmp(w0->style, w1->style) * 1);
      return(r);
    },
  },
  [FONT_SORT_TYPE_STYLE_DESC] =  {
    .type                     = FONT_SORT_TYPE_STYLE,
    .direction                = SORT_DIRECTION_DESC,
    .function                 = font_sort_style_desc,
    .block                    = ^ int (FONT_SORT_PARAMS){
      int r                   = (strcmp(w0->style, w1->style) * -1);
      return(r);
    },
  },
  [FONT_SORT_TYPE_FAMILY_ASC] =  {
    .type                     = FONT_SORT_TYPE_FAMILY,
    .direction                = SORT_DIRECTION_ASC,
    .function                 = font_sort_name_asc,
    .block                    = ^ int (FONT_SORT_PARAMS){
      int r                   = (strcmp(w0->family, w1->family) * 1);
      return(r);
    },
  },
  [FONT_SORT_TYPE_FAMILY_DESC] = {
    .type                      = FONT_SORT_TYPE_FAMILY,
    .direction                 = SORT_DIRECTION_DESC,
    .function                  = font_sort_family_desc,
    .block                     = ^ int (FONT_SORT_PARAMS){
      int r                    = (strcmp(w0->family, w1->family) * -1);
      return(r);
    },
  },
  [FONT_SORT_TYPE_NAME_ASC] =    {
    .type                   = FONT_SORT_TYPE_NAME,
    .direction              = SORT_DIRECTION_ASC,
    .function               = font_sort_name_asc,
    .block                  = ^ int (FONT_SORT_PARAMS){
      int r                 = (strcmp(w0->name, w1->name) * 1);
      return(r);
    },
  },
  [FONT_SORT_TYPE_NAME_DESC] =   {
    .type                    = FONT_SORT_TYPE_NAME,
    .direction               = SORT_DIRECTION_DESC,
    .function                = font_sort_name_desc,
    .block                   = ^ int (FONT_SORT_PARAMS){
      int r                  = (strcmp(w0->name, w1->name) * -1);
      return(r);
    },
  },
  [FONT_SORT_TYPE_DUPE_ASC] =    {
    .type                   = FONT_SORT_TYPE_DUPE,
    .direction              = SORT_DIRECTION_ASC,
    .function               = font_sort_dupe_asc,
    .block                  = ^ int (FONT_SORT_PARAMS){
      int r                 = ((w0->duplicate == w1->duplicate) * 1);
      return(r);
    },
  },
  [FONT_SORT_TYPE_DUPE_DESC] =   {
    .type                    = FONT_SORT_TYPE_DUPE,
    .direction               = SORT_DIRECTION_DESC,
    .function                = font_sort_dupe_desc,
    .block                   = ^ int (FONT_SORT_PARAMS){
      int r                  = ((w0->duplicate == w1->duplicate) * -1);
      return(r);
    },
  },
};
static const struct window_sort_t app_sorts[] = {
};
static const struct window_sort_t sorts[] = {
  [WINDOW_SORT_OPTION_PID_ASC] =        {
    .type                      = WINDOW_SORT_TYPE_PID,
    .direction                 = SORT_DIRECTION_ASC,
    .function                  = window_sort_pid_asc,
    .block                     = ^ int (WINDOW_SORT_PARAMS){
      int r                    = ((w0->pid > w1->pid) ? 1 : (w0->pid < w1->pid) ? -1 : 0);
      return(r);
    },
  },
  [WINDOW_SORT_OPTION_PID_DESC] =       {
    .type                       = WINDOW_SORT_TYPE_PID,
    .direction                  = SORT_DIRECTION_DESC,
    .function                   = window_sort_pid_desc,
    .block                      = ^ int (WINDOW_SORT_PARAMS){
      int r                     = ((w0->pid < w1->pid) ? 1 : (w0->pid > w1->pid) ? -1 : 0);
      return(r);
    }, },
  [WINDOW_SORT_OPTION_WINDOW_ID_ASC] =  {
    .type                            = WINDOW_SORT_TYPE_WINDOW_ID,
    .direction                       = SORT_DIRECTION_ASC,
    .function                        = window_sort_window_id_asc,
    .block                           = ^ int (WINDOW_SORT_PARAMS){
      return((w0->window_id > w1->window_id) ? 1 : (w0->window_id < w1->window_id) ? -1 : 0);
    }, },
  [WINDOW_SORT_OPTION_WINDOW_ID_DESC] = {
    .type                             = WINDOW_SORT_TYPE_WINDOW_ID,
    .direction                        = SORT_DIRECTION_DESC,
    .function                         = window_sort_window_id_desc,
    .block                            = ^ int (WINDOW_SORT_PARAMS){
      return((w0->window_id < w1->window_id) ? 1 : (w0->window_id > w1->window_id) ? -1 : 0);
    },
  },
};
#endif
