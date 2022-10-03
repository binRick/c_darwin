////////////////////////////////////////////

////////////////////////////////////////////
#include "screen/utils/test/test.h"
#include "screen/utils/utils-module.h"
#include "screen/utils/utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"

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

TEST t_screen_utils_test_capture(){
  struct screen_capture_t *C = screen_capture();

  free(C);
  PASS();
}

TEST t_screen_utils_test_module_init(){
  ScreenUtils *S = require(screen_utils);

  S->set_log_level(LOG_LEVEL_WARN);
  S->set_log_level(LOG_LEVEL_DEBUG);
  log_info("pid:%d", S->get_pid());

  FreeScreenUtils(S);

  PASS();
}

SUITE(s_screen_utils_test_capture) {
  RUN_TEST(t_screen_utils_test_capture);
}

SUITE(s_screen_utils_test_init) {
  RUN_TEST(t_screen_utils_test_init_display);
  RUN_TEST(t_screen_utils_test_init_screen_capture);
}
SUITE(s_screen_utils_test_module) {
  RUN_TEST(t_screen_utils_test_module_init);
}
GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_screen_utils_test_init);
  RUN_SUITE(s_screen_utils_test_capture);
  RUN_SUITE(s_screen_utils_test_module);
  GREATEST_MAIN_END();
}
