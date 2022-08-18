#include "c_dbg/c_dbg.h"
#include "debug_print_h/include/debug_print.h"
#include "generic-print/print.h"
#include "ls-proc-args.h"
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
  long unsigned started                = timestamp();
  struct Vector *saved                 = vector_new();
  struct Vector *pids_v                = get_all_processes();
  size_t        PIDS_QTY               = vector_size(pids_v);

  for (size_t i = 0; i < PIDS_QTY; i++) {
    bool pid_match = false;
    int  pid       = (int)(long long)vector_get(pids_v, i);
    if (pid <= 1) {
      continue;
    }
    if (pid_match == false) {
      char          *pid_s;
      asprintf(&pid_s, "%d", pid);
      pid_match = match_item_v(ctx.pids, pid_s);
    }
    if (pid_match == false){
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
        int ppid = (int)(size_t)vector_get(ppids_v,i);
        char          *ppid_s;
        asprintf(&ppid_s, "%d", ppid);
        pid_match = match_item_v(ctx.ppids, ppid_s);
      }
      if(ppids_v)
        vector_release(ppids_v);
    }
    if (pid_match == false) {
      struct Vector *process_env_v     = get_process_env(pid);
      for (size_t ii = 0; ii < vector_size(process_env_v) && (pid_match == false); ii++) {
        process_env_t *process_env_key_val = (process_env_t *)(vector_get(process_env_v, ii));
        if (pid_match == false) {
          pid_match = match_item_v(ctx.env_keys, process_env_key_val->key);
        }
        if (pid_match == false) {
          pid_match = match_item_v(ctx.env_vals, process_env_key_val->val);
        }
      }
      if(process_env_v)
        vector_release(process_env_v);
    }
    if ((true == ctx.verbose) || (true == pid_match)) {
      PRINT(pid,
        "pid match:", pid_match?"Yes":"No"
        );
    }
  }
  return(EXIT_SUCCESS);
} /* ls_procs */

int main(int argc, char **argv) {
  parse_args(argc, argv);
  return(ls_procs());
}
/*
 * int _main(int argc, char **argv) {
 * user_supplied_keys = vector_new();
 * if (argc > 1) {
 *  char **tmp = argv;
 * tmp++;
 *  while (*tmp != NULL) {
 *    if (strcmp(*tmp, "-v") == 0) {
 *      verbose_mode = true;
 *      fprintf(stderr, ">Verbose Mode Enabled\n");
 *    }else{
 *      vector_push(user_supplied_keys, *tmp);
 *      if (verbose_mode) {
 *        fprintf(stderr, ">Added env key/value glob and pid match for '%s'\n", *tmp);
 *      }
 *    }
 * tmp++;
 *  }
 * }
 * return(ls_procs());
 * }
 */
