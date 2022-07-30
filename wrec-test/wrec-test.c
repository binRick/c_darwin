#define DEBUG_MODE                false
#define DEBUG_KITTEN_LISTEN_ON    false
//#include "debug-memory/debug_memory.h"
#include "wrec-test/wrec-test.h"
#include "wrec/wrec.h"


TEST t_wrec0(void){
  int res = wrec0();

  ASSERT_EQ(res, 0);
  PASS();
}


SUITE(s_wrec0){
  RUN_TEST(t_wrec0);
  PASS();
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_wrec0);
  GREATEST_MAIN_END();
  return(0);
}
