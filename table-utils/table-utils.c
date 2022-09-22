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
#include "window-info/window-info.h"
///////////////////////////////////////////////////////////////////////////////
static bool TABLE_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__table_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_TABLE_UTILS") != NULL) {
    log_debug("Enabling Table Utils Debug Mode");
    TABLE_UTILS_DEBUG_MODE = true;
  }
}
enum table_dur_type_t {
  TABLE_DUR_TYPE_COLORS,
  TABLE_DUR_TYPE_TOTAL,
  TABLE_DUR_TYPE_QUERIES,
  TABLE_DUR_TYPE_QUERY_SPACES,
  TABLE_DUR_TYPE_QUERY_CUR_DISPLAY,
  TABLE_DUR_TYPE_QUERY_WINDOWS,
  TABLE_DUR_TYPE_FORT,
  TABLE_DUR_TYPES_QTY,
};
struct table_dur_t {
  unsigned long started;
  unsigned long dur;
};
static const char *table_dur_type_names[] = {
  [TABLE_DUR_TYPE_COLORS]            = "colors",
  [TABLE_DUR_TYPE_QUERIES]           = "queries",
  [TABLE_DUR_TYPE_QUERY_SPACES]      = "query spaces",
  [TABLE_DUR_TYPE_QUERY_WINDOWS]     = "query windows",
  [TABLE_DUR_TYPE_QUERY_CUR_DISPLAY] = "query current display",
  [TABLE_DUR_TYPE_TOTAL]             = "total",
  [TABLE_DUR_TYPE_FORT]              = "fort",
};

int list_window_infos_table(void *ARGS) {
  const char *sort_key = "pid", *sort_direction = "desc";

  sort_direction = "asc";
  sort_key       = "pid";
  sort_key       = "window-id";
  sort_direction = "desc";
  int                term_width = get_terminal_width();
  size_t             cur_display_id;
  size_t             cur_space_id;
  ft_table_t         *table;
  struct table_dur_t durs[TABLE_DUR_TYPES_QTY];

  durs[TABLE_DUR_TYPE_TOTAL].started = timestamp();
  for (int o = 0; o < TABLE_DUR_TYPES_QTY; o++) {
    durs[o].dur = 0;
  }

  struct list_table_t __attribute__((unused)) * args;

  if (ARGS != 0) {
    args = (struct list_table_t *)ARGS;
  }
  struct Vector        *window_infos_v;
  struct window_info_t *w;

  {
    unsigned long started = timestamp();
    unsigned long s0;
    s0                                          = timestamp();
    window_infos_v                              = get_window_infos_v();
    durs[TABLE_DUR_TYPE_QUERY_WINDOWS].dur     += (timestamp() - s0);
    s0                                          = timestamp();
    cur_space_id                                = get_space_id();
    durs[TABLE_DUR_TYPE_QUERY_SPACES].dur      += (timestamp() - s0);
    s0                                          = timestamp();
    cur_display_id                              = get_display_id_for_space(cur_space_id);
    s0                                          = timestamp();
    durs[TABLE_DUR_TYPE_QUERY_CUR_DISPLAY].dur += (timestamp() - s0);
    durs[TABLE_DUR_TYPE_QUERIES].dur           += (timestamp() - started);
  }
  {
    struct window_info_t *sorted_window_infos  = calloc((vector_size(window_infos_v) + 1), sizeof(struct window_info_t));
    struct window_info_t *_sorted_window_infos = vector_new();
    for (size_t i = 0; i < vector_size(window_infos_v); i++) {
      sorted_window_infos[i] = *((struct window_info_t *)vector_get(window_infos_v, i));
    }
    qsort(sorted_window_infos, vector_size(window_infos_v), sizeof(struct window_info_t), get_window_info_sort_function_from_key(sort_key, sort_direction));
    for (size_t i = 0; i < vector_size(window_infos_v); i++) {
      vector_push(_sorted_window_infos, (void *)&(sorted_window_infos[i]));
    }
    window_infos_v = _sorted_window_infos;
  }

  {
    unsigned long started = timestamp();
    table = ft_create_table();
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
    durs[TABLE_DUR_TYPE_FORT].dur += (timestamp() - started);
  }

  ft_write_ln(table,
              "ID",
              "PID",
              "Application",
              "Size",
              "Position",
              "Space",
              "Disp",
              "Min",
              "Dur"
              );
  int max_name_len = 0;

  for (size_t i = 0; i < vector_size(window_infos_v); i++) {
    w            = (struct window_info_t *)vector_get(window_infos_v, i);
    max_name_len = (strlen(w->name) > (size_t)max_name_len) ? (int)strlen(w->name) : max_name_len;
  }
  while (term_width > 0 && max_name_len > (term_width / 4)) {
    max_name_len--;
  }
  for (size_t i = 0; i < vector_size(window_infos_v); i++) {
    if (TABLE_UTILS_DEBUG_MODE) {
      for (int o = 0; o < WINDOW_INFO_DUR_TYPES_QTY; o++) {
        log_debug("Window Duration> %s %s: %s",
                  w->name,
                  window_info_dur_type_names[o],
                  milliseconds_to_string(w->durs[o].dur)
                  );
      }
    }
    w = (struct window_info_t *)vector_get(window_infos_v, i);
    ft_printf_ln(table,
                 "%lu|%d|%.*s|%dx%d|%dx%d|%lu|%lu|%s|%s"
                 "%s",
                 w->window_id,
                 w->pid,
                 max_name_len, w->name,
                 (int)w->rect.size.width, (int)w->rect.size.height,
                 (int)w->rect.origin.x, (int)w->rect.origin.y,
                 w->space_id,
                 w->display_id,
                 (w->is_minimized == true) ? "Yes" : "No",
                 milliseconds_to_string(w->dur),
                 ""
                 );

    unsigned long colors_started = timestamp();
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);

    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);

    if (w->rect.size.width < 2 || w->rect.size.height < 2) {
      ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_BLUE);
    }else if (w->rect.size.width == 0 && w->rect.size.height == 0) {
      ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_YELLOW);
    }else{
      ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
    }

    if ((int)w->rect.origin.x < 0 || (int)w->rect.origin.y < 0) {
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
    }else{
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GREEN);
    }
    if (w->space_id == cur_space_id) {
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
    }else{
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_RED);
    }
    if (w->display_id == cur_display_id) {
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_BLUE);
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    }else{
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);
    }
    if (w->is_minimized == true) {
      ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    }else{
      ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }
    if (w->dur == 0) {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else if (w->dur > 20) {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }else if (w->dur > 10) {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
    }else {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_MAGENTA);
    }
    durs[TABLE_DUR_TYPE_COLORS].dur += (timestamp() - colors_started);
  }
  char *table_s = ft_to_string(table);

  printf("%s\n", table_s);

  durs[TABLE_DUR_TYPE_TOTAL].dur = timestamp() - durs[TABLE_DUR_TYPE_TOTAL].started;
  if (TABLE_UTILS_DEBUG_MODE == true) {
    log_info("Rendered %s %lu row, %lu column table from %lu windows in %s",
             bytes_to_string(strlen(table_s)),
             ft_row_count(table),
             ft_col_count(table),
             vector_size(window_infos_v),
             milliseconds_to_string(durs[TABLE_DUR_TYPE_TOTAL].dur)
             );
    for (int o = 0; o < TABLE_DUR_TYPES_QTY; o++) {
      log_debug("Table Duration> %s %s: %s",
                w->name,
                table_dur_type_names[o],
                milliseconds_to_string(durs[o].dur)
                );
    }
  }
  ft_destroy_table(table);
} /* list_window_infos_table */

int list_displays_table(__attribute__((unused)) void *ARGS) {
  unsigned long list_displays_table_started = timestamp();
  struct Vector *displays_v                 = get_displays_v();
  ft_table_t    *table                      = ft_create_table();
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
                "Main",
                "Width",
                "Height",
                "# Spaces"
                );
  }

  for (size_t i = 0; i < vector_size(displays_v); i++) {
    struct display_t *display = (struct display_t *)vector_get(displays_v, i);
    ft_printf_ln(table,
                 "%lu"
                 "|%s"
                 "|%d"
                 "|%d"
                 "|%lu"
                 "%s",
                 display->display_id,
                 display->is_main ? "Yes" : "No",
                 display->width,
                 display->height,
                 vector_size(display->space_ids_v),
                 ""

                 );

    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
  }
  char *table_s = ft_to_string(table);

  fprintf(stdout,
          "" AC_YELLOW AC_BOLD AC_UNDERLINE "%lu Spaces" AC_RESETALL
          "\n",
          vector_size(displays_v)
          );
  fprintf(stdout, "%s\n", table_s);
  unsigned long list_displays_table_dur = timestamp() - list_displays_table_started;

  if (TABLE_UTILS_DEBUG_MODE == true) {
    log_info("Rendered %s %lu row, %lu column table from %lu spaces in %s",
             bytes_to_string(strlen(table_s)),
             ft_row_count(table),
             ft_col_count(table),
             vector_size(displays_v),
             milliseconds_to_string(list_displays_table_dur)
             );
    ft_destroy_table(table);
  }
  return(0);
} /* list_displays_table */

int list_spaces_table(__attribute__((unused)) void *ARGS) {
  unsigned long list_spaces_table_started = timestamp();
  int           terminal_width            = get_terminal_width();
  struct Vector *spaces_v                 = get_spaces_v();
  ft_table_t    *table                    = ft_create_table();

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
  unsigned long list_spaces_table_dur = timestamp() - list_spaces_table_started;

  if (TABLE_UTILS_DEBUG_MODE == true) {
    log_info("Rendered %s %lu row, %lu column table from %lu spaces in %s",
             bytes_to_string(strlen(table_s)),
             ft_row_count(table),
             ft_col_count(table),
             vector_size(spaces_v),
             milliseconds_to_string(list_spaces_table_dur)
             );
  }
  ft_destroy_table(table);

  return(0);
} /* list_spaces_table */

///////////////////////////////////

int list_windows_table(void *ARGS) {
  unsigned long       table_started = timestamp();
  struct list_table_t *args         = 0;

  if (ARGS != 0) {
    args = (struct list_table_t *)ARGS;
  }
  struct Vector *windows;
  int           cur_space_id = -1, cur_display_id = -1;
  ft_table_t    *table = ft_create_table();

  {
    windows        = get_windows();
    cur_space_id   = get_space_id();
    cur_display_id = get_display_id_for_space(cur_space_id);
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
                "Pos",
                "Lyr/Lvl",
                "Foc", "Vis", "Min",
                "Abv/Blw"
                );
  }

  struct Vector *table_windows_v = vector_new();
  int           max_app_name_len = 0;

  for (size_t i = 0; i < vector_size(windows); i++) {
    struct window_t *w = (struct window_t *)vector_get(windows, i);
    if (args->space_id > 0 && (w->space_id != args->space_id)) {
      continue;
    }
    if (args->current_space_only == true && (w->space_id != cur_space_id)) {
      continue;
    }
    if ((int)w->position.y < 25 && (int)w->size.height < 50 && w->layer > 24) {
      continue;
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

  ft_printf_ln(table,
               "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
               "abc",
               "",
               "def",
               "",
               "",
               "",
               "",
               "",
               "",
               ""
               );
  ft_set_cell_span(table, vector_size(table_windows_v) + 1, 0, 2);
  ft_set_cell_span(table, vector_size(table_windows_v) + 1, 2, 3);

  char *table_s = ft_to_string(table);

  printf("%s\n", table_s);
  unsigned long table_dur = timestamp() - table_started;

  if (TABLE_UTILS_DEBUG_MODE == true) {
    log_info("Rendered %s %lu row, %lu column table from %lu windows in %s",
             bytes_to_string(strlen(table_s)),
             ft_row_count(table),
             ft_col_count(table),
             vector_size(windows),
             milliseconds_to_string(table_dur)
             );
  }
  ft_destroy_table(table);

  return(0);
} /* list_windows_table */
