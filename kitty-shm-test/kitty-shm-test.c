
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "kitty-shm-test/kitty-shm-test.h"
#include "kitty/shm/shm.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

TEST t_kitty_shm_get(){
  int r = kitty_shm_get();

  ASSERT_EQ(r, EXIT_SUCCESS);
  PASS();
}

TEST t_kitty_msg_image_by_path(){
  char *f   = "/tmp/m.png";
  char *msg = kitty_msg_get_display_image_by_path(f);
  int  c    = fprintf(stdout, "%s", msg);

  ASSERT_GT(c, 0);
  fflush(stdout);
  PASS();
}

TEST t_kitty_shm_set(){
  int r = kitty_shm_set();

  ASSERT_EQ(r, EXIT_SUCCESS);
  PASS();
}

SUITE(s_kitty_shm_test) {
  RUN_TEST(t_kitty_shm_set);
  RUN_TEST(t_kitty_shm_get);
  RUN_TEST(t_kitty_msg_image_by_path);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_kitty_shm_test);
  GREATEST_MAIN_END();
}
