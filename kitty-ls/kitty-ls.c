#include "kitty-ls.h"
#include "timestamp/timestamp.h"
#include "wildcardcmp/wildcardcmp.h"
struct Vector *user_supplied_keys;
bool          verbose_mode = false;

int ls_kitty(){
  long unsigned started                = timestamp();
  struct Vector *saved                 = vector_new();
  struct Vector *pids_v                = get_all_processes();
  size_t        searched_env_vars      = 0;
  size_t        searched_env_var_bytes = 0;
  size_t        PIDS_QTY               = vector_size(pids_v);

  for (size_t i = 0; i < PIDS_QTY; i++) {
    int pid = (int)(long long)vector_get(pids_v, i);
    if (pid <= 1) {
      continue;
    }
    struct Vector *PE = get_process_env(pid);
    searched_env_vars += vector_size(PE);
    for (size_t ii = 0; ii < vector_size(PE); ii++) {
      process_env_t *E = (process_env_t *)(vector_get(PE, ii));
      searched_env_var_bytes += strlen(E->key) + strlen(E->val);
      bool          match = false;
      if (
        wildcardcmp("KITTY_LISTEN_ON", E->key)
        ||
        wildcardcmp("KITTY_TYPE", E->key)
        ||
        wildcardcmp("KITTY_PID", E->key)
        ) {
        match = true;
      }
      if (match == false) {
        for (size_t u = 0; u < vector_size(user_supplied_keys); u++) {
          if (wildcardcmp((char *)vector_get(user_supplied_keys, u), E->key)) {
            match = true;
          }
          if (wildcardcmp((char *)vector_get(user_supplied_keys, u), E->key)) {
            match = true;
          }
          if (pid == atoi((char *)vector_get(user_supplied_keys, u))) {
            match = true;
          }
        }
      }
      if (match == true) {
        char *result;
        bool found = false;
        asprintf(&result, "%d %s %s", pid, E->key, E->val);
        for (size_t j = 0; j < vector_size(saved); j++) {
          if (strcmp(result, (char *)vector_get(saved, j)) == 0) {
            found = true;
          }
        }
        if (found == false) {
          vector_push(saved, (void *)result);
        }
      }
    }
    vector_release(PE);
  }
  for (size_t j = 0; j < vector_size(saved); j++) {
    char *msg = (char *)vector_get(saved, j);
    if (msg) {
      printf("%s\n", msg);
      free(msg);
    }
  }
  long unsigned dur_ms = timestamp() - started;

  if (verbose_mode) {
    fprintf(stderr, "Acquired %lu results from %lu env vars, %lu pids, and %lu bytes of env vars in %ldms using %lu user supplied search items\n", vector_size(saved), searched_env_vars, vector_size(pids_v), searched_env_var_bytes, dur_ms, vector_size(user_supplied_keys));
  }
  vector_release(saved);
  vector_release(pids_v);
  vector_release(user_supplied_keys);
  return(EXIT_SUCCESS);
} /* ls_kitty */

int main(int argc, char **argv) {
  user_supplied_keys = vector_new();
  if (argc > 1) {
    char **tmp = argv;
    while (*tmp != NULL) {
      if (strcmp(*tmp, "-v") == 0) {
        verbose_mode = true;
        fprintf(stderr, ">Verbose Mode Enabled\n");
      }else{
        vector_push(user_supplied_keys, *tmp);
        if (verbose_mode) {
          fprintf(stderr, ">Added env key/value glob and pid match for '%s'\n", *tmp);
        }
      }
      *tmp++;
    }
  }
  return(ls_kitty());
}
