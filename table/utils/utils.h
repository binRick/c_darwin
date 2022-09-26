#pragma once
#ifndef TABLE_UTILS_H
#define TABLE_UTILS_H
#include <fnmatch.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
///////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app/utils/utils.h"
#include "app/utils/utils.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "process/utils/utils.h"
#include "window/info/info.h"
#include "window/utils/utils.h"
///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
#define LIST_TABLE_ITEMS()                                             \
  LIST_TABLE(list_window_table, "Windows", WINDOW, window_info_t)      \
  LIST_TABLE(list_process_table, "Processes", PROCESS, process_info_t) \
  LIST_TABLE(list_usb_table, "USB Devices", USB, usbdev_t)             \
  LIST_TABLE(list_kitty_table, "Kitty Processes", KITTY, kitty_t)      \
  LIST_TABLE(list_spaces_table, "Spaces", SPACE, space_t)              \
  LIST_TABLE(list_displays_table, "Displays", DISPLAY, display_t)      \
  LIST_TABLE(list_app_table, "Applications", APP, app_t)               \
  LIST_TABLE(list_hotkey_table, "Hotkeys", HOTKEY, key_t)              \
  LIST_TABLE(list_monitor_table, "Monitors", MONITOR, monitor_t)       \
  LIST_TABLE(list_font_table, "Fonts", FONT, font_t)                   \
////////////////////////////////////////////////////////////////////////
enum table_dur_type_t {
  TABLE_DUR_TYPE_COLORS,
  TABLE_DUR_TYPE_TOTAL,
  TABLE_DUR_TYPE_QUERIES,
  TABLE_DUR_TYPE_QUERY_ITEMS,
  TABLE_DUR_TYPE_QUERY_SPACES,
  TABLE_DUR_TYPE_QUERY_CUR_DISPLAY,
  TABLE_DUR_TYPE_QUERY_WINDOWS,
  TABLE_DUR_TYPE_SORT_ROWS,
  TABLE_DUR_TYPE_FILTER_ROWS,
  TABLE_DUR_TYPE_FORT,
  TABLE_DUR_TYPES_QTY,
};
struct table_dur_t {
  unsigned long started;
  unsigned long dur;
};
enum table_row_filter_type_t {
  TABLE_ROW_FILTER_CURRENT_SPACE,
  TABLE_ROW_FILTER_CURRENT_DISPLAY,
  TABLE_ROW_FILTER_MINIMIZED,
  TABLE_ROW_FILTER_NOT_MINIMIZED,
  TABLE_ROW_FILTERS_QTY,
};
typedef bool (^table_row_filter_b)(void *row);
struct table_row_filter_t {
  char               *name;
  table_row_filter_b filter;
};
static struct table_row_filter_t __attribute__((unused)) table_row_filters[] = {
  [TABLE_ROW_FILTER_CURRENT_SPACE]                                           = { .name = "Current Space", .filter    = ^ bool (void *row){
                                                                                   return(((struct window_info_t *)row)->is_minimized == false);
                                                                                 },
  },
  [TABLE_ROW_FILTER_MINIMIZED] =                                               { .name = "Is Minimized", .filter     = ^ bool (void *row){
                                                                                   return(((struct window_info_t *)row)->is_minimized == true);
                                                                                 },
  },
  [TABLE_ROW_FILTER_NOT_MINIMIZED] =                                           { .name = "Is Not Minimized", .filter = ^ bool (void *row){
                                                                                   return(((struct window_info_t *)row)->is_minimized == false);
                                                                                 },
  },
};
enum table_order_key_type_t {
  TABLE_ORDER_PID,
  TABLE_ORDER_WINDOW_ID,
  TABLE_ORDER_APPLICATION,
  TABLE_ORDER_WINDOW_SIZE,
  TABLE_ORDER_DISPLAY_ID,
  TABLE_ORDER_SPACE_ID,
  TABLE_ORDER_MINIMIZED,
  TABLE_ORDER_DURATION,
  TABLE_ORDERS_QTY,
};
enum table_order_direction_type_t {
  TABLE_ORDER_DIRECTION_ASC,
  TABLE_ORDER_DIRECTION_DESC,
  TABLE_ORDER_DIRECTIONS_QTY,
};
struct list_table_t {
  struct Vector *windows_v;
  bool          current_space_only, current_display_only, exact_match, case_sensitive, duplicate, non_duplicate;
  size_t        space_id, display_id, window_id;
  char          *sort_key, *sort_direction, *application_name, *font_family, *font_name, *font_type, *font_style;
  int           width_less, width_greater, height_less, height_greater, width, height, limit;
  pid_t         pid;
};
struct table_order_t {
  char                              *key, *name;
  enum table_order_direction_type_t default_order_direction;
};
static struct table_order_t __attribute__((unused)) table_orders[] = {
  [TABLE_ORDER_PID]                                                = { .key = "pid", .name         = "PID", .default_order_direction         = TABLE_ORDER_DIRECTION_DESC, },
  [TABLE_ORDER_WINDOW_ID]                                          = { .key = "window-id", .name   = "Window ID", .default_order_direction   = TABLE_ORDER_DIRECTION_DESC, },
  [TABLE_ORDER_APPLICATION]                                        = { .key = "application", .name = "Application", .default_order_direction = TABLE_ORDER_DIRECTION_DESC, },
  [TABLE_ORDER_WINDOW_SIZE]                                        = { .key = "window-size", .name = "Window Size", .default_order_direction = TABLE_ORDER_DIRECTION_DESC, },
  [TABLE_ORDER_DISPLAY_ID]                                         = { .key = "display-id", .name  = "Display ID", .default_order_direction  = TABLE_ORDER_DIRECTION_DESC, },
  [TABLE_ORDER_SPACE_ID]                                           = { .key = "space-id", .name    = "Space ID", .default_order_direction    = TABLE_ORDER_DIRECTION_DESC, },
  [TABLE_ORDER_MINIMIZED]                                          = { .key = "minimized", .name   = "Minimized", .default_order_direction   = TABLE_ORDER_DIRECTION_DESC, },
  [TABLE_ORDER_DURATION]                                           = { .key = "duration", .name    = "Duration", .default_order_direction    = TABLE_ORDER_DIRECTION_DESC, },
};
static const char *table_dur_type_names[] = {
  [TABLE_DUR_TYPE_COLORS]            = "colors",
  [TABLE_DUR_TYPE_QUERIES]           = "queries",
  [TABLE_DUR_TYPE_QUERY_SPACES]      = "query spaces",
  [TABLE_DUR_TYPE_QUERY_WINDOWS]     = "query windows",
  [TABLE_DUR_TYPE_QUERY_CUR_DISPLAY] = "query current display",
  [TABLE_DUR_TYPE_TOTAL]             = "total",
  [TABLE_DUR_TYPE_FORT]              = "fort",
  [TABLE_DUR_TYPE_SORT_ROWS]         = "sort items",
};
int list_window_table(void *ARGS);
int list_spaces_table(void *ARGS);
int list_displays_table(void *ARGS);
int list_hotkey_table(void *ARGS);
int list_app_table(void *ARGS);
int list_installed_fonts_table(void *ARGS);
int list_usb_table(void *ARGS);
int list_process_table(void *ARGS);
int list_kitty_table(void *ARGS);
int list_monitor_table(void *ARGS);

#define LIST_TABLE(FXN, NAME, TYPE, STRUCT_TYPE) \
  int FXN(void *ARGS);
LIST_TABLE_ITEMS()
#undef LIST_TABLE
#endif
