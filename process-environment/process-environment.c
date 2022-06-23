#pragma once
#include "process-environment.h"


static int get_argmax() {
  int    argmax;
  int    mib[] = { CTL_KERN, KERN_ARGMAX };
  size_t size  = sizeof(argmax);

  if (sysctl(mib, 2, &argmax, &size, NULL, 0) == 0) {
    return(argmax);
  }
  return(0);
}

struct Vector *get_process_environment(int pid){
  struct Vector
                         *vector = vector_new(),
  *pid_env_v                     = vector_new();
  struct StringFNStrings EnvSplit;
  int                    env_res = -1, nargs;
  char                   *procenv = NULL, *procargs, *arg_ptr, *arg_end, *arg_start, *env_start, *s;
  size_t                 argmax, size;

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
} /* get_process_environment */
