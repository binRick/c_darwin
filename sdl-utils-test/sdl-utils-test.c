
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
#include "incbin/incbin.h"
#include "vips/vips.h"
INCBIN(kitty_icon_png, "assets/kitty_icon.png");

TEST t_sdl_utils_vips(){
  VipsImage *vi = vips_image_new_from_buffer(gkitty_icon_pngData,gkitty_icon_pngSize,"",NULL);
  Dbg(vips_image_get_width(vi),%d);
  Dbg(vips_image_get_height(vi),%d);
  int rc = su_vi(vi);
  ASSERT_EQ(rc,EXIT_SUCCESS);
  PASS();
}
TEST t_sdl_utils_test1(){
  sdl_utils_main();
  PASS();
}

SUITE(s_sdl_utils_vips) {
  RUN_TEST(t_sdl_utils_vips);

}
SUITE(s_sdl_utils_test) {
  RUN_TEST(t_sdl_utils_test1);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  if(isatty(STDOUT_FILENO)){
    RUN_SUITE(s_sdl_utils_test);
    RUN_SUITE(s_sdl_utils_vips);
  }
  GREATEST_MAIN_END();
}
