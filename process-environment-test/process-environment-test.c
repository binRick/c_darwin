#define DEBUG_PID_ENV    false
#include "process-environment-test.h"


TEST t_process_env(void){
  struct Vector *PE     = get_process_environment((int)getpid());
  size_t        ENV_QTY = vector_size(PE);

  ASSERT_GTE(ENV_QTY, 0);
  for (size_t i = 0; i < ENV_QTY; i++) {
    char *ENV_KEY = ((process_env_t *)vector_get(PE, i))->key,
         *ENV_VAL = ((process_env_t *)vector_get(PE, i))->val;
    ASSERT_GTE(strlen(ENV_KEY), 0);
    ASSERT_GTE(strlen(ENV_VAL), 0);
    dbg(ENV_KEY, %s);
    dbg(ENV_VAL, %s);
    free(ENV_KEY);
    free(ENV_VAL);
    free(((process_env_t *)vector_get(PE, i)));
  }
  dbg(ENV_QTY, %lu);
  vector_release(PE);

  PASS();
}

SUITE(s_process_env){
  RUN_TEST(t_process_env);
  PASS();
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_process_env);
  GREATEST_MAIN_END();
  return(0);
}
