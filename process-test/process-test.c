#define DEBUG_MODE    false
#include "process-test.h"


TEST t_pid_cwd(void){
  char *cwd = get_pid_cwd((int)getpid());

  ASSERT_NEQ(cwd, NULL);
  ASSERT_GTE(strlen(cwd), 0);
  dbg(cwd, %s);
  PASS();
}


TEST t_pids(void){
  struct Vector *pids_v  = get_all_pids();
  size_t        PIDS_QTY = vector_size(pids_v);

  ASSERT_GTE(PIDS_QTY, 0);
  for (size_t i = 0; i < PIDS_QTY; i++) {
    long long PID = (long long)vector_get(pids_v, i);
    ASSERT_GTE(PID, 0);
    if (DEBUG_MODE) {
      dbg(PID, % llu);
    }
  }
  dbg(PIDS_QTY, %lu);
  vector_release(pids_v);

  PASS();
}


TEST t_process_env(void){
  struct Vector *PE     = get_process((int)getpid());
  size_t        ENV_QTY = vector_size(PE);

  ASSERT_GTE(ENV_QTY, 0);
  for (size_t i = 0; i < ENV_QTY; i++) {
    char *ENV_KEY = ((process_env_t *)vector_get(PE, i))->key,
         *ENV_VAL = ((process_env_t *)vector_get(PE, i))->val;
    ASSERT_GTE(strlen(ENV_KEY), 0);
    ASSERT_GTE(strlen(ENV_VAL), 0);
    if (DEBUG_MODE) {
      dbg(ENV_KEY, %s);
      dbg(ENV_VAL, %s);
    }
    free(ENV_KEY);
    free(ENV_VAL);
    free(((process_env_t *)vector_get(PE, i)));
  }
  dbg(ENV_QTY, %lu);
  vector_release(PE);

  PASS();
}

SUITE(s_process){
  RUN_TEST(t_process_env);
  RUN_TEST(t_pids);
  RUN_TEST(t_pid_cwd);
  PASS();
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_process);
  GREATEST_MAIN_END();
  return(0);
}
