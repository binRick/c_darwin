//////////////////////////////////////////////
#include <stdio.h>
//////////////////////////////////////////////
#include "keylogger-test.h"
//////////////////////////////////////////////
#ifdef ASSERT_EQ
#undef ASSERT_EQ
#endif
#include "submodules/greatest/greatest.h"


//////////////////////////////////////////////


TEST t_keylogger(){
  int res = keylogger_exec();

  ASSERT_EQm("keylogger_exec failed", res, 0);
  PASS();
}

GREATEST_MAIN_DEFS();

SUITE(s_keylogger){
  RUN_TEST(t_keylogger);
}


void __init(void){
  fprintf(stdout, "=======================TEST INIT=============================\n");
}

__attribute__((constructor)) void init(void){
  __init();
}


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_keylogger);
  GREATEST_MAIN_END();
}

