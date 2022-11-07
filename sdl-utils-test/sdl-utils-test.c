
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "sdl-utils-test/sdl-utils-test.h"
#include "sdl-utils/sdl-utils.h"
#include "timestamp/timestamp.h"

TEST t_sdl_utils_test1(){
  sdl_utils_main();
  PASS();
}

SUITE(s_sdl_utils_test) {
  RUN_TEST(t_sdl_utils_test1);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_sdl_utils_test);
  GREATEST_MAIN_END();
}
