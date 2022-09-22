#pragma once
#ifndef WINDOW_INFO_H
#define WINDOW_INFO_H
//////////////////////////////////////
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
enum window_info_sort_type_t {
  WINDOW_INFO_SORT_WINDOW_ID_ASC,
  WINDOW_INFO_SORT_WINDOW_ID_DESC,
  WINDOW_INFO_SORT_PID_ASC,
  WINDOW_INFO_SORT_PID_DESC,
  WINDOW_INFO_SORT_TYPES_QTY,
};
#define WINDOW_INFO_SORT_PARAMS    const struct window_info_t *w0, const struct window_info_t *w1
typedef int (*window_info_sort_function)(WINDOW_INFO_SORT_PARAMS);
window_info_sort_function get_window_info_sort_function_from_key(const char *name, const char *direction);
#endif
