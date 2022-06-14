#include "greatest.h"
#include "pbpaste-test.h"
#include <stdio.h>

//////////////////////////////////////////////


TEST t_pbpaste_test(){
  char *cb = pbpaste_exec();

  fprintf(stderr, "%lub Clipboard: '%s'\n", strlen(cb), cb);
  ASSERT_GT(strlen(cb), 0);
  PASS();
}

GREATEST_MAIN_DEFS();

SUITE(s_pbpaste_test){
  RUN_TEST(t_pbpaste_test);
  PASS();
}


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_pbpaste_test);
  GREATEST_MAIN_END();
  return(0);
}

