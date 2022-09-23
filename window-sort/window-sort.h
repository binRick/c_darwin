#pragma once
#ifndef WINDOW_SORT_H
#define WINDOW_SORT_H
//////////////////////////////////////
#include "window-info/window-info.h"
#include "window-utils/window-utils.h"
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
#define WINDOW_SORT_PARAMS    const struct WINDOW_SORT_STRUCT *w0, const struct WINDOW_SORT_STRUCT *w1
typedef int (*window_sort_function)(WINDOW_SORT_PARAMS);
typedef int (^window_sort_block)(WINDOW_SORT_PARAMS);
enum window_sort_direction_t {
  WINDOW_SORT_DIRECTION_ASC,
  WINDOW_SORT_DIRECTION_DESC,
  WINDOW_SORT_DIRECTIONS_QTY,
};
static const char *window_sort_direction_keys[] = {
  [WINDOW_SORT_DIRECTION_ASC]  = "asc",
  [WINDOW_SORT_DIRECTION_DESC] = "desc",
};
static const char *window_sort_direction_names[] = {
  [WINDOW_SORT_DIRECTION_ASC]  = "Ascending",
  [WINDOW_SORT_DIRECTION_DESC] = "Descending",
};
enum window_sort_type_t {
  WINDOW_SORT_TYPE_WINDOW_ID,
  WINDOW_SORT_TYPE_PID,
  WINDOW_SORT_TYPES_QTY,
};
static const char *window_sort_type_names[] = {
  [WINDOW_SORT_TYPE_PID]       = "pid",
  [WINDOW_SORT_TYPE_WINDOW_ID] = "window-id",
};
static const char *window_sort_type_descriptions[] = {
  [WINDOW_SORT_TYPE_PID]       = "PID",
  [WINDOW_SORT_TYPE_WINDOW_ID] = "Window ID",
};
enum window_sort_option_t {
  WINDOW_SORT_OPTION_WINDOW_ID_ASC,
  WINDOW_SORT_OPTION_WINDOW_ID_DESC,
  WINDOW_SORT_OPTION_PID_ASC,
  WINDOW_SORT_OPTION_PID_DESC,
  WINDOW_SORT_OPTIONS_QTY,
};
struct window_sort_t {
  const enum window_sort_type_t      type;
  const enum window_sort_direction_t direction;
  window_sort_block                  block;
  window_sort_function               function;
};
int window_sort_pid_desc(WINDOW_SORT_PARAMS);
int window_sort_pid_asc(WINDOW_SORT_PARAMS);
int window_sort_window_id_desc(WINDOW_SORT_PARAMS);
int window_sort_window_id_asc(WINDOW_SORT_PARAMS);
char *get_window_sort_types_description();
window_sort_function get_window_sort_function_from_key(const char *name, const char *direction);
static const struct window_sort_t window_sorts[] = {
  [WINDOW_SORT_OPTION_PID_ASC] =        {
    .type                      = WINDOW_SORT_TYPE_PID,
    .direction                 = WINDOW_SORT_DIRECTION_ASC,
    .function                  = window_sort_pid_asc,
    .block                     = ^ int (WINDOW_SORT_PARAMS){
      int r                    = ((w0->pid > w1->pid) ? 1 : (w0->pid < w1->pid) ? -1 : 0);
      return(r);
    },
  },
  [WINDOW_SORT_OPTION_PID_DESC] =       {
    .type                       = WINDOW_SORT_TYPE_PID,
    .direction                  = WINDOW_SORT_DIRECTION_DESC,
    .function                   = window_sort_pid_desc,
    .block                      = ^ int (WINDOW_SORT_PARAMS){
      int r                     = ((w0->pid < w1->pid) ? 1 : (w0->pid > w1->pid) ? -1 : 0);
      return(r);
    }, },
  [WINDOW_SORT_OPTION_WINDOW_ID_ASC] =  {
    .type                            = WINDOW_SORT_TYPE_WINDOW_ID,
    .direction                       = WINDOW_SORT_DIRECTION_ASC,
    .function                        = window_sort_window_id_asc,
    .block                           = ^ int (WINDOW_SORT_PARAMS){
      return((w0->window_id > w1->window_id) ? 1 : (w0->window_id < w1->window_id) ? -1 : 0);
    }, },
  [WINDOW_SORT_OPTION_WINDOW_ID_DESC] = {
    .type                             = WINDOW_SORT_TYPE_WINDOW_ID,
    .direction                        = WINDOW_SORT_DIRECTION_DESC,
    .function                         = window_sort_window_id_desc,
    .block                            = ^ int (WINDOW_SORT_PARAMS){
      return((w0->window_id < w1->window_id) ? 1 : (w0->window_id > w1->window_id) ? -1 : 0);
    },
  },
};
#endif
