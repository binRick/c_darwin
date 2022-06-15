#include "greatest.h"
#include "keylogger-test.h"
#include <stdio.h>

//////////////////////////////////////////////


TEST t_keylogger_test(){
  int res = keylogger_exec();

  PASS();
}

GREATEST_MAIN_DEFS();

SUITE(s_keylogger_test){
  RUN_TEST(t_keylogger_test);
  PASS();
}


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_keylogger_test);
  GREATEST_MAIN_END();
  return(0);
}

