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


TEST t_set_currently_focused_app(void) {
  int pid = get_focused_pid();

  printf("setting focused process to pid %d.....\n", pid);
  size_t limit = 3, on = 0;

  while (on < limit) {
    bool ok = set_focused_pid(pid);
    ASSERT_EQm("failed to focus process", true, ok);
    printf("set focused process. to '%d'....\n", pid);
    sleep(1);
    on++;
  }

  PASS();
}


TEST t_get_currently_focused_app(void) {
  focused_t *fp = get_focused_process();

  ASSERT_NEQm("focused process seems invalid", NULL, fp);
  ASSERT_GTm("PID seems invalid", fp->pid, 0);
  ASSERT_GTm("Prcess name seems invalid", strlen(fp->name), 0);
  log_debug("name:%s", fp->name);
  log_debug("pid:%d", fp->pid);
  PASS();
}


SUITE(s_get_currently_focused) {
  RUN_TEST(t_get_currently_focused_app);
  PASS();
}
SUITE(s_set_currently_focused) {
  RUN_TEST(t_set_currently_focused_app);
  PASS();
}


GREATEST_MAIN_DEFS();


int do_test(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_get_currently_focused);
  RUN_SUITE(s_set_currently_focused);
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
