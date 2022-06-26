#pragma once
#include "dbg.h"
#include "process.h"
#include <libproc.h>
#include <sys/sysctl.h>
const char   KITTY_MSG[] = "\x1bP@kitty-cmd{\"cmd\":\"ls\",\"version\":[0,25,2]}\x1b\\";
const size_t BUFSIZE     = 8192;
#define GET_PID(proc)       (proc)->kp_proc.p_pid
#define IS_RUNNING(proc)    (((proc)->kp_proc.p_stat & SRUN) != 0)
#define ERROR_CHECK(fun) \
  do {                   \
    if (fun) {           \
      goto ERROR;        \
    }                    \
  } while (0)


void connect_kitty_port(int PORT){
  socket99_config cfg = { .host = "127.0.0.1", .port = PORT };
  socket99_result res;

  if (!socket99_open(&cfg, &res)) {
    return;
  }
  struct StringBuffer *SB = stringbuffer_new_with_options(BUFSIZE, true);
  size_t              total_recvd = 0, total_sent = 0, msg_size = strlen(KITTY_MSG), recvd = 0;

  do {
    total_sent += send(res.fd, KITTY_MSG, strlen(KITTY_MSG), 0);
  } while (total_sent < msg_size);
  do {
    char *buf = calloc(BUFSIZE + 1, 1);
    recvd = 0;
    recvd = recv(res.fd, buf, BUFSIZE, 0);
    if (recvd < 1) {
      break;
    }
    buf[recvd]   = '\0';
    total_recvd += recvd;
    stringbuffer_append_string(SB, buf);
    free(buf);
    if ((int)(buf[strlen(buf) - 1]) == 92) {
      if ((int)(buf[strlen(buf) - 2]) == 27) {
        recvd = -1;
        close(res.fd);
        break;
      }
    }
  }while (recvd > 0);
  close(res.fd);
  char *Nb = calloc(stringbuffer_get_content_size(SB) + 1, 1);

  strncpy(Nb, stringbuffer_to_string(SB) + 12, stringbuffer_get_content_size(SB) - 3);
  stringbuffer_release(SB);
  JSON_Value  *V  = json_parse_string(Nb);
  JSON_Object *O0 = json_value_get_object(V);

  assert(json_object_get_boolean(O0, "ok") == 1);
  JSON_Value *S0V = json_parse_string(json_object_get_string(O0, "data"));
  JSON_Array *A0  = json_value_get_array(S0V);

  assert(json_array_get_count(A0) > 0);
  assert(json_type(json_array_get_value(A0, 0)) == 4);
  JSON_Object *V101 = json_value_get_object(json_array_get_value(A0, 0));

  assert(json_object_get_count(V101) > 0);
  JSON_Value *V_WM_NAME = json_object_get_value(V101, "wm_name");
  JSON_Value *V_ID      = json_object_get_value(V101, "id");

  assert(json_type(V_WM_NAME) == 2);
  assert(json_type(V_ID) == 3);
  assert(json_value_get_string(V_WM_NAME) != NULL);
  assert(json_value_get_number(V_ID) > 0);
  JSON_Value *V_IS_FOCUSED = json_object_get_value(V101, "is_focused");

  assert(json_type(V_IS_FOCUSED) == 6);
  bool       IS_FOCUSED = json_value_get_boolean(V_IS_FOCUSED);
  JSON_Value *V_TABS    = json_object_get_value(V101, "tabs");
  JSON_Array *TABS      = json_value_get_array(V_TABS);

  assert(json_array_get_count(TABS) > 0);
  for (int ii = 0; ii < json_array_get_count(TABS); ii++) {
    JSON_Value  *TABv = json_array_get_value(TABS, ii);
    assert(json_type(TABv) == 4);
    JSON_Object *TAB = json_value_get_object(TABv);
    assert(json_object_get_count(TAB) == 9);
    assert(json_object_has_value_of_type(TAB, "id", JSONNumber));
    assert(json_object_has_value_of_type(TAB, "is_focused", JSONBoolean));
    assert(json_object_has_value_of_type(TAB, "title", JSONString));
    assert(json_object_has_value_of_type(TAB, "layout", JSONString));
    assert(json_object_has_value_of_type(TAB, "layout_state", JSONObject));
    assert(json_object_has_value_of_type(TAB, "layout_opts", JSONObject));
    assert(json_object_has_value_of_type(TAB, "windows", JSONArray));
    assert(json_object_has_value_of_type(TAB, "enabled_layouts", JSONArray));
    assert(json_object_has_value_of_type(TAB, "active_window_history", JSONArray));
    dbg(json_object_get_string(TAB, "title"), %s);
    dbg(json_object_get_boolean(TAB, "is_focused"), %d);
    dbg(json_object_get_string(TAB, "layout"), %s);
  }
} /* connect_kitty_port */

struct Vector *connect_kitty_processes(struct Vector *KittyProcesses_v){
  struct Vector *ConnectedKittyProcesses_v = vector_new();

  return(ConnectedKittyProcesses_v);
} /* connect_kitty_processes */


struct Vector *get_kitty_processes(){
  struct Vector *KittyProcesses_v = vector_new();
  const re_t    pattern = re_compile("[Kk]itty");
  int           match_length, match_idx;
  struct Vector *pids_v = get_all_processes();

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
  fprintf(stdout,
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
  if (DEBUG_PID_ENV) {
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
