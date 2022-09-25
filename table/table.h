#pragma once
#ifndef TABLE_H
#define TABLE_H
//////////////////////////////////////
#include "app/utils/utils.h"
#include "core-utils/core-utils.h"
#include "font-utils/font-utils.h"
#include "frameworks/frameworks.h"
#include "hotkey-utils/hotkey-utils.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "space/utils/utils.h"
#include "string-utils/string-utils.h"
#include "table/utils/utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "window/info/info.h"
//#include "table/sort/sort.h"
#include "c_vector/vector/vector.h"
#include "libfort/lib/fort.h"
//////////////////////////////////////
#define MAX_COLUMNS    20
typedef struct Vector *(^query_items_b)(void *);
typedef void (^row_b)(ft_table_t *table, size_t index, void *item);
typedef void (^row_style_b)(ft_table_t *table, size_t index, void *item);
typedef bool (^row_skip_b)(ft_table_t *table, size_t index, void *item, struct list_table_t *args);
typedef struct Vector *(*query_items_f)(void);
enum table_type_t {
  TABLE_TYPE_KITTY,
  TABLE_TYPE_MONITOR,
  TABLE_TYPE_PROCESS,
  TABLE_TYPE_USB,
  TABLE_TYPE_FONT,
  TABLE_TYPE_APP,
  TABLE_TYPE_WINDOW,
  TABLE_TYPE_SPACE,
  TABLE_TYPE_DISPLAY,
  TABLE_TYPE_HOTKEY,
  TABLE_TYPES_QTY,
};
struct table_logic_t {
  row_b         row;
  row_style_b   row_style;
  row_skip_b    row_skip;
  query_items_f query_items;
  char          *columns[MAX_COLUMNS];
};
#endif
