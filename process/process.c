#pragma once
#ifndef DEBUG_PID_ENV
#define DEBUG_PID_ENV    true
#endif
#include "process.h"
#include <libproc.h>
#include <sys/sysctl.h>
typedef void *rusage_info_t;
#define GET_PID(proc)       (proc)->kp_proc.p_pid
#define IS_RUNNING(proc)    (((proc)->kp_proc.p_stat & SRUN) != 0)
#define ERROR_CHECK(fun) \
  do {                   \
    if (fun) {           \
      goto ERROR;        \
    }                    \
  } while (0)


struct kinfo_proc *proc_list(size_t *count) {
  struct kinfo_proc *list = NULL;
  int               mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL };
  size_t            size  = 0;

  ERROR_CHECK(sysctl(mib, sizeof(mib) / sizeof(*mib), NULL, &size, NULL, 0));
  list = malloc(size);
  ERROR_CHECK(sysctl(mib, sizeof(mib) / sizeof(*mib), list, &size, NULL, 0));
  *count = size / sizeof(struct kinfo_proc);
  return(list);

ERROR:
  if (list) {
    free(list);
  }
  return(NULL);
}

struct Vector *connect_kitty_processes(struct Vector *KittyProcesses_v){
  const size_t           BUFSIZE                    = 1024;
  char                   *msg                       = "\x1bP@kitty-cmd{\"cmd\":\"ls\",\"version\":[0,25,2]}\x1b\\";
  struct Vector          *ConnectedKittyProcesses_v = vector_new();
  struct StringFNStrings ListenSplit;
  size_t                 recvd = 0, total_recvd = 0;

  for (int i = 0; i < vector_size(KittyProcesses_v); i++) {
    dbg(i, %d);
    kitty_process_t *KP = (kitty_process_t *)vector_get(KittyProcesses_v, i);
    for (int ii = 0; ii < vector_size(KP->env_v); ii++) {
      if (strcmp((char *)((process_env_t *)vector_get(KP->env_v, ii))->key, "KITTY_LISTEN_ON") == 0) {
        dbg((char *)((process_env_t *)vector_get(KP->env_v, ii))->val, %s);
        ListenSplit = stringfn_split(stringfn_trim((char *)((process_env_t *)vector_get(KP->env_v, ii))->val), ':');
        if (ListenSplit.count > 0 && strcmp(ListenSplit.strings[0], "tcp") == 0 && ListenSplit.count == 3) {
          dbg("CONNETING TO TCP!", %s);
          socket99_config cfg = { .host = strdup(ListenSplit.strings[1]), .port = atoi(ListenSplit.strings[2]), };
          socket99_result res;
          if (!socket99_open(&cfg, &res)) {
            continue;
          }
          size_t total_sent = 0;
          size_t msg_size   = strlen(msg);
          dbg(msg_size, %lu);
          do {
            size_t sent = send(res.fd, msg, msg_size, 0);
            dbg(sent, %lu);
            total_sent += sent;
          } while (total_sent < msg_size);
          dbg("SENT!", %s);
          dbg(total_sent, %lu);
          struct StringBuffer *SB = stringbuffer_new_with_options(BUFSIZE, true);
          do {
            char *buffer = calloc(BUFSIZE + 1, 1);
            dbg("RECEIVING!", %s);
            recvd = recv(res.fd, buffer, BUFSIZE - 32, 0);
            dbg(recvd, %lu);
            if (recvd < 1) {
              break;
            }
            buffer[recvd] = '\0';
            dbg("RECEIVED!", %s);
            total_recvd += recvd;
            stringbuffer_append_string(SB, buffer);
            free(buffer);
            if ((int)(buffer[strlen(buffer) - 1]) == 92) {
              if ((int)(buffer[strlen(buffer) - 2]) == 27) {
                dbg("END!", %s);
                close(res.fd);
                recvd = -1;
                break;
              }
            }
          }while (recvd > 0);
          close(res.fd);
          dbg(total_recvd, %lu);
          dbg("RECV DONE!", %s);
          char   *READ = stringbuffer_to_string(SB);
          size_t s     = strlen(READ);
          dbg("OK!", %s);
          dbg(s, %lu);
          fprintf(stderr, "%s\n", READ);
          stringbuffer_release(SB);
        }
      }
    }
  }
  return(ConnectedKittyProcesses_v);
} /* connect_kitty_processes */
struct Vector *get_kitty_processes(){
  const re_t    pattern = re_compile("[Kk]itty");
  int           match_length, match_idx;
  struct Vector *KittyProcesses_v = vector_new();
  struct Vector *pids_v           = get_all_processes();

  assert(pids_v != NULL);
  for (size_t i = 0; i < vector_size(pids_v); i++) {
    int           pid        = (int)(long long)vector_get(pids_v, i);
    struct Vector *cmdline_v = get_process_cmdline(pid);
    if (cmdline_v == NULL) {
      continue;
    }
    for (int i = 0; i < vector_size(cmdline_v); i++) {
      match_idx = re_matchp(pattern, (char *)vector_get(cmdline_v, i), &match_length);
      if (match_length < 1) {
        continue;
      }
      struct Vector *PE = get_process_env(pid);
      for (size_t ii = 0; ii < vector_size(PE); ii++) {
        process_env_t *E = (process_env_t *)(vector_get(PE, ii));
        if (strcmp(E->key, "KITTY_WINDOW_ID") == 0) {
          kitty_process_t *KP = malloc(sizeof(kitty_process_t));
          assert(KP != NULL);
          KP->window_id = atoi(E->val);
          assert(KP->window_id > 0);
          KP->pid   = (unsigned long)pid;
          KP->env_v = PE;
          assert(KP->pid > 0);
          KP->listen_on = strdup(E->key);
          vector_push(KittyProcesses_v, (void *)KP);
        }
      }
    }
  }
  return(KittyProcesses_v);
} /* get_kitty_processes */
  /*
   *  continue;
   *  socket99_config cfg = { .host = "127.0.0.1", .port = 25009, };
   *  socket99_result res;
   *  if (!socket99_open(&cfg, &res)) {
   *    continue;
   *  }
   *  const char *msg     = "\eP@kitty-cmd{\"cmd\":\"ls\",\"version\":[0,25,2]}\e\\";
   *  size_t     msg_size = strlen(msg);
   *  size_t     sent     = send(res.fd, msg, msg_size, 0);
   *  bool       pass     = ((size_t)sent == msg_size);
   *  assert(msg_size == sent);
   *  size_t     BUFSIZE = 1024 * 16;
   *  size_t     recvd = 0, total_recvd = 0;
   *  dbg("SENT!", %s);
   *  dbg(sent, %lu);
   *  struct StringBuffer *SB = stringbuffer_new_with_options(1024, true);
   *  char                buffer[BUFSIZE];
   *  do {
   *    dbg("RECEIVING!", %s);
   *    recvd         = recv(res.fd, buffer, BUFSIZE, 0);
   *    buffer[recvd] = '\0';
   *    stringbuffer_append_string(SB, buffer);
   *    dbg("RECEIVED!", %s);
   *    dbg(recvd, %lu);
   *    total_recvd += recvd;
   *  } while (recvd > 0);
   *  buffer[total_recvd] = '\0';
   *  close(res.fd);
   *  dbg("RECV DONE!", %s);
   *  char   *READ = stringbuffer_to_string(SB);
   *  size_t s     = strlen(READ);
   *  stringbuffer_release(SB);
   *  dbg(READ, %s);
   *  dbg("OK!", %s);
   *  dbg(s, %lu);
   * }
   * }
   * }
   *
   *
   * return(kitty_procs_v);
   * }
   */
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
    return(NULL);
  }

  arg_end = &procargs[argmax];
  memcpy(&nargs, procargs, sizeof(nargs));
  arg_ptr  = procargs + sizeof(nargs);
  len      = strlen(arg_ptr);
  arg_ptr += len + 1;
  if (arg_ptr == arg_end) {
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
  return(processes_v);
}

struct Vector *get_process_env(int process){
  struct Vector
  *vector        = vector_new(),
  *process_env_v = vector_new();

  if (process == 1) {
    return(process_env_v);
  }
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
