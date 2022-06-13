#include "submodules/log.h/log.h"
#include "system-utils-test.h"
//////////////////////////////////////////

//////////////////////////////////////////


//////////////////////////////////////////
TEST t_display_id(void){
  CGDirectDisplayID display_id = get_display_id(0);

  fprintf(stdout,
          "display id:%d\n",
          display_id
          );
  PASS();
}


TEST t_display_count(void){
  int display_count = get_display_count();

  fprintf(stdout,
          "display count:%d\n",
          display_count
          );
  PASS();
}


TEST t_display_size(void){
  CGDirectDisplayID              display_id = get_display_id(0);
  struct DarwinDisplayResolution *res       = get_display_resolution(display_id);

  fprintf(stdout,
          "x:%zu|y:%zu|pixels:%d|width:%fmm|height:%fmm|\n",
          res->x,
          res->y,
          res->pixels,
          res->size.width,
          res->size.height
          );
  PASS();
}

SUITE(s_display_count){
  RUN_TEST(t_display_count);
  PASS();
}

SUITE(s_display_id){
  RUN_TEST(t_display_id);
  PASS();
}

SUITE(s_display_size){
  RUN_TEST(t_display_size);
  PASS();
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_display_id);
  RUN_SUITE(s_display_size);
  RUN_SUITE(s_display_count);
  GREATEST_MAIN_END();
  return(0);
}
