#pragma once
#include "log/log.h"
#include "process.h"
#include <CoreFoundation/CoreFoundation.h>
#include <dlfcn.h>
#include <pthread.h>
////////////////////////
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <libproc.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
/////////////////////////
static bool PROCESS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__process(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_PROCESS") != NULL) {
    log_debug("Enabling Process Debug Mode");
    PROCESS_DEBUG_MODE = true;
  }
}
#define GET_PID(proc)       (proc)->kp_proc.p_pid
#define IS_RUNNING(proc)    (((proc)->kp_proc.p_stat & SRUN) != 0)
#define ERROR_CHECK(fun) \
  do {                   \
    if (fun) {           \
      goto ERROR;        \
    }                    \
  } while (0)

char *get_pid_cwd(pid_t pid){
  char                      *cwd = NULL;
  struct proc_vnodepathinfo vpi;

  proc_pidinfo(pid, PROC_PIDVNODEPATHINFO, 0, &vpi, sizeof(vpi));
  if (strlen(vpi.pvi_cdir.vip_path) > 0) {
    cwd = strdup(vpi.pvi_cdir.vip_path);
  }
  return(cwd);
}

char *pid_path(pid_t pid){
  char *pathbuf = malloc(PATH_MAX);

  proc_pidpath(pid, pathbuf, sizeof(pathbuf));
  return(pathbuf);
}

struct Vector *get_process_ppids(int pid){
  int           _pid = pid;
  struct Vector *v   = vector_new();

  while (true) {
    int _ppid = get_process_ppid(_pid);
    vector_push(v, (void *)(size_t)_ppid);
    if (_ppid == 1) {
      break;
    }
    _pid = _ppid;
  }

  return(v);
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
    return(-1);
  }
  return(0);
}

int get_process_ppid(int pid) {
  struct kinfo_proc kp;

  if (get_kinfo_proc((long)pid, &kp) == -1) {
    return(-1);
  }
  return((int)(long)kp.kp_eproc.e_ppid);
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

struct Vector *get_all_processes(){
  struct Vector *processes_v      = vector_new();
  pid_t         num               = proc_listallpids(NULL, 0);
  size_t        processes_len     = sizeof(pid_t) * num * 2;
  pid_t         *processes_buffer = malloc(processes_len);
  size_t        processes_qty     = proc_listallpids(processes_buffer, processes_len);

  for (size_t i = 0; i < processes_qty; i++) {
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
  char                   *procenv = NULL, *procargs, *arg_ptr, *arg_end, *env_start, *s;
  size_t                 argmax;

  argmax   = get_argmax();
  procargs = (char *)malloc(argmax);
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
  if (true == DEBUG_PID_ENV) {
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
  return(get_pid_cwd(getpid()));
}

struct Vector *get_child_pids(int PID){
  struct Vector *v      = vector_new();
  struct Vector *pids_v = get_all_processes();

  for (size_t i = 0; i < vector_size(pids_v); i++) {
    int pid = (int)(long long)vector_get(pids_v, i);
    if (pid < 2) {
      continue;
    }
    int ppid   = (int)(long)get_process_ppid(pid);
    int pppid  = (int)(long)get_process_ppid(ppid);
    int ppppid = (int)(long)get_process_ppid(pppid);
    if (ppid == PID || pppid == PID || ppppid == PID) {
      vector_push(v, (void *)(long)pid);
    }
  }
  return(v);
}

static bool darwin_pthread_setname_np(const char *name) {
  char namebuf[64];

  strncpy(namebuf, name, sizeof(namebuf) - 1);
  namebuf[sizeof(namebuf) - 1] = '\0';
  return(pthread_setname_np(namebuf) != 0);
}

typedef enum {
  kLSDefaultSessionID = -2,
} LSSessionID;
CFTypeRef LSGetCurrentApplicationASN(void);
OSStatus LSSetApplicationInformationItem(LSSessionID, CFTypeRef, CFStringRef, CFStringRef, CFDictionaryRef *);
CFDictionaryRef LSApplicationCheckIn(LSSessionID, CFDictionaryRef);
void LSSetApplicationLaunchServicesServerConnectionStatus(uint64_t, void *);

typedef struct {
  void        *application_services_handle;

  CFBundleRef launch_services_bundle;
  typeof(LSGetCurrentApplicationASN) * pLSGetCurrentApplicationASN;
  typeof(LSSetApplicationInformationItem) * pLSSetApplicationInformationItem;
  typeof(LSApplicationCheckIn) * pLSApplicationCheckIn;
  typeof(LSSetApplicationLaunchServicesServerConnectionStatus) * pLSSetApplicationLaunchServicesServerConnectionStatus;

  CFStringRef *display_name_key_ptr;
} launch_services_t;

static bool launch_services_init(launch_services_t *it) {
  enum {
    has_nothing,
    has_application_services_handle
  }    state = has_nothing;
  bool ret   = false;

  it->application_services_handle = dlopen("/System/Library/Frameworks/"
                                           "ApplicationServices.framework/"
                                           "Versions/Current/ApplicationServices",
                                           RTLD_LAZY | RTLD_LOCAL);
  if (!it->application_services_handle) {
    goto done;
  }
  ++state;

  it->launch_services_bundle = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.LaunchServices"));
  if (!it->launch_services_bundle) {
    goto done;
  }

#define LOAD_METHOD(name)                                                            \
  *(void **)(&it->p ## name) =                                                       \
    CFBundleGetFunctionPointerForName(it->launch_services_bundle, CFSTR("_" #name)); \
  if (!it->p ## name) goto done;

  LOAD_METHOD(LSGetCurrentApplicationASN)
  LOAD_METHOD(LSSetApplicationInformationItem)
  LOAD_METHOD(LSApplicationCheckIn)
  LOAD_METHOD(LSSetApplicationLaunchServicesServerConnectionStatus)

#undef LOAD_METHOD

  it->display_name_key_ptr =
    CFBundleGetDataPointerForName(it->launch_services_bundle, CFSTR("_kLSDisplayNameKey"));
  if (!it->display_name_key_ptr || !*it->display_name_key_ptr) {
    goto done;
  }

  ret = true;

done:
  switch (state) {
  case has_application_services_handle: if (!ret) {
      dlclose(it->application_services_handle);
  }
  case has_nothing:;
  }
  return(ret);
} /* launch_services_init */

static inline void launch_services_destroy(launch_services_t *it) {
  dlclose(it->application_services_handle);
}

static bool launch_services_set_process_title(const launch_services_t *it, const char *title) {
  enum {
    has_nothing,
    has_cf_title
  }           state = has_nothing;
  bool        ret   = false;

  static bool checked_in = false;

  CFTypeRef   asn;
  CFStringRef cf_title;

  if (!checked_in) {
    it->pLSSetApplicationLaunchServicesServerConnectionStatus(0, NULL);
    it->pLSApplicationCheckIn(kLSDefaultSessionID, CFBundleGetInfoDictionary(CFBundleGetMainBundle()));
    checked_in = true;
  }

  asn = it->pLSGetCurrentApplicationASN();
  if (!asn) {
    goto done;
  }

  cf_title = CFStringCreateWithCString(NULL, title, kCFStringEncodingUTF8);
  if (!cf_title) {
    goto done;
  }
  ++state;
  if (it->pLSSetApplicationInformationItem(kLSDefaultSessionID,
                                           asn,
                                           *it->display_name_key_ptr,
                                           cf_title,
                                           NULL) != noErr) {
    goto done;
  }
  ret = true;
done:
  switch (state) {
  case has_cf_title: CFRelease(cf_title);
  case has_nothing:;
  }

  return(ret);
} /* launch_services_set_process_title */

bool darwin_set_process_title(const char *title) {
  enum {
    has_nothing,
    has_launch_services
  }                 state = has_nothing;
  bool              ret   = false;
  launch_services_t launch_services;
  if (!launch_services_init(&launch_services)) {
    goto done;
  }
  ++state;
  if (!launch_services_set_process_title(&launch_services, title)) {
    goto done;
  }
  (void)darwin_pthread_setname_np(title);
  ret = true;
done:
  switch (state) {
  case has_launch_services: launch_services_destroy(&launch_services);
  case has_nothing:;
  }
  return(ret);
}
