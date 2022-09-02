#pragma once
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils-extern.h"
#include "core-utils/core-utils.h"
#include "libfort/lib/fort.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process-utils/process-utils.h"
#include "process/process.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
#include "submodules/log.h/log.h"
#include "system-utils/system-utils.h"
#include "table-utils/table-utils.h"
#include "timestamp/timestamp.h"
///////////////////////////////////////////////////////////////////////////////

int list_windows_table(void *ARGS) {
  struct list_window_table_t *args = 0;

  if (ARGS != 0) {
    args = (struct list_window_table_t *)ARGS;
  }
  struct Vector *windows; struct Vector *display_ids_v; struct Vector *space_ids_v;
  int           cur_space_id;
  ft_table_t    *table = ft_create_table();

  {
    windows       = get_windows();
    display_ids_v = get_display_ids_v();
    space_ids_v   = get_space_ids_v();
    cur_space_id  = get_space_id();
  }

  {
    ft_set_border_style(table, FT_SOLID_ROUND_STYLE);
    ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 0);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);

    ft_write_ln(table,
                "App",
                "PID",
                "Win",
                "Disp",
                "Space",
                "Size",
                "Pos",
                "Layer/Level",
                "Focused",
                "Vis",
                "Min",
                "# Above/Below"
                );
  }

  log_debug("loaded %lu windows|current space id:%d|%lu displays|%lu spaces",
            vector_size(windows),
            get_space_id(),
            vector_size(display_ids_v),
            vector_size(space_ids_v)
            );

  struct Vector *table_windows_v = vector_new();

  for (size_t i = 0; i < vector_size(windows); i++) {
    struct window_t *w = (struct window_t *)vector_get(windows, i);
    if (args->current_space_only == true && w->space_id != cur_space_id) {
      continue;
    }
    if (w == NULL || window_is_excluded(w) == true) {
      continue;
    }
    if ((int)w->position.y < 25 && (int)w->size.height < 50 && w->layer > 24) {
      //continue;
    }
    vector_push(table_windows_v, (void *)w);
  }
  for (size_t i = 0; i < vector_size(table_windows_v); i++) {
    struct window_t *w = (struct window_t *)vector_get(table_windows_v, i);
    ft_printf_ln(table,
                 "%s|%d|%lu|%d|%d|%dx%d|%dx%d|%d/%d|%s|%s|%s|%lu/%lu",
                 w->app_name,
                 w->pid,
                 w->window_id,
                 w->display_index,
                 w->space_id,
                 (int)w->size.height, (int)w->size.width,
                 (int)w->position.x, (int)w->position.y,
                 w->layer, w->level,
                 int_to_string(w->is_focused),
                 int_to_string(w->is_visible),
                 int_to_string(w->is_minimized),
                 vector_size(w->window_ids_above), vector_size(w->window_ids_below)
                 );

    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED);

    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);

    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED);

    ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);

    if (w->space_id == cur_space_id) {
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GRAY);
    }

    if (w->size.width == 0 && w->size.height == 0) {
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GRAY);
    }else{
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_CYAN);
    }

    ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);

    if (w->is_focused) {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }

    if (w->is_visible) {
      ft_set_cell_prop(table, i + 1, 9, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 9, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }

    if (w->is_minimized) {
      ft_set_cell_prop(table, i + 1, 10, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 10, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
  }
  printf("\n%s\n", ft_to_string(table));
  ft_destroy_table(table);

  return(0);
} /* list_windows_table */
