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
#include "app/utils/utils.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_timer/include/c_timer.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "string-utils/string-utils.h"
#include "submodules/libfort/src/fort.h"
#include "window/utils/test/test.h"
#include "window/utils/utils.h"

////////////////////////////////////////

TEST t_authorized_tests(void){
  authorized_test_t *authorized_test_results = execute_authorization_tests();

  for (size_t i = 0; i < AUTHORIZED_TEST_TYPE_IDS_QTY; i++) {
    log_info("Auth test #%lu> %s => %s :: %s", i, authorized_test_results[i].name, authorized_test_results[i].authorized == true ? "OK" : "Failed", milliseconds_to_string(authorized_test_results[i].dur_ms));
    ASSERT_EQ(authorized_test_results[i].authorized, true);
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
              "App",
              "PID",
              "Conn",
              "Window",
              "Disp",
              "Space",
              "Size",
              "Position",
              "Layer",
              "Focused",
              "Visible",
              "Min",
              "PPID"

              );

  ct_start(NULL);
  struct Vector *windows = get_window_infos_v();

  for (size_t i = 0; i < vector_size(windows); i++) {
    struct window_info_t *w = (struct window_info_t *)vector_get(windows, i);

    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED);
    ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);

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
  log_info("dur:%s", dur);
  ft_destroy_table(table);

  PASS();
} /* t_list_windows */

SUITE(s_list_windows){
  RUN_TEST(t_list_windows);
}

TEST t_info(){
  log_info("focused_window_title:%s", get_focused_window_title());
  PASS();
}

TEST t_capture_to_memory(){
  PASS();
}
SUITE(s_capture_to_memory){
  RUN_TEST(t_capture_to_memory);
}
SUITE(s_window_info){
  RUN_TEST(t_info);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  (void)argc; (void)argv;
  GREATEST_MAIN_BEGIN();
  RUN_TEST(t_authorized_tests);
  RUN_SUITE(s_capture_to_memory);
  //RUN_SUITE(s_windows);
  //RUN_SUITE(s_list_windows);
  //RUN_SUITE(s_window_info);

  GREATEST_MAIN_END();
  return(0);
}