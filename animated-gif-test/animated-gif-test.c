////////////////////////////////////////////
#include "animated-gif-test/animated-gif-test.h"
#include "animated-gif/animated-gif.h"
#include "c_greatest/greatest/greatest.h"
#include "tempdir.c/tempdir.h"
////////////////////////////////////////////

TEST t_animated_gif_test(void){
  int res = load_pngs_create_animated_gif(gettempdir());

  ASSERT_EQ(res, 0);
  PASS();
}

SUITE(s_animated_gif_test) {
  RUN_TEST(t_animated_gif_test);
}

GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  GREATEST_MAIN_BEGIN();
  if (isatty(STDOUT_FILENO)) {
  }
  RUN_SUITE(s_animated_gif_test);
  GREATEST_MAIN_END();
}
