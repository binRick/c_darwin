////////////////////////////////////////////

////////////////////////////////////////////
#include "screen-utils-test/screen-utils-test.h"
#include "screen-utils/screen-utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"

////////////////////////////////////////////
TEST t_screen_utils_test_init_display(){
  struct screen_capture_t *D = init_display(kCGDirectMainDisplay);

  free(D);
  PASS();
}

TEST t_screen_utils_test_init_screen_capture(){
  struct screen_capture_t *C = init_screen_capture();

  free(C);
  PASS();
}

TEST t_screen_utils_test_save_capture(){
  struct screen_capture_t *C = screen_capture();
  bool                    ok = save_captures(C);

  ASSERT_EQ(ok, true);
  free(C);
  PASS();
}

TEST t_screen_utils_test_capture(){
  struct screen_capture_t *C = screen_capture();

  free(C);
  PASS();
}

SUITE(s_screen_utils_test_capture) {
  RUN_TEST(t_screen_utils_test_capture);
  RUN_TEST(t_screen_utils_test_save_capture);
}

SUITE(s_screen_utils_test_init) {
  RUN_TEST(t_screen_utils_test_init_display);
  RUN_TEST(t_screen_utils_test_init_screen_capture);
}
GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_screen_utils_test_init);
  RUN_SUITE(s_screen_utils_test_capture);
  GREATEST_MAIN_END();
}
