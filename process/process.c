#pragma once
//#include "debug-memory/debug_memory.h"
#include "process.h"
#include <ctype.h>
#include <libproc.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/time.h>


#include <assert.h>
#include <dirent.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>


#define GET_PID(proc)       (proc)->kp_proc.p_pid
#define IS_RUNNING(proc)    (((proc)->kp_proc.p_stat & SRUN) != 0)
#define ERROR_CHECK(fun) \
  do {                   \
    if (fun) {           \
      goto ERROR;        \
    }                    \
  } while (0)


char *pid_path(pid_t pid){
  char *pathbuf = malloc(PATH_MAX);

  proc_pidpath(pid, pathbuf, sizeof(pathbuf));
  return(pathbuf);
}


int get_kinfo_proc(pid_t pid, struct kinfo_proc *kp) {
  int    mib[4];
  size_t len;

  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PID;
  mib[3] = pid;

  len = sizeof(struct kinfo_proc);
  if (sysctl(mib, 4, kp, &len, NULL, 0) == -1) {
    return(-1);
  }

  if (len == 0) {
    fprintf(stderr, "sysctl(kinfo_proc), len == 0\n");
    return(-1);
  }
  return(0);
}


static long get_process_ppid(unsigned long pid) {
  struct kinfo_proc kp;

  if (get_kinfo_proc(pid, &kp) == -1) {
    return(-1);
  }
  return((long)kp.kp_eproc.e_ppid);
}


struct kinfo_proc *proc_info_for_pid(pid_t pid) {
  struct kinfo_proc *list = NULL;
  int               mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, pid };
  size_t            size  = 0;

  ERROR_CHECK(sysctl(mib, sizeof(mib) / sizeof(*mib), NULL, &size, NULL, 0));
  list = malloc(size);
  ERROR_CHECK(sysctl(mib, sizeof(mib) / sizeof(*mib), list, &size, NULL, 0));
  return(list);

ERROR:
  if (list) {
    free(list);
  }
  return(NULL);
}


static int get_argmax() {
  int    argmax, mib[] = { CTL_KERN, KERN_ARGMAX };
  size_t size = sizeof(argmax);

  if (sysctl(mib, 2, &argmax, &size, NULL, 0) == 0) {
    return(argmax);
  }
  return(0);
}


struct Vector *get_process_cmdline(int process){
  struct Vector *cmdline_v = vector_new();
  int           mib[3], nargs;
  char          *procargs = NULL, *arg_ptr, *arg_end, *curr_arg;
  size_t        len, argmax;

  if (process < 1) {
    return(NULL);
  }

  argmax = get_argmax();
  if (!argmax) {
    return(NULL);
  }
  procargs = (char *)malloc(argmax);
  if (procargs == NULL) {
    return(NULL);
  }

  mib[0] = CTL_KERN;
  mib[1] = KERN_PROCARGS2;
  mib[2] = (pid_t)process;
  if (sysctl(mib, 3, procargs, &argmax, NULL, 0) < 0) {
    free(procargs);
    return(NULL);
  }

  arg_end = &procargs[argmax];
  memcpy(&nargs, procargs, sizeof(nargs));
  arg_ptr  = procargs + sizeof(nargs);
  len      = strlen(arg_ptr);
  arg_ptr += len + 1;
  if (arg_ptr == arg_end) {
    free(procargs);
    return(NULL);
  }

  for ( ; arg_ptr < arg_end; arg_ptr++) {
    if (*arg_ptr != '\0') {
      break;
    }
  }

  curr_arg = arg_ptr;
  while (arg_ptr < arg_end && nargs > 0) {
    if (*arg_ptr++ == '\0') {
      vector_push(cmdline_v, curr_arg);
      curr_arg = arg_ptr;
      nargs--;
    }
  }
  if (procargs) {
    free(procargs);
  }
  return(cmdline_v);
} /* get_process_cmdline */


char *get_process_cwd(int process){
  if (process < 0) {
    return(NULL);
  }
  struct proc_vnodepathinfo vpi;
  int                       ret = proc_pidinfo(process, PROC_PIDVNODEPATHINFO, 0, &vpi, sizeof(vpi));
  if (ret < 0) {
    return(NULL);
  }
  char *cwd = strdup(vpi.pvi_cdir.vip_path);
  return(cwd);
}

struct Vector *get_all_processes(){
  struct Vector *processes_v      = vector_new();
  pid_t         num               = proc_listallpids(NULL, 0);
  size_t        processes_len     = sizeof(pid_t) * num * 2;
  pid_t         *processes_buffer = malloc(processes_len);
  size_t        processes_qty     = proc_listallpids(processes_buffer, processes_len);

  for (pid_t i = 0; i < processes_qty; i++) {
    if ((long long)processes_buffer[i] > 0) {
      vector_push(processes_v, (void *)(long long)processes_buffer[i]);
    }
  }
  free(processes_buffer);
  return(processes_v);
}

struct Vector *get_process_env(int process){
  struct Vector *process_env_v = vector_new();

  if (process == 1) {
    return(process_env_v);
  }
  struct Vector          *vector = vector_new();
  struct StringFNStrings EnvSplit;
  int                    env_res = -1, nargs;
  char                   *procenv = NULL, *procargs, *arg_ptr, *arg_end, *arg_start, *env_start, *s;
  size_t                 argmax, size;

  int
    mib_env[]    = { CTL_KERN, KERN_PROCARGS2, (pid_t)process },
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
    if (false) {
      fprintf(stderr,
              AC_RESETALL AC_BLUE "%s=>%s\n" AC_RESETALL, pe->key, pe->val);
    }
    vector_push(process_env_v, pe);
  }
  if (DEBUG_PID_ENV) {
    fprintf(stderr,
            "process:%d\n"
            "argmax:%lu\n"
            "env_res:%d\n"
            "nargs:%d\n"
            "progargs:%s\n"
            "arg_ptr:%s\n"
            "arg_end:%s\n"
            "vectors:%lu\n"
            "process env vector len:%lu\n",
            process,
            argmax,
            env_res,
            nargs,
            procargs,
            arg_ptr,
            arg_end,
            vector_size(vector),
            vector_size(process_env_v)
            );
  }
  if (procargs != NULL) {
    free(procargs);
  }
  if (procenv != NULL) {
    free(procenv);
  }
  if (vector_size(vector) > 0) {
    stringfn_release_strings_struct(EnvSplit);
  }
  vector_release(vector);
  return(process_env_v);
} /* get_process_env */


char *get_my_cwd(){
  struct stat   Odir;
  struct dirent *d_stat;

  long int      inode, pinode;
  DIR           *cur_dir;
  char          nameofdir[225];

  while (inode > 2) {
    lstat(".", &Odir);
    inode = Odir.st_ino;
    chdir("..");
    cur_dir = opendir(".");
    printf("get_my_cwd> DIR\n");

    while ((d_stat = readdir(cur_dir)) != NULL) {
      pinode = d_stat->d_ino;
      if (pinode == inode) {
        strcpy(nameofdir, d_stat->d_name);
        if (!strcmp(nameofdir, ".")) {
          printf("home\n");
        }else if (!strcmp(nameofdir, "..")) {
          printf("root\n");
        }else{
          printf("%s\n", nameofdir);
        }
      }
    }
    closedir(cur_dir);
  }
  return(0);
}
