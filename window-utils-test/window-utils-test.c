////////////////////////////////////////
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
////////////////////////////////////////
#include "app-utils/app-utils.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_timer/include/c_timer.h"
#include "capture/capture.h"
#include "c_greatest/greatest/greatest.h"
#include "string-utils/string-utils.h"
#include "submodules/c_deps/submodules/dbg/dbg.h"
#include "submodules/libfort/src/fort.h"
#include "window-utils-test/window-utils-test.h"
#include "window-utils/window-utils.h"


////////////////////////////////////////


TEST t_windows_search(void *NAME){
  LsWinCtx ctx;
  int      ch;
  char     *name = (char *)NAME;

  ctx.longDisplay = 0;
  ctx.id          = -1;
  ctx.numFound    = 0;
  char *pattern = NULL;

  if (name != NULL) {
    pattern = strdup(name);

    EnumerateWindows(pattern, PrintWindow, (void *)&ctx);
    printf("\n"
           "%s> ctx.numFound: %d|"
           "\t"
           "Pattern:%s|ID:%d|\n",
           APPLICATION_NAME, ctx.numFound,
           pattern, ctx.id
           );
  }
  PASS();
}


TEST t_authorized_tests(void){
  authorized_test_t *authorized_test_results = execute_authorization_tests();

  for (int i = 0; i < (sizeof(*authorized_test_results) / sizeof((authorized_test_results)[0])); i++) {
    ASSERT_EQm(authorized_test_results[i].name, authorized_test_results[i].authorized, true);
  }
}


TEST t_windows_qty(void){
  int qty = get_windows_qty();

  ASSERT_GT(qty, 1);
  printf("get_windows_qty:%d\n", qty);
  PASS();
}

SUITE(s_windows){
  RUN_TEST(t_windows_qty);
  RUN_TESTp(t_windows_search, (void *)NULL);
  RUN_TESTp(t_windows_search, (void *)"kitty");
  RUN_TESTp(t_windows_search, (void *)"chrome");
}


TEST t_list_windows(){
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

  ct_start(NULL);
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
  char *dur = ct_stop("");


  printf("\n%s\n", ft_to_string(table));
  dbg(dur, %s);
  ft_destroy_table(table);


  PASS();
} /* t_list_windows */

SUITE(s_list_windows){
  RUN_TEST(t_list_windows);
}


TEST t_move_window(void *ID){
  size_t id = (size_t)ID, x = 200, y = 300;

  dbg(id, %lu);
  move_window_id(id, x, y);
  PASS();
}

SUITE(s_move_window){
  RUN_TESTp(t_windows_search, (void *)"Alacritty");
  RUN_TESTp(t_move_window, (void *)69);
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  (void)argc; (void)argv;
  GREATEST_MAIN_BEGIN();
  RUN_TEST(t_authorized_tests);
  //RUN_SUITE(s_authorized);
  RUN_SUITE(s_windows);
  RUN_SUITE(s_move_window);
  RUN_SUITE(s_list_windows);

  GREATEST_MAIN_END();
  return(0);
}
