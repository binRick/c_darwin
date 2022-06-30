#include <stdbool.h>
#define DEBUG_PASTEBOARD_TEST    true
//////////////////////////////////////////
#include "greatest/greatest.h"
#include "pasteboard-test/pasteboard-test.h"


TEST t_pasteboard(void){
  fprintf(stdout,
          "pb OK:%d\n",
          123
          );
  PASS();
}


SUITE(s_pasteboard){
  RUN_TEST(t_pasteboard);
  PASS();
}


GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_pasteboard);
  GREATEST_MAIN_END();
  return(0);
}
