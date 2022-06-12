#include "active-app-module-test.h"
#include "submodules/log.h/log.h"
//////////////////////////////////////////
#define WATCH_INTERVAL_MS    100
#define TEST_ITERATIONS      1
//////////////////////////////////////////
void __show_focus();
int do_test(int argc, char **argv);

//////////////////////////////////////////
bool RUNNING = true;


//////////////////////////////////////////


TEST test_currently_focused_app(void) {
  module(logger) * logger = require(logger);
  logger->mode            = LOGGER_DEBUG;

  int updated = logger->Update();

  ASSERT_EQm("Update Failed", updated, 0);
  ASSERT_GTm("PID seems invalid", logger->PID, 0);
  ASSERT_GTm("PID seems invalid", logger->GetPID(), 0);
  ASSERT_GTm("Prcess name seems invalid", strlen(logger->Name), 0);
  ASSERT_GTm("Prcess name seems invalid", strlen(logger->GetName()), 0);
  log_info("name:%s", logger->Name);
  log_info("pid:%d", logger->PID);

  PASS();
}


SUITE(the_suite) {
  RUN_TEST(test_currently_focused_app);
}


void __show_focus(){
  module(logger) * logger = require(logger);
  logger->mode            = LOGGER_DEBUG;
  int updated = logger->Update();

  //////////////////////////////////////////////
  fprintf(stdout, "Updated:%d\n", updated);
  fprintf(stdout, "\n");
  fprintf(stdout, "PID:%d\n", logger->PID);
  fprintf(stdout, "Name:%s\n", logger->Name);
  fprintf(stdout, "\n");
  fprintf(stdout, "GetPID():%d\n", logger->GetPID());
  fprintf(stdout, "GetName():%s\n", logger->GetName());
  fprintf(stdout, "\n");
  //////////////////////////////////////////////
  clib_module_free(logger);
  //////////////////////////////////////////////
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
      __show_focus();
      usleep(WATCH_INTERVAL_MS * 1000);
    }
    return(0);
  }

  return(do_test(argc, argv));
}
