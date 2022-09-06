#pragma once
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "frameworks/frameworks.h"
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
static bool TABLE_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__table_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_TABLE_UTILS") != NULL) {
    log_debug("Enabling Table Utils Debug Mode");
    TABLE_UTILS_DEBUG_MODE = true;
  }
}

int list_spaces_table(void *ARGS) {
  unsigned long list_windows_table_started = timestamp();
  int           terminal_width             = get_terminal_width();
  struct Vector *spaces_v                  = get_spaces_v();
  ft_table_t    *table                     = ft_create_table();

  {
    ft_set_border_style(table, FT_FRAME_STYLE);
    ft_set_border_style(table, FT_SOLID_ROUND_STYLE);
    ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_RIGHT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 0);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);

    ft_write_ln(table,
                "ID",
                "Active",
                "Windows",
                "#"
                );
  }

  for (size_t i = 0; i < vector_size(spaces_v); i++) {
    struct space_t *space = (struct space_t *)vector_get(spaces_v, i);
    ft_printf_ln(table,
                 "%d"
                 "|%s"
                 "|%s"
                 "|%lu"
                 "%s",
                 space->id,
                 space->is_current ? "Yes" : "No",
                 vector_size_ts_to_csv(space->window_ids_v, terminal_width / 5),
                 vector_size(space->window_ids_v),
                 ""

                 );

    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
    if (space->is_current == true) {
      ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
    }else{
      ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GRAY);
    }
    if (vector_size(space->window_ids_v) > 0) {
      ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_YELLOW);
    }
  }
  char *table_s = ft_to_string(table);

  fprintf(stdout,
          "" AC_YELLOW AC_BOLD AC_UNDERLINE "%lu Spaces" AC_RESETALL
          "\n",
          vector_size(spaces_v)
          );
  fprintf(stdout, "%s\n", table_s);
  unsigned long list_windows_table_dur = timestamp() - list_windows_table_started;

  if (TABLE_UTILS_DEBUG_MODE == true) {
    log_info("Rendered %s %lu row, %lu column table from %lu spaces in %s",
             bytes_to_string(strlen(table_s)),
             ft_row_count(table),
             ft_col_count(table),
             vector_size(spaces_v),
             milliseconds_to_string(list_windows_table_dur)
             );
  }
  ft_destroy_table(table);

  return(0);
} /* list_windows_table */

int list_windows_table(void *ARGS) {
  unsigned long              list_windows_table_started = timestamp();
  struct list_window_table_t *args                      = 0;

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
    ft_set_border_style(table, FT_FRAME_STYLE);
    ft_set_border_style(table, FT_SOLID_ROUND_STYLE);
    ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_RIGHT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 0);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);

    ft_write_ln(table,
                "Application",
                "PID",
                "Win",
                "Disp",
                "Spc",
                "Size",
                "Position",
                "Lyr/Lvl",
                "Foc", "Vis", "Min",
                "Abv/Blw"
                );
  }

  if (TABLE_UTILS_DEBUG_MODE == true) {
    log_debug("loaded %lu windows|current space id:%d|%lu displays|%lu spaces|args spaceid: %d|",
              vector_size(windows),
              get_space_id(),
              vector_size(display_ids_v),
              vector_size(space_ids_v),
              args->space_id
              );
  }

  struct Vector *table_windows_v = vector_new();
  struct Vector *table_space_ids = vector_new();
  int           max_app_name_len = 0;

  for (size_t i = 0; i < vector_size(windows); i++) {
    struct window_t *w = (struct window_t *)vector_get(windows, i);
    if (args->space_id > 0 && w->space_id != args->space_id) {
      continue;
    }
    if (args->current_space_only == true && w->space_id != cur_space_id) {
      continue;
    }
    if (w == NULL || window_is_excluded(w) == true) {
      continue;
    }
    if ((int)w->position.y < 25 && (int)w->size.height < 50 && w->layer > 24) {
      //continue;
    }
    max_app_name_len = (strlen(w->app_name) > (size_t)max_app_name_len) ? (int)strlen(w->app_name) : max_app_name_len;
    vector_push(table_windows_v, (void *)w);
  }
  for (size_t i = 0; i < vector_size(table_windows_v); i++) {
    struct window_t *w = (struct window_t *)vector_get(table_windows_v, i);
    ft_printf_ln(table,
                 "  %-*s  |%-5d|%-5lu|%.5s|%d|%4dx%-4d|%4dx%-4d|%d/%d|%-3s|%-3s|%3s|%2lu/%2lu",
                 max_app_name_len + 4, w->app_name,
                 w->pid,
                 w->window_id,
                 w->display_uuid,
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
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);

    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);

    ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);

    if (w->space_id == cur_space_id) {
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
    }else{
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GRAY);
    }
    ft_set_cell_prop(table, i + 1, 4, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);

    if (w->size.width < 0 || w->size.height < 0) {
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_INVERTED);
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    }else if (w->size.width == 0 && w->size.height == 0) {
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GRAY);
    }else{
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_CYAN);
    }

    if ((int)w->position.x < 0 || (int)w->position.y < 0) {
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    }else{
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_YELLOW);
    }

    ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_YELLOW);

    if (w->is_focused == true) {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_INVERTED | FT_TSTYLE_BOLD);
    }else{
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
    ft_set_cell_prop(table, i + 1, 8, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);

    if (w->is_visible == true) {
      ft_set_cell_prop(table, i + 1, 9, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table, i + 1, 9, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_INVERTED | FT_TSTYLE_BOLD);
    }else{
      ft_set_cell_prop(table, i + 1, 9, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }

    if (w->is_minimized == true) {
      ft_set_cell_prop(table, i + 1, 10, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table, i + 1, 10, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_INVERTED | FT_TSTYLE_BOLD);
    }else{
      ft_set_cell_prop(table, i + 1, 10, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
  }
  ft_add_separator(table);
  ft_set_cell_span(table, vector_size(table_windows_v) + 1, 5, 1);

  ft_printf_ln(table,
               "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
               "",
               "",
               "",
               "",
               "",
               "",
               "",
               "",
               "",
               "",
               "",
               ""
               );
  ft_printf_ln(table,
               "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
               "",
               "",
               "",
               "",
               "",
               "",
               "",
               "",
               "",
               "",
               "",
               ""
               );

  char *table_s = ft_to_string(table);

  printf("%s\n", table_s);
  unsigned long list_windows_table_dur = timestamp() - list_windows_table_started;

  if (TABLE_UTILS_DEBUG_MODE == true) {
    log_info("Rendered %s %lu row, %lu column table from %lu windows in %s",
             bytes_to_string(strlen(table_s)),
             ft_row_count(table),
             ft_col_count(table),
             vector_size(windows),
             milliseconds_to_string(list_windows_table_dur)
             );
  }
  ft_destroy_table(table);

  return(0);
} /* list_windows_table */
