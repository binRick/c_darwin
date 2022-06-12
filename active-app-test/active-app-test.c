#include "active-app-module.h"
#include "active-app-test.h"
#include "app-utils.h"
#include "submodules/greatest/greatest.h"
#include "submodules/log.h/log.h"
#include "window-utils.h"
bool RUNNING = true;


void show_focus(){
  focused_t *fp = get_focused_process();

  fprintf(stdout,
          "<%d> Focus: Name:%10s | Pid:%8d | \n",
          getpid(),
          fp->name,
          fp->pid
          );
}


TEST test_currently_focused_app(void) {
  focused_t *fp = get_focused_process();

  ASSERT_NEQm("focused process seems invalid", NULL, fp);
  ASSERT_GTm("PID seems invalid", fp->pid, 0);
  ASSERT_GTm("Prcess name seems invalid", strlen(fp->name), 0);
  log_debug("name:%s", fp->name);
  log_debug("pid:%d", fp->pid);
  PASS();
}


SUITE(the_suite) {
  RUN_TEST(test_currently_focused_app);
}


GREATEST_MAIN_DEFS();


int do_test(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  for (int i = 0; i < TEST_ITERATIONS; i++) {
    RUN_SUITE(the_suite);
  }
  GREATEST_MAIN_END();
  return(0);
}


int main(int argc, char **argv) {
  (void)argc; (void)argv;
  if ((argc >= 2) && (strcmp(argv[1], "--watch") == 0)) {
    while (RUNNING == true) {
      show_focus();
      usleep(WATCH_INTERVAL_MS * 1000);
    }
    return(0);
  }
  return(do_test(argc, argv));
}
