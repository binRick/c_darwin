#include <stdbool.h>
//////////////////////////////////////////
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/include/vector.h"
#include "kitty-test.h"
#include "kitty/kitty-module.h"
#include "kitty/kitty.h"
//////////////////////////////////////////
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
static char *msg;
static void __kitty_test_constructor(void) __attribute__((constructor));
static void __kitty_test_destructor(void) __attribute__((destructor));

static module(kitty_utils) * KU;


//////////////////////////////////////////


TEST t_kitty_module_pids(void){
  asprintf(&msg, "%lu Kitty PIDs", vector_size(KU->pids_v));
  PASSm(msg);
}


TEST t_kitty_module_pids_child_pids(void){
  for (size_t i = 0; i < KU->pids_qty; i++) {
    int           pid           = (int)(size_t)vector_get(KU->pids_v, i);
    struct Vector *child_pids_v = get_child_pids(pid);
    printf("pid: %d, children: %lu", pid, vector_size(child_pids_v));
    for (size_t ii = 0; ii < vector_size(child_pids_v); ii++) {
      int cpid = (int)(size_t)vector_get(child_pids_v, ii);
      printf("\tcpid: %d\n", cpid);
    }
  }
  PASS();
}


TEST t_kitty_module_pids_windowid(void){
  size_t              cwds_qty = 0;
  struct StringBuffer *sb      = stringbuffer_new();

  for (size_t i = 0; i < KU->pids_qty; i++) {
    int pid       = (int)(size_t)vector_get(KU->pids_v, i);
    int window_id = get_kitty_pid_windowid(pid);
    printf("pid:%d | window id: %d\n", pid, window_id);
  }
  stringbuffer_release(sb);
  PASS();
}


TEST t_kitty_module_pids_cwd(void){
  size_t              cwds_qty = 0;
  struct StringBuffer *sb      = stringbuffer_new();

  for (size_t i = 0; i < KU->pids_qty; i++) {
    int  pid  = (int)(size_t)vector_get(KU->pids_v, i);
    char *cwd = get_process_cwd(pid);
    cwds_qty++;
    stringbuffer_append_string(sb, "\n\t");
    stringbuffer_append_int(sb, pid);
    stringbuffer_append_string(sb, " => ");
    stringbuffer_append_string(sb, cwd);
  }
  asprintf(&msg, "Acquired %lu cmd lines from %lu Kitty PIDs: %s", cwds_qty, vector_size(KU->pids_v), stringbuffer_to_string(sb));
  stringbuffer_release(sb);
  PASSm(msg);
}


TEST t_kitty_module_pids_cmdline(void){
  size_t              cmdlines_qty = 0;
  struct StringBuffer *sb          = stringbuffer_new();

  for (size_t i = 0; i < KU->pids_qty; i++) {
    int           pid            = (int)(size_t)vector_get(KU->pids_v, i);
    struct Vector *pid_cmdline_v = get_process_cmdline(pid);
    cmdlines_qty += vector_size(pid_cmdline_v);
    stringbuffer_append_string(sb, "\n\t");
    stringbuffer_append_int(sb, pid);
    stringbuffer_append_string(sb, " => ");
    for (size_t ii = 0; ii < vector_size(pid_cmdline_v); ii++) {
      stringbuffer_append_string(sb, (char *)vector_get(pid_cmdline_v, ii));
      stringbuffer_append_string(sb, " ");
    }
  }
  asprintf(&msg, "Acquired %lu cmd lines from %lu Kitty PIDs: %s", cmdlines_qty, vector_size(KU->pids_v), stringbuffer_to_string(sb));
  stringbuffer_release(sb);
  PASSm(msg);
}


TEST t_kitty_module_pids_env(void){
  size_t              env_vars_qty = 0;
  struct StringBuffer *sb          = stringbuffer_new();

  for (size_t i = 0; i < KU->pids_qty; i++) {
    int           pid         = (int)(size_t)vector_get(KU->pids_v, i);
    struct Vector *pid_envs_v = get_process_env(pid);
    env_vars_qty += vector_size(pid_envs_v);
    stringbuffer_append_int(sb, vector_size(pid_envs_v));
    stringbuffer_append_string(sb, " envs");
    if (i < KU->pids_qty - 1) {
      stringbuffer_append_string(sb, ", ");
    }
  }
  asprintf(&msg, "Acquired %lu env vars from %lu Kitty PIDs: %s", env_vars_qty, vector_size(KU->pids_v), stringbuffer_to_string(sb));
  stringbuffer_release(sb);
  PASSm(msg);
}


TEST t_kitty_module_iterate_pids(void){
  struct StringBuffer *sb = stringbuffer_new();

  for (size_t i = 0; i < KU->pids_qty; i++) {
    int pid = (int)(size_t)vector_get(KU->pids_v, i);
    stringbuffer_append_int(sb, pid);
    if (i < KU->pids_qty - 1) {
      stringbuffer_append_string(sb, ", ");
    }
  }
  asprintf(&msg, "Iterated %lu Kitty PIDs: %s", vector_size(KU->pids_v), stringbuffer_to_string(sb));
  stringbuffer_release(sb);
  PASSm(msg);
}


TEST t_kitty_module_parse_listen_ons(void){
  struct StringBuffer *sb = stringbuffer_new();

  for (size_t i = 0; i < KU->listen_ons_qty; i++) {
    kitty_listen_on_t *parsed = parse_kitten_listen_on((char *)vector_get(KU->listen_ons_v, i));
    stringbuffer_append_string(sb, parsed->protocol);
    stringbuffer_append_string(sb, "://");
    stringbuffer_append_string(sb, parsed->host);
    stringbuffer_append_string(sb, ":");
    stringbuffer_append_int(sb, parsed->port);
    if (i < KU->listen_ons_qty - 1) {
      stringbuffer_append_string(sb, ", ");
    }
  }
  asprintf(&msg, "Parsed %lu Kitty Listen Ons: %s", vector_size(KU->listen_ons_v), stringbuffer_to_string(sb));
  stringbuffer_release(sb);
  PASSm(msg);
}


TEST t_kitty_module_listen_ons(void){
  struct StringBuffer *sb = stringbuffer_new();

  for (size_t i = 0; i < KU->listen_ons_qty; i++) {
    stringbuffer_append_string(sb, (char *)vector_get(KU->listen_ons_v, i));
    if (i < KU->listen_ons_qty - 1) {
      stringbuffer_append_string(sb, ", ");
    }
  }
  asprintf(&msg, "%lu Kitty Listen Ons: %s", vector_size(KU->listen_ons_v), stringbuffer_to_string(sb));
  stringbuffer_release(sb);
  PASSm(msg);
}

SUITE(s_kitty_pid_tests){
  RUN_TEST(t_kitty_module_pids);
  RUN_TEST(t_kitty_module_pids_env);
  RUN_TEST(t_kitty_module_pids_cmdline);
  RUN_TEST(t_kitty_module_pids_child_pids);
  RUN_TEST(t_kitty_module_pids_cwd);
  RUN_TEST(t_kitty_module_pids_windowid);
  RUN_TEST(t_kitty_module_iterate_pids);
  RUN_TEST(t_kitty_module_listen_ons);
  RUN_TEST(t_kitty_module_parse_listen_ons);
  RUN_TEST(t_kitty_module_pids_child_pids);
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_kitty_pid_tests);
  GREATEST_MAIN_END();
  clib_module_free(KU);
  return(0);
}

static void __attribute__((destructor)) __kitty_test_destructor(){
  clib_module_free(KU);
}

static void __attribute__((constructor)) __kitty_test_constructor(){
  KU = require(kitty_utils);
}

