#include "../include/includes.h"

uv_thread_t         tid;
uint64_t            start_time;
double              duration;
int                 i, r;
static volatile int num_threads;


static void thread_entry(void *arg) {
  num_threads++;
  fprintf(stderr, "thread!\n");
}


static void exec_cmd(void *cmd){
  // invoke the script with default options
  struct ScriptExecResult result = scriptexec_run(cmd);

  printf("Code: %d\nOutput:\n%s\nError:\n%s\n", result.code, result.out, result.err);

  // invoke the script with custom options
  struct ScriptExecOptions options = scriptexec_create_options();

  options.runner            = "bash";     // default is 'sh'
  options.working_directory = "./target"; // default to current working directory
  options.exit_on_error     = true;       // default true, will break script execution on any error
  options.print_commands    = true;       // default false, if true will print every command before invocation

  result = scriptexec_run_with_options("echo 1\necho 2\necho 3\necho 4", options);
  printf("Code: %d\nOutput:\n%s\nError:\n%s\n", result.code, result.out, result.err);

  free(result.out);
  free(result.err);
}


int test_thread1(){
  char *cmd = "pwd";

  uv_thread_create(&tid, exec_cmd, (void *)cmd);
  uv_thread_join(&tid);
}
