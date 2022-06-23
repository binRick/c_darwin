#include <stdbool.h>
#define DEBUG_PID_ENV    false
//////////////////////////////////////////
#include "submodules/log.h/log.h"
#include "system-utils-test.h"
typedef struct {
  char *key;
  char *val;
} process_env_t;


//////////////////////////////////////////
static int get_argmax() {
  int    argmax;
  int    mib[] = { CTL_KERN, KERN_ARGMAX };
  size_t size  = sizeof(argmax);

  if (sysctl(mib, 2, &argmax, &size, NULL, 0) == 0) {
    return(argmax);
  }
  return(0);
}

static struct Vector *get_pid_env(int pid){
  struct Vector
                         *vector = vector_new(),
  *pid_env_v                     = vector_new();
  struct StringFNStrings EnvSplit;
  int                    env_res = -1, nargs;
  char                   *procenv = NULL, *procargs, *arg_ptr, *arg_end, *arg_start, *env_start;
  size_t                 argmax, size;

  pid = 7465;
  int
    mib_env[]    = { CTL_KERN, KERN_PROCARGS2, (pid_t)pid },
    mib_argmax[] = { CTL_KERN, KERN_ARGMAX }
  ;

  argmax   = get_argmax();
  size     = sizeof(argmax);
  procargs = (char *)malloc(argmax);
  env_res  = sysctl(mib_env, 3, procargs, &argmax, NULL, 0);
  arg_end  = &procargs[argmax];
  memcpy(&nargs, procargs, sizeof(nargs));
  arg_ptr = procargs + sizeof(nargs);
  arg_ptr = memchr(arg_ptr, '\0', arg_end - arg_ptr);

  for ( ; arg_ptr < arg_end; arg_ptr++) {
    if (*arg_ptr != '\0') {
      break;
    }
  }
  while (arg_ptr < arg_end && nargs > 0) {
    if (*arg_ptr++ == '\0') {
      nargs--;
    }
  }
  env_start = arg_ptr;
  procenv   = calloc(1, arg_end - arg_ptr);

  char *s;

  while (*arg_ptr != '\0' && arg_ptr < arg_end) {
    s = memchr(arg_ptr + 1, '\0', arg_end - arg_ptr);
    if (s == NULL) {
      break;
    }
    memcpy(procenv + (arg_ptr - env_start), arg_ptr, s - arg_ptr);
    arg_ptr = s + 1;
    if (strlen(arg_ptr) < 3) {
      continue;
    }
    EnvSplit = stringfn_split(arg_ptr, '=');
    if (EnvSplit.count > 1) {
      vector_push(vector, arg_ptr);
    }
  }

  for (size_t i = 0; i < vector_size(vector); i++) {
    EnvSplit = stringfn_split(vector_get(vector, i), '=');
    process_env_t *pe = malloc(sizeof(process_env_t));
    pe->key = strdup(EnvSplit.strings[0]);
    pe->val = strdup(stringfn_join(EnvSplit.strings, "=", 1, EnvSplit.count - 1));
    vector_push(pid_env_v, pe);
  }
  if (DEBUG_PID_ENV) {
    fprintf(stderr,
            "pid:%d\n"
            "argmax:%lu\n"
            "env_res:%d\n"
            "nargs:%d\n"
            "progargs:%s\n"
            "arg_ptr:%s\n"
            "arg_end:%s\n"
            "vectors:%lu\n"
            "process env vector len:%lu\n",
            pid,
            argmax,
            env_res,
            nargs,
            procargs,
            arg_ptr,
            arg_end,
            vector_size(vector),
            vector_size(pid_env_v)
            );
  }
  free(procargs);
  free(procenv);
  vector_release(vector);
  stringfn_release_strings_struct(EnvSplit);
  return(pid_env_v);
} /* get_pid_env */


TEST t_pid_env(void){
  int           pid        = getpid();
  struct Vector *pid_env_v = get_pid_env(pid);
  size_t        ENV_QTY    = vector_size(pid_env_v);

  ASSERT_GTE(ENV_QTY, 0);

  for (size_t i = 0; i < ENV_QTY; i++) {
    char *ENV_KEY = ((process_env_t *)vector_get(pid_env_v, i))->key,
         *ENV_VAL = ((process_env_t *)vector_get(pid_env_v, i))->val;
    ASSERT_GTE(strlen(ENV_KEY), 0);
    ASSERT_GTE(strlen(ENV_VAL), 0);
    dbg(ENV_KEY, %s);
    dbg(ENV_VAL, %s);
    free(ENV_KEY);
    free(ENV_VAL);
    free(((process_env_t *)vector_get(pid_env_v, i)));
  }
  dbg(ENV_QTY, %lu);
  vector_release(pid_env_v);
  PASS();
} /* t_pid_env */


//////////////////////////////////////////
TEST t_display_id(void){
  CGDirectDisplayID display_id = get_display_id(0);

  fprintf(stdout,
          "display id:%d\n",
          display_id
          );
  PASS();
}


TEST t_display_count(void){
  int display_count = get_display_count();

  fprintf(stdout,
          "display count:%d\n",
          display_count
          );
  PASS();
}


TEST t_display_size(void){
  CGDirectDisplayID              display_id = get_display_id(0);
  struct DarwinDisplayResolution *res       = get_display_resolution(display_id);

  fprintf(stdout,
          "x:%zu|y:%zu|pixels:%d|width:%fmm|height:%fmm|\n",
          res->x,
          res->y,
          res->pixels,
          res->size.width,
          res->size.height
          );
  PASS();
}

SUITE(s_display_count){
  RUN_TEST(t_display_count);
  PASS();
}

SUITE(s_display_id){
  RUN_TEST(t_display_id);
  PASS();
}


SUITE(s_pid_env){
  RUN_TEST(t_pid_env);
  PASS();
}

SUITE(s_display_size){
  RUN_TEST(t_display_size);
  PASS();
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_display_id);
  RUN_SUITE(s_display_size);
  RUN_SUITE(s_display_count);
  RUN_SUITE(s_pid_env);
  GREATEST_MAIN_END();
  return(0);
}
