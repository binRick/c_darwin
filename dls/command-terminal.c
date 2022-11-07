#ifndef DLS_COMMAND_TERMINAL_C
#define DLS_COMMAND_TERMINAL_C
#include "dls/command-terminal.h"
#include "kitty/kitty.h"

int _command_terminal_kitty_pids(){
  struct Vector  *v = get_kittys();
  struct kitty_t *k;

  for (size_t i = 0; i < vector_size(v); i++) {
    k = (struct kitty_t *)vector_get(v, i);
    log_info(
      "%s, %lu, %d, %s, %s, %lu env"
      ", %ld, age: %s, "
      "hash: %s, "
      "kitty window id: %d, "
      "kitty_config_directory: %s, "
      "kitty_launched_by_launch_services: %d, "
      "kitty_listen_on: %s, "
      "pids: %lu, ppids: %lu, "
      "%s",
      bytes_to_string(k->binary_size),
      k->window_id, k->pid, k->cmdline, k->cwd, vector_size(k->env_v),
      k->binary_ts,
      milliseconds_to_string(timestamp() - k->binary_ts * 1000),
      k->binary_hash,
      k->kitty_window_id,
      k->kitty_config_directory,
      k->kitty_launched_by_launch_services,
      k->kitty_listen_on,
      vector_size(k->pids_v), vector_size(k->ppids_v),
      ""
      );
  }
  log_info(">term kitty pids: %lu", vector_size(v));
  vector_release(v);
  exit(EXIT_SUCCESS);
}

int _command_terminal_kitty(){
  log_info("term kitty");
  exit(EXIT_SUCCESS);
}
#endif
