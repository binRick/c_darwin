#pragma once
#ifndef ALACRITTY_C
#define ALACRITTY_C
////////////////////////////////////////////
///Applications/Alacritty.app/Contents/MacOS/alacritty msg config font.normal.family=InconsolataGo\ Nerd\ Font font.normal.style=Regular font.size=30
////////////////////////////////////////////
#include "alacritty/alacritty.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "process/process.h"
#include "timestamp/timestamp.h"
#include "wildcardcmp/wildcardcmp.h"
////////////////////////////////////////////
static bool ALACRITTY_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__alacritty(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_ALACRITTY") != NULL) {
    log_debug("Enabling alacritty Debug Mode");
    ALACRITTY_DEBUG_MODE = true;
  }
}

static bool vector_contains_pid(struct Vector *pids_v, int pid){
  for (size_t i = 0; i < vector_size(pids_v); i++) {
    if ((size_t)pid == (size_t)vector_get(pids_v, i)) {
      return(true);
    }
  }
  return(false);
}

int get_pid_alacritty(int pid){
  struct Vector *PE = get_process_env(pid);

  for (size_t ii = 0; ii < vector_size(PE); ii++) {
    process_env_t *E = (process_env_t *)(vector_get(PE, ii));
    if (strcmp(E->key, "ALACRITTY_LOG") == 0) {
      struct     StringFNStrings slash_split = stringfn_split(E->val, '/');
      if (slash_split.count > 0) {
        struct     StringFNStrings dash_split = stringfn_split(slash_split.strings[slash_split.count - 1], '-');
        if (dash_split.count > 0) {
          struct     StringFNStrings dot_split = stringfn_split(dash_split.strings[dash_split.count - 1], '.');
          if (dot_split.count > 0) {
            int apid = atoi(dot_split.strings[0]);
            stringfn_release_strings_struct(slash_split);
            stringfn_release_strings_struct(dash_split);
            stringfn_release_strings_struct(dot_split);
            vector_release(PE);
            return(apid);
          }
          stringfn_release_strings_struct(dot_split);
        }
        stringfn_release_strings_struct(dash_split);
      }
      stringfn_release_strings_struct(slash_split);
    }
  }
  vector_release(PE);
  return(-1);
}

struct Vector *get_alacritty_pids(){
  struct Vector *pids_v           = get_all_processes();
  struct Vector *alacritty_pids_v = vector_new();

  for (size_t i = 0; i < vector_size(pids_v); i++) {
    int pid = (int)(long long)vector_get(pids_v, i);
    if (vector_contains_pid(alacritty_pids_v, pid) == true) {
      continue;
    }
    get_process_cmdline(pid);
    int kp = get_pid_alacritty(pid);
    if (kp > 1 && (vector_contains_pid(alacritty_pids_v, kp) == false)) {
      vector_push(alacritty_pids_v, (void *)(size_t)kp);
    }
  }
  return(alacritty_pids_v);
}

#endif
