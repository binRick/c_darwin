#pragma once
#include "wrec/wrec.h"


int list_windows() {
  printf("list wins............\n");
  ft_table_t *table = ft_create_table();

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
              "Minimized",
              "PPID"

              );

  struct Vector *windows = get_windows();
  for (size_t i = 0; i < vector_size(windows); i++) {
    window_t *w = (window_t *)vector_get(windows, i);
    ft_printf_ln(table,
                 "%.20s|%d|%d|%dx%d|%dx%d|%d|%s|%s|%s|%d",
                 strip_non_ascii(w->app_name),
                 w->pid,
                 w->window_id,
                 (int)w->size.height, (int)w->size.width,
                 (int)w->position.x, (int)w->position.y,
                 w->layer,
                 int_to_string(w->is_focused),
                 int_to_string(w->is_visible),
                 int_to_string(w->is_minimized),
                 w->pid_info.kp_eproc.e_ppid
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
    if (w->pid_info.kp_eproc.e_ppid > 1) {
      ft_set_cell_prop(table, i + 1, 9, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 9, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
  }

  printf("\n%s\n", ft_to_string(table));
  ft_destroy_table(table);

  return(0);
} /* list_windows */


int wrec0() {
  printf("wrec0............\n");
  return(0);
}
