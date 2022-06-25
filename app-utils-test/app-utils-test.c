#include "app-utils-test.h"
#include "app-utils.h"
#ifndef LOGLEVEL
#define LOGLEVEL    DEFAULT_LOGLEVEL
#endif
#include "submodules/greatest/greatest.h"
#include "submodules/log.h/log.h"
#include "window-utils.h"
#include <stdbool.h>
#include <stdio.h>


APP_AUTHORIZATION_TESTS
GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  (void)argc; (void)argv;
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_authorized);
  GREATEST_MAIN_END();
  return(0);
}
