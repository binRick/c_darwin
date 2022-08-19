////////////////////////////////////////////
#include "rectangle/rectangle.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "assertf/assertf.h"
#include "c_dbg/c_dbg.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/include/vector.h"
#include "process/process.h"
#include "str-flatten.c/src/str-flatten.h"
#include "subprocess.h/subprocess.h"
#include "timestamp/timestamp.h"
#include "tiny-regex-c/re.h"
#include "wildcardcmp/wildcardcmp.h"
#include <signal.h>
////////////////////////////////////////////
#define DEBUG_STDOUT               false
#define STDOUT_READ_BUFFER_SIZE    1024 * 16
////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__rectangle_test();
static void __attribute__((destructor)) __destructor__rectangle_test();

static char *RECTANGLE_APP_PATH   = "/Applications/Rectangle.app";
static char *RECTANGLE_CONFIG_KEY = "com.knollsoft.Rectangle";

bool rectangle_set_todo_width(const int NEW_TODO_WIDTH){
  char *cmd;

  asprintf(&cmd, "defaults write %s todoSidebarWidth -float %d", RECTANGLE_CONFIG_KEY, NEW_TODO_WIDTH);
  rectangle_run_cmd(cmd);
  bool ok = (rectangle_get_todo_width() == NEW_TODO_WIDTH);

  return(ok);
}

int rectangle_get_todo_width(){
  char *cmd;

  asprintf(&cmd, "defaults read %s todoSidebarWidth -float", RECTANGLE_CONFIG_KEY);
  int todo_width = atoi(rectangle_run_cmd(cmd));

  return(todo_width);
}

bool rectangle_run(){
  int pid = rectangle_get_pid();

  if (pid > 2) {
    return(true);
  }
  char *cmd;

  asprintf(&cmd, "open %s", RECTANGLE_APP_PATH);
  rectangle_run_cmd(cmd);
  pid = rectangle_get_pid();
  bool ok = (pid > 2) ? true : false;

  return(ok);
}

bool rectangle_kill(){
  int pid = rectangle_get_pid();

  if (pid <= 1) {
    return(true);
  }
  int  res = kill(pid, SIGINT);
  bool ok  = (res == 0) ? true : false;

  return(ok);
}

bool rectangle_set_todo_app(const char *NEW_TODO_APP){
  char *cmd;

  asprintf(&cmd, "defaults write %s todoApplication -string %s", RECTANGLE_CONFIG_KEY, NEW_TODO_APP);
  rectangle_run_cmd(cmd);
  bool ok = true;

  return(ok);
}

bool rectangle_set_todo_mode_enabled(){
  char *cmd;

  asprintf(&cmd, "defaults write %s todoMode -string %d", RECTANGLE_CONFIG_KEY, 1);
  rectangle_run_cmd(cmd);
  bool ok = true;

  return(ok);
}

bool rectangle_set_todo_mode_disabled(){
  char *cmd;

  asprintf(&cmd, "defaults write %s todoMode -string %d", RECTANGLE_CONFIG_KEY, 0);
  rectangle_run_cmd(cmd);
  bool ok = true;

  return(ok);
}

char *rectangle_get_todo_app(){
  char *cmd;

  asprintf(&cmd, "defaults read %s todoApplication -string", RECTANGLE_CONFIG_KEY);
  char                   *todo_app = stringfn_mut_trim(rectangle_run_cmd(cmd));
  struct StringFNStrings lines     = stringfn_split_lines_and_trim(todo_app);

  if (lines.count > 1) {
    todo_app = lines.strings[0];
  }

  return(todo_app);
}

bool rectangle_get_todo_mode_enabled(){
  char *cmd;

  asprintf(&cmd, "defaults read %s todoMode -bool", RECTANGLE_CONFIG_KEY);
  bool enabled = atoi(rectangle_run_cmd(cmd));

  return(enabled);
}

char *rectangle_get_config(){
  char *config = NULL;
  char *cmd;

  asprintf(&cmd, "defaults read %s", RECTANGLE_CONFIG_KEY);
  config = rectangle_run_cmd(cmd);
  return(config);
}

int rectangle_get_pid(){
  struct Vector *pids_v       = get_all_processes();
  int           rectangle_pid = -1;

  for (size_t i = 0; i < vector_size(pids_v); i++) {
    size_t pid = (size_t)vector_get(pids_v, i);
    if (pid < 2) {
      continue;
    }
    struct Vector *pid_cmdline_v = get_process_cmdline(pid);
    if (!pid_cmdline_v) {
      continue;
    }
    if (vector_size(pid_cmdline_v) > 0) {
      char *cmdline = str_flatten(vector_to_array(pid_cmdline_v), 0, vector_size(pid_cmdline_v));
      if (cmdline) {
        if (wildcardcmp("*/Rectangle.app/*", cmdline)) {
          rectangle_pid = pid;
        }
        free(cmdline);
      }
      vector_release(pid_cmdline_v);
    }
  }
  if (pids_v) {
    vector_release(pids_v);
  }
  return(rectangle_pid);
}

char *rectangle_run_cmd(char *CMD){
  char                 *READ_STDOUT;
  int                  exited, result;
  struct subprocess_s  subprocess;
  const char           *command_line[]                        = { "/bin/sh", "--norc", "--noprofile", "-c", CMD, NULL };
  char                 stdout_buffer[STDOUT_READ_BUFFER_SIZE] = { 0 };
  struct  StringBuffer *SB                                    = stringbuffer_new_with_options(STDOUT_READ_BUFFER_SIZE, true);
  size_t               bytes_read                             = 0,
                       index                                  = 0;

  result = subprocess_create(command_line, 0, &subprocess);
  assert_eq(result, 0, %d);
  do {
    bytes_read = subprocess_read_stdout(&subprocess, stdout_buffer, STDOUT_READ_BUFFER_SIZE - 1);
    stringbuffer_append_string(SB, stdout_buffer);
    index += bytes_read;
  } while (bytes_read != 0);

  result = subprocess_join(&subprocess, &exited);
  assert_eq(result, 0, %d);
  assert_eq(exited, 0, %d);
  READ_STDOUT = stringbuffer_to_string(SB);
  stringbuffer_release(SB);

  return(READ_STDOUT);
}

static void __attribute__((constructor)) __constructor__rectangle_test(){
}

static void __attribute__((destructor)) __destructor__rectangle_test(){
}
