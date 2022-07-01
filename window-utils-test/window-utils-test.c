#include "window-utils-test.h"
#define DEFAULT_WINDOW_NAME_PATTERN    "kitty"
#include "app-utils/app-utils.h"
#include "greatest/greatest.h"
#include "window-utils/window-utils.h"

APP_AUTHORIZATION_TESTS


TEST t_windows_search(void *NAME){
  LsWinCtx ctx;
  int      ch;

  ctx.longDisplay = 0;
  ctx.id          = -1;
  ctx.numFound    = 0;
  char *pattern = NULL;

  if ((char *)NAME != NULL) {
    pattern = (char *)strdup(NAME);
  }

  EnumerateWindows(pattern, PrintWindow, (void *)&ctx);
  printf("\n"
         "%s> ctx.numFound: %d|"
         "\t"
         "Pattern:%s|ID:%d|\n",
         APPLICATION_NAME, ctx.numFound,
         pattern, ctx.id
         );
  PASS();
}


TEST t_authorized_tests(void){
  authorized_test_t *authorized_test_results = execute_authorization_tests();

  for (int i = 0; i < (sizeof(*authorized_test_results) / sizeof((authorized_test_results)[0])); i++) {
    ASSERT_EQm(authorized_test_results[i].name, authorized_test_results[i].authorized, true);
  }
}


TEST t_windows_qty(void){
  int qty = get_windows_qty();

  ASSERT_GT(qty, 1);
  printf("get_windows_qty:%d\n", qty);
  PASS();
}

SUITE(s_windows){
  RUN_TEST(t_windows_qty);
  RUN_TESTp(t_windows_search, (void *)NULL);
  RUN_TESTp(t_windows_search, (void *)"kitty");
  RUN_TESTp(t_windows_search, (void *)"chrome");
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  (void)argc; (void)argv;
  GREATEST_MAIN_BEGIN();
  RUN_TEST(t_authorized_tests);
  RUN_SUITE(s_authorized);
  RUN_SUITE(s_windows);

  GREATEST_MAIN_END();
  return(0);
}
