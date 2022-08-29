//////////////////////////////////////////////
#include <stdio.h>
//////////////////////////////////////////////
#include "keylogger-test.h"
//////////////////////////////////////////////
#ifdef ASSERT_EQ
#undef ASSERT_EQ
#endif
#include "submodules/c_greatest/greatest/greatest.h"

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

void __kl__init(void){
  fprintf(stdout, "=======================TEST INIT=============================\n");
}

__attribute__((constructor)) void init(void){
  __kl__init();
}

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  if (isatty(STDOUT_FILENO)) {
    RUN_SUITE(s_keylogger);
  }
  GREATEST_MAIN_END();
}
