#define DEBUG_MODE    false
#include "dbg.h"
#include "print.h"
#include "process-test.h"
#define CT_STOP_AND_DEBUG(COLOR)    { do {                                                                             \
                                        PRINT(AC_RESETALL "\t" COLOR "Duration: " AC_RESETALL, ct_stop(__FUNCTION__)); \
                                      }while (0); }


TEST t_pid_cwd(void){
  ct_start(NULL);
  char *cwd = get_process_cwd((int)getpid());

  ASSERT_NEQ(cwd, NULL);
  ASSERT_GTE(strlen(cwd), 0);
  dbg(cwd, %s);
  CT_STOP_AND_DEBUG(AC_BLUE);
  PASS();
}


TEST t_get_process_cmdline(void){
  ct_start(NULL);
  struct Vector *cmdline_v = get_process_cmdline((int)getpid());

  ASSERT_NEQ(cmdline_v, NULL);
  size_t CMDS_QTY = vector_size(cmdline_v);

  ASSERT_GTE(CMDS_QTY, 1);
  for (size_t i = 0; i < CMDS_QTY; i++) {
    char *CMD = (char *)vector_get(cmdline_v, i);
    ASSERT_NEQ(CMD, NULL);
    ASSERT_GTE(strlen(CMD), 0);
    dbg(CMD, %s);
  }
  dbg(CMDS_QTY, %lu);
  vector_release(cmdline_v);
  CT_STOP_AND_DEBUG(AC_RED);
  PASS();
}


TEST t_kitty_pids_connect(void){
  ct_start(NULL);
  struct Vector *KittyProcesses_v = get_kitty_processes();
  dbg(vector_size(KittyProcesses_v), %lu);
  ASSERT_GTE(vector_size(KittyProcesses_v), 0);
  struct Vector *ConnectedKittyProcess_v = connect_kitty_processes(KittyProcesses_v);
  dbg(vector_size(ConnectedKittyProcess_v), %lu);
  CT_STOP_AND_DEBUG(AC_RED);
  PASS();
} /* t_kitty_pids */


TEST t_kitty_pids(void){
  ct_start(NULL);
  struct Vector *KittyProcesses_v = get_kitty_processes();
  dbg(vector_size(KittyProcesses_v), %lu);
  ASSERT_GTE(vector_size(KittyProcesses_v), 0);
  CT_STOP_AND_DEBUG(AC_RED);
  PASS();
} /* t_kitty_pids */


TEST t_pids_iterate(void){
  ct_start(NULL);
  struct Vector *pids_v = get_all_processes();
  ASSERT_NEQ(pids_v, NULL);
  size_t        PIDS_QTY = vector_size(pids_v);
  for (size_t i = 0; i < PIDS_QTY; i++) {
    int pid = (int)(long long)vector_get(pids_v, i);
    if (DEBUG_MODE) {
      dbg(pid, %d);
    }
    if (pid <= 1) {
      continue;
    }
    char *cwd = get_process_cwd(pid);
    if (cwd == NULL) {
      continue;
    }
    struct Vector *cmdline_v = get_process_cmdline(pid);
    if (cmdline_v == NULL) {
      continue;
    }
    struct Vector *PE = get_process_env(pid);
    if (PE == NULL) {
      continue;
    }
    size_t CMDS_QTY = vector_size(cmdline_v);
    size_t ENV_QTY  = vector_size(PE);
    if (DEBUG_MODE) {
      dbg(CMDS_QTY, %lu);
      dbg(ENV_QTY, %lu);
      dbg(cwd, %s);
    }
    for (size_t i = 0; i < ENV_QTY; i++) {
      char *ENV_KEY = ((process_env_t *)vector_get(PE, i))->key,
           *ENV_VAL = ((process_env_t *)vector_get(PE, i))->val;
      if (DEBUG_MODE) {
        dbg(ENV_KEY, %s);
        dbg(ENV_VAL, %s);
      }
      free(ENV_KEY);
      free(ENV_VAL);
      free(((process_env_t *)vector_get(PE, i)));
    }
    continue;
    vector_release(cmdline_v);
    vector_release(PE);
  }
  vector_release(pids_v);
  CT_STOP_AND_DEBUG(AC_CYAN);
} /* t_pids_iterate */


TEST t_pids(void){
  ct_start(NULL);
  struct Vector *pids_v = get_all_processes();

  ASSERT_NEQ(pids_v, NULL);
  size_t PIDS_QTY = vector_size(pids_v);

  ASSERT_GTE(PIDS_QTY, 0);
  for (size_t i = 0; i < PIDS_QTY; i++) {
    long long PID = (long long)vector_get(pids_v, i);
    ASSERT_GTE(PID, 0);
    if (DEBUG_MODE) {
      dbg(PID, %llu);
    }
  }
  dbg(PIDS_QTY, %lu);
  vector_release(pids_v);
  char *dur = ct_stop(__FUNCTION__);
  dbg(dur, %s);

  PASS();
}


TEST t_process_env(void){
  ct_start(NULL);
  struct Vector *PE = get_process_env((int)getpid());

  ASSERT_NEQ(PE, NULL);
  size_t ENV_QTY = vector_size(PE);

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
  char *dur = ct_stop(__FUNCTION__);
  dbg(dur, %s);

  PASS();
}

SUITE(s_process){
  ct_start(NULL);
  RUN_TEST(t_process_env);
  RUN_TEST(t_pids);
  RUN_TEST(t_pid_cwd);
  RUN_TEST(t_get_process_cmdline);
  RUN_TEST(t_pids_iterate);
  RUN_TEST(t_kitty_pids);
  RUN_TEST(t_kitty_pids_connect);
  CT_STOP_AND_DEBUG(AC_RED_BLACK);
  PASS();
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  ct_start(NULL);
  RUN_SUITE(s_process);
  CT_STOP_AND_DEBUG(AC_BLUE_BLACK AC_ITALIC);
  GREATEST_MAIN_END();
  return(0);
}
