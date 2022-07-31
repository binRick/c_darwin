#pragma once
#define PROGRESS_BAR_WIDTH      40
#define BG_PROGRESS_BAR_CHAR    "."
#define PROGRESS_BAR_CHAR       "="
#include "ansi-codes/ansi-codes.h"
#ifdef _GNU_SOURCE
#undef _GNU_SOURCE
#endif
#include "progress.c/progress.h"


static void db_progress_start(progress_data_t *data) {
  assert(data);
  fprintf(stdout,
          AC_HIDE_CURSOR
          AC_RESETALL AC_GREEN AC_ITALIC "Processing" AC_RESETALL
          AC_RESETALL " "
          AC_RESETALL AC_BLUE AC_REVERSED AC_BOLD "%d"
          AC_RESETALL " "
          AC_RESETALL AC_GREEN AC_ITALIC "JSON Lines" AC_RESETALL
          "\n",
          data->holder->total
          );
  progress_write(data->holder);
}


static void db_progress(progress_data_t *data) {
  progress_write(data->holder);
}


static void db_progress_end(progress_data_t *data) {
  fprintf(stdout,
          AC_SHOW_CURSOR
          AC_RESETALL "\n"
          AC_GREEN AC_REVERSED AC_BOLD "Complete" AC_RESETALL
          "\n"
          );
}
