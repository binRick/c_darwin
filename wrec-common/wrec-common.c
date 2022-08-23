////////////////////////////////////////////////////
#include <stdbool.h>
#include <string.h>
////////////////////////////////////////////////////
#include "wrec-common/wrec-common.h"
////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "libfort/lib/fort.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "msf_gif/msf_gif.h"
#include "timestamp/timestamp.h"
#include "wildcardcmp/wildcardcmp.h"
#include "wrec-common/extern.h"
////////////////////////////////////////////////////
static const char *EXCLUDED_WINDOW_APP_NAMES[] = {
  "Control Center",
  "Rectangle",
  "SystemUIServer",
  "Window Server",
  NULL,
};

int list_windows(void *ARGS) {
  struct args_t execution_args = *(struct args_t *)ARGS;
  ft_table_t    *table         = ft_create_table();

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
              "Application",
              "PID",
              "Window ID",
              "Size",
              "Position",
              "Layer",
              "Focused",
              "Visible",
              "Minimized"
              );

  struct Vector *windows = get_windows();

  for (size_t i = 0; i < vector_size(windows); i++) {
    window_t *w         = (window_t *)vector_get(windows, i);
    int      NAME_MATCH = wildcardcmp(execution_args.application_name_glob, strip_non_ascii(w->app_name));
    if (NAME_MATCH == 0) {
      continue;
    }
    if ((int)w->position.y < 25 && (int)w->size.height < 50 && w->layer > 0) {
      continue;
    }
    char **tmp           = EXCLUDED_WINDOW_APP_NAMES;
    bool excluded_window = false;
    while (*tmp != NULL && excluded_window == false) {
      if (strcmp(*tmp, w->app_name) == 0) {
        excluded_window = true;
      }

      tmp++;
    }
    if (excluded_window) {
      continue;
    }
    ft_printf_ln(table,
                 "%.20s|%d|%d|%dx%d|%dx%d|%d|%s|%s|%s",
                 strip_non_ascii(w->app_name),
                 w->pid,
                 w->window_id,
                 (int)w->size.height, (int)w->size.width,
                 (int)w->position.x, (int)w->position.y,
                 w->layer,
                 int_to_string(w->is_focused),
                 int_to_string(w->is_visible),
                 int_to_string(w->is_minimized)
                 );

    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED);
    ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);

    if (w->position.x != 0 || w->position.y != 0) {
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
    if (w->layer != 0) {
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }else{
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }
    if (w->is_focused) {
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
    if (w->is_visible) {
      ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
    if (w->is_minimized) {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
  }

  printf("\n%s\n", ft_to_string(table));
  ft_destroy_table(table);

  return(0);
} /* list_windows */

void debug_resize(int WINDOW_ID,
                  char *FILE_NAME,
                  int RESIZE_TYPE, int RESIZE_VALUE,
                  int ORIGINAL_WIDTH, int ORIGINAL_HEIGHT,
                  long unsigned CAPTURE_DURATION_MS, long unsigned SAVE_DURATION_MS) {
  char *msg;

  asprintf(&msg,
           "<%d> [debug_resize] "
           "\n\t|RESIZE_TYPE:%d"
           "|RESIZE_VALUE:%s"
           "\n\t|WINDOW_ID:%d"
           "|FILE_NAME:%s"
           "|FILE_SIZE:%ld"
           "\n\t|ORIGINAL_WIDTH:%d|ORIGINAL_HEIGHT:%d"
           "\n\t|CAPTURE_DUR_MS:%ld|SAVE_DURATION_MS:%ld",
           getpid(),
           RESIZE_TYPE,
           resize_type_name(RESIZE_VALUE),
           WINDOW_ID,
           FILE_NAME,
           fsio_file_size(FILE_NAME),
           ORIGINAL_WIDTH, ORIGINAL_HEIGHT,
           CAPTURE_DURATION_MS, SAVE_DURATION_MS
           );

  log_debug("%s\n", msg);

  free(msg);
}

int compare_file_time_items(struct file_time_t *e1, struct file_time_t *e2){
  int ret = (e1->file_creation_ts > e2->file_creation_ts)
              ? 1
              : (e1->file_creation_ts < e2->file_creation_ts)
                ? -1
                : 0;

  return(ret);
}
