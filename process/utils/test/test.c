
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "process-utils-test/process-utils-test.h"
#include "process/utils/utils.h"
#include "process/process.h"
#include "timestamp/timestamp.h"
#include "window/utils/utils.h"

TEST t_process_utils_focused_pid(){
  pid_t pid = get_focused_pid();

  ASSERT_GTm("Unable to acquire focused pid", pid, 0);
  char *msg;

  asprintf(&msg, "Pid %d is focused", pid);
  PASSm(msg);
}

TEST t_process_utils_pid_env(){
  struct Vector *v = get_process_env(getpid());

  ASSERT_GTm("Unable to acquire pid env", vector_size(v), 0);
  char *msg;

  asprintf(&msg, "Pid %d has %lu Env Vars", getpid(), vector_size(v));
  PASSm(msg);
}

TEST t_process_utils_pid_child_pids(){
  pid_t         pid = 1;
  struct Vector *v  = get_child_pids(pid);

  ASSERT_GTm("Unable to acquire child pids", vector_size(v), 0);
  char *msg;

  asprintf(&msg, "Pid %d has %lu child PIDs", pid, vector_size(v));
  PASSm(msg);
}

TEST t_process_utils_pid_ppids(){
  struct Vector *v = get_process_ppids(getpid());

  ASSERT_GTm("Unable to acquire ppids", vector_size(v), 0);
  char *msg;

  asprintf(&msg, "Pid %d has %lu PPIDs", getpid(), vector_size(v));
  PASSm(msg);
}

TEST t_process_utils_my_cwd(){
  char *p = get_my_cwd();
  char *msg;

  asprintf(&msg, "PID %d has cwd %s", getpid(), p);
  PASSm(msg);
}

TEST t_process_utils_pid_path(){
  char *p = pid_path(getpid());
  char *msg;

  asprintf(&msg, "PID %d has Path %s", getpid(), p);
  PASSm(msg);
}

TEST t_process_utils_pid_ppid(){
  pid_t ppid = get_process_ppid(getpid());

  ASSERT_GTm("Unable to acquire ppid of current pid", ppid, 0);
  char *msg;

  asprintf(&msg, "PID %d has PPID %d", getpid(), ppid);
  PASSm(msg);
}

TEST t_process_utils_pids(){
  struct Vector *v = get_all_processes();

  ASSERT_GTm("Unable to acquire pids", vector_size(v), 0);
  char *msg;

  asprintf(&msg, "The system has %lu PIDs", vector_size(v));
  PASSm(msg);
}

TEST t_process_utils_pid_cmdline(){
  struct Vector *cmdline_v = get_process_cmdline(getpid());

  ASSERT_GTm("Unable to acquire cmdline of current pid", vector_size(cmdline_v), 0);
  struct StringBuffer *sb = stringbuffer_new();

  for (size_t i = 0; i < vector_size(cmdline_v); i++) {
    stringbuffer_append_string(sb, (char *)vector_get(cmdline_v, i));
    if (i < vector_size(cmdline_v) - 1) {
      stringbuffer_append_string(sb, " ");
    }
  }
  char *cmdline = stringbuffer_to_string(sb);
  char *msg;

  asprintf(&msg, "PID %d has cmdline %s", getpid(), cmdline);
  PASSm(msg);
}

TEST t_process_utils_run_osascript_system_prefs(){
  run_osascript_system_prefs();
  PASS();
}

TEST t_process_utils_pid_cwd(){
  char *my_cwd = get_pid_cwd(getpid());
  char *msg;

  asprintf(&msg, "PID %d has cwd %s", getpid(), my_cwd);
  free(my_cwd);
  PASSm(msg);
}

SUITE(s_process_utils_test) {
  RUN_TEST(t_process_utils_pid_cwd);
  RUN_TEST(t_process_utils_pid_cmdline);
  RUN_TEST(t_process_utils_pids);
  RUN_TEST(t_process_utils_pid_ppid);
  RUN_TEST(t_process_utils_pid_path);
  RUN_TEST(t_process_utils_my_cwd);
  RUN_TEST(t_process_utils_pid_ppids);
  RUN_TEST(t_process_utils_pid_child_pids);
  RUN_TEST(t_process_utils_run_osascript_system_prefs);
//  RUN_TEST(t_process_utils_pid_env);
  RUN_TEST(t_process_utils_focused_pid);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_process_utils_test);
  GREATEST_MAIN_END();
}
#include "window/info/info.h"
