#include "kitty-ls.h"
#include "wildcardcmp/wildcardcmp.h"

int ls_kitty(){
  struct Vector *saved   = vector_new();
  struct Vector *pids_v  = get_all_processes();
  size_t        PIDS_QTY = vector_size(pids_v);

  for (size_t i = 0; i < PIDS_QTY; i++) {
    int pid = (int)(long long)vector_get(pids_v, i);
    if (pid <= 1) {
      continue;
    }
    struct Vector *PE = get_process_env(pid);
    for (size_t ii = 0; ii < vector_size(PE); ii++) {
      process_env_t *E = (process_env_t *)(vector_get(PE, ii));
      if (
        wildcardcmp("KITTY_LISTEN_ON", E->key)
        ||
        wildcardcmp("KITTY_TYPE", E->key)
        ||
        wildcardcmp("KITTY_PID", E->key)
        ) {
        char *result;
        bool found = false;
        asprintf(&result, "%s %s", E->key, E->val);
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
  vector_release(pids_v);
  for (size_t j = 0; j < vector_size(saved); j++) {
    char *msg = (char *)vector_get(saved, j);
    if (msg) {
      printf("%s\n", msg);
      free(msg);
    }
  }
  vector_release(saved);
  return(EXIT_SUCCESS);
} /* ls_kitty */

int main(int argc, char **argv) {
  return(ls_kitty());
}
