#include "c_dbg/c_dbg.h"
#include "debug_print_h/include/debug_print.h"
#include "generic-print/print.h"
#include "ls-proc.h"

bool match_item(char *MATCH_WILDCARD, char *ITEM){
  return(wildcardcmp(MATCH_WILDCARD, ITEM));
}

bool match_item_v(struct Vector *MATCH_WILDCARD_V, char *ITEM){
  if (vector_size(MATCH_WILDCARD_V) == 0) {
    return(false);
  }
  for (size_t ii = 0; ii < vector_size(MATCH_WILDCARD_V); ii++) {
    bool m = match_item((char *)vector_get(MATCH_WILDCARD_V, ii), ITEM);
    if (m == true) {
      return(true);
    }
  }

  return(false);
}

int ls_procs(){
  long unsigned started       = timestamp();
  struct Vector *matched_pids = vector_new();
  struct Vector *pids_v       = get_all_processes();
  size_t        PIDS_QTY      = vector_size(pids_v);

  for (size_t i = 0; i < PIDS_QTY; i++) {
    int  pid       = (int)(long long)vector_get(pids_v, i);
    bool pid_match = false;
    if (pid <= 1) {
      continue;
    }
    if (pid_match == false) {
      char *pid_s;
      asprintf(&pid_s, "%d", pid);
      pid_match = match_item_v(ctx.pids, pid_s);
      if (pid_s) {
        free(pid_s);
      }
    }
    if (pid_match == false) {
      struct Vector *pid_cmdline_v = get_process_cmdline(pid);
      char          *pid_cmdline   = FLATTEN_VECTOR(pid_cmdline_v);
      pid_match = match_item_v(ctx.cmd_lines, pid_cmdline);
    }
    if (pid_match == false) {
      pid_match = match_item_v(ctx.cwds, get_process_cwd(pid));
    }
    if (pid_match == false) {
      struct Vector *ppids_v = get_process_ppids(pid);
      for (size_t ii = 0; ii < vector_size(ppids_v) && (pid_match == false); ii++) {
        int  ppid = (int)(size_t)vector_get(ppids_v, i);
        char *ppid_s;
        asprintf(&ppid_s, "%d", ppid);
        pid_match = match_item_v(ctx.ppids, ppid_s);
        if (ppid_s) {
          free(ppid_s);
        }
      }
      if (ppids_v) {
        vector_release(ppids_v);
      }
    }
    if (pid_match == false) {
      struct Vector *process_env_v = get_process_env(pid);
      for (size_t ii = 0; ii < vector_size(process_env_v) && (pid_match == false); ii++) {
        process_env_t *process_env_key_val = (process_env_t *)(vector_get(process_env_v, ii));
        if (pid_match == false) {
          pid_match = match_item_v(ctx.env_keys, process_env_key_val->key);
        }
        if (pid_match == false) {
          pid_match = match_item_v(ctx.env_vals, process_env_key_val->val);
        }
      }
      if (process_env_v) {
        vector_release(process_env_v);
      }
    }
    if (pid_match == true) {
      vector_push(matched_pids, (void *)(size_t)pid);
    }
    if ((true == ctx.verbose) || (true == pid_match)) {
      PRINT(pid,
            "pid match:", pid_match?"Yes":"No"
            );
    }
  }
  char *msg;

  asprintf(&msg, "Matched %lu/%lu PIDs",
           vector_size(matched_pids),
           PIDS_QTY
           );
  fprintf(stdout, "%s\n", msg);
  if (msg) {
    free(msg);
  }
  return(EXIT_SUCCESS);
} /* ls_procs */

int main(int argc, char **argv) {
  parse_args(argc, argv);
  return(ls_procs());
}
