
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "frameworks/frameworks.h"
#include "log/log.h"
#include "mouse/utils/test/test.h"
#include "mouse/utils/utils.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

TEST t_mouse_utils_mouse_right_click(){
  right_click_mouse();
}

TEST t_mouse_utils_mouse_left_click(){
  left_click_mouse();
}

TEST t_mouse_utils_mouse_move_to_point(){
  CGPoint *loc1  = get_mouse_location();
  CGPoint newloc = { .x = (int)(loc1->x) + 15, .y = (int)(loc1->y) - 35 };

  move_mouse_to_point(newloc);
  CGPoint *loc = get_mouse_location();

  ASSERT_EQ((int)(loc->x), (int)(newloc.x));
  ASSERT_EQ((int)(loc->y), (int)(newloc.y));
  char *msg;

  asprintf(&msg, "Mouse moved from %dx%d to %dx%d", (int)loc1->x, (int)loc1->y, (int)loc->x, (int)loc->y);
  PASSm(msg);
}

TEST t_mouse_utils_mouse_move(){
  CGPoint *loc1 = get_mouse_location();
  int     x = (int)loc1->x + 25, y = (int)loc1->y - 25;

  move_mouse(x, y);
  CGPoint *loc = get_mouse_location();

  ASSERT_EQ((int)(loc->x), x);
  ASSERT_EQ((int)(loc->y), y);
  char *msg;

  asprintf(&msg, "Mouse moved from %dx%d to %dx%d", (int)loc1->x, (int)loc1->y, (int)loc->x, (int)loc->y);
  PASSm(msg);
}

TEST t_mouse_utils_mouse_location(){
  CGPoint *loc = get_mouse_location();

  ASSERT_GT((int)(loc->x), 0);
  ASSERT_GT((int)(loc->y), 0);
  char *msg;

  asprintf(&msg, "Mouse is at %dx%d", (int)loc->x, (int)loc->y);
  PASSm(msg);
}

SUITE(s_mouse_utils_test) {
  RUN_TEST(t_mouse_utils_mouse_location);
  RUN_TEST(t_mouse_utils_mouse_move);
  RUN_TEST(t_mouse_utils_mouse_move_to_point);
  RUN_TEST(t_mouse_utils_mouse_left_click);
  RUN_TEST(t_mouse_utils_mouse_right_click);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_mouse_utils_test);
  GREATEST_MAIN_END();
}
