#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "layout-test/layout-test.h"
#include "layout/layout.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

TEST t_layout_test2(){
  PASS();
}

TEST t_layout_test1(){
  lay_context ctx;

  lay_init_context(&ctx);
  lay_reserve_items_capacity(&ctx, 1024);
  lay_id root = lay_item(&ctx);

#define WIDTH        1800
#define TERMWIDTH    (WIDTH * .60)
  lay_set_size_xy(&ctx, root, WIDTH, 1500);
  lay_id terminals = lay_item(&ctx);

  lay_insert(&ctx, root, terminals);
  lay_set_size_xy(&ctx, terminals, TERMWIDTH, 0);
  lay_set_behave(&ctx, terminals, LAY_RIGHT | LAY_VFILL);
  lay_set_contain(&ctx, terminals, LAY_ROW);
  lay_id t1 = lay_item(&ctx);
  lay_set_behave(&ctx, t1, LAY_FILL);
  lay_insert(&ctx, terminals, t1);
  lay_id browsers = lay_item(&ctx);

  lay_insert(&ctx, root, browsers);
  lay_set_behave(&ctx, browsers, LAY_LEFT | LAY_VFILL);
  lay_set_contain(&ctx, browsers, LAY_ROW);
  lay_id b1 = lay_item(&ctx);
  lay_id b2 = lay_item(&ctx);
  lay_set_behave(&ctx, b1, LAY_TOP | LAY_HFILL);
  lay_set_behave(&ctx, b2, LAY_BOTTOM | LAY_HFILL);
  lay_insert(&ctx, browsers, b1);
  lay_insert(&ctx, browsers, b2);

  lay_run_context(&ctx);
  lay_vec4 b1r = lay_get_rect(&ctx, b1);
  lay_vec4 b2r = lay_get_rect(&ctx, b2);
  lay_vec4 t1r = lay_get_rect(&ctx, t1);
  lay_vec4 br  = lay_get_rect(&ctx, browsers);

  for (int i = 0; i < 4; i++) {
    log_debug("t1                   #%d:   %d", i, t1r[i]);
    log_debug("browsers_rect        #%d:   %d", i, br[i]);
    log_debug("b1                   #%d:   %d", i, b1r[i]);
    log_debug("b2                   #%d:   %d", i, b2r[i]);
  }
  /*my_ui_library_draw_box_x_y_width_height(
   *  terminal_rect[0],
   *  terminal_rect[1],
   *  terminal_rect[2],
   *  terminal_rect[3]);*/
  lay_reset_context(&ctx);
  lay_destroy_context(&ctx);
  PASS();
} /* t_layout_test1 */

SUITE(s_layout_test) {
  RUN_TEST(t_layout_test1);
  if (isatty(STDOUT_FILENO))
    RUN_TEST(t_layout_test2);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_layout_test);
  GREATEST_MAIN_END();
}
