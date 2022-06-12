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
int main(int argc, char **argv) {
  (void)argc; (void)argv;
  RUN_APP_AUTHORIZATION_TESTS
    END_APP_AUTHORIZATION_TESTS
  return(0);
}
