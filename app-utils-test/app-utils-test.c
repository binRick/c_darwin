#include "app-utils-test.h"
#include "app-utils.h"
#ifndef LOGLEVEL
#define LOGLEVEL    DEFAULT_LOGLEVEL
#endif
#include "c_greatest/greatest/greatest.h"
#include "submodules/c_greatest/greatest/greatest.h"
#include "submodules/log.h/log.h"
#include "window-utils.h"
#include <stdbool.h>
#include <stdio.h>

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  (void)argc; (void)argv;
  GREATEST_MAIN_BEGIN();
  execute_authorization_tests();
//  RUN_SUITE(s_authorized);
  GREATEST_MAIN_END();
  return(0);
}
