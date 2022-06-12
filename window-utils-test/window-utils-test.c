#include "window-utils-test.h"
#define DEFAULT_WINDOW_NAME_PATTERN    "kitty"
#ifndef LOGLEVEL
#define LOGLEVEL                       DEFAULT_LOGLEVEL
#endif
#include "app-utils.h"
#include "submodules/greatest/greatest.h"
#include "submodules/log.h/log.h"
#include "window-utils.h"


APP_AUTHORIZATION_TESTS
int main(int argc, char **argv) {
  (void)argc; (void)argv;
  RUN_APP_AUTHORIZATION_TESTS
  LsWinCtx ctx;
  int      ch;
  ctx.longDisplay = 0;
  ctx.id          = -1;
  ctx.numFound    = 0;
  char *pattern = malloc(64);

//  sprintf(pattern, "%s", "kitty");
//  sprintf(pattern, "%s", "alacritty");
  if (DEFAULT_WINDOW_NAME_PATTERN != NULL) {
    sprintf(pattern, "%s", DEFAULT_WINDOW_NAME_PATTERN);
  }    else{
    pattern = NULL;
  }

  log_debug("Pattern:%s|ID:%d|", pattern, ctx.id);
  EnumerateWindows(pattern, PrintWindow, (void *)&ctx);
  log_debug("%s> ctx.numFound: %d|", APPLICATION_NAME, ctx.numFound);
  END_APP_AUTHORIZATION_TESTS
  return(0);
}
