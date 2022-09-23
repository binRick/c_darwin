
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "timg-utils-test/timg-utils-test.h"
#include "timg-utils/timg-utils.h"

TEST t_timg_utils_test_binary(){
  bool ok = write_binary_path();

  ASSERT_EQ(ok, true);
  PASS();
}

TEST t_timg_utils_test_images(){
  int ret = timg_utils_test_images();

  ASSERT_EQ(ret, 0);
  PASS();
}

TEST t_timg_utils_test_image(void *img){
  log_debug("%s", (char *)img);
  int ret = timg_utils_image((char *)img);
  ASSERT_EQ(ret, 0);
  PASS();
}

TEST t_timg_utils_test_help(){
  int ret = timg_utils_run_help();

  ASSERT_EQ(ret, 0);
  PASS();
}

SUITE(s_timg_utils_test) {
  RUN_TEST(t_timg_utils_test_binary);
  RUN_TEST(t_timg_utils_test_help);
  char *img = "/tmp/window-25712.png";
  RUN_TESTp(t_timg_utils_test_image, (void *)img);
  RUN_TEST(t_timg_utils_test_images);
  if (isatty(STDOUT_FILENO)) {
  }
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_timg_utils_test);
  GREATEST_MAIN_END();
}
