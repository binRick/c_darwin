#include "ls-win/ls-win.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "optparse99/optparse99.h"
#include "timestamp/timestamp.h"

////////////////////////////////////////////
struct args_t {
  bool verbose;
  bool current_space_only;
};
static const struct args_t *args = &(struct args_t){
  .verbose            = false,
  .current_space_only = false,
};

void _command_windows(){
  struct Vector *windows_v = get_windows();

  log_debug("listing %lu windows", vector_size(windows_v));
  struct list_window_table_t *list_options = &(struct list_window_table_t){
    .current_space_only = args->current_space_only,
  };

  list_windows_table((void *)list_options);
}

void _command_displays(){
  struct Vector *display_ids_v = get_display_ids_v();

  log_debug("listing %lu displays", vector_size(display_ids_v));
}

void _command_spaces(){
  struct Vector *space_ids_v = get_space_ids_v();

  log_debug("listing %lu spaces", vector_size(space_ids_v));
}

void debug_args(){
  log_info("Verbose:                 %s", args->verbose == true ? "Yes" : "No");
  log_info("Current Space Only:      %s", args->current_space_only == true ? "Yes" : "No");
}

int main(int argc, char **argv) {
  is_authorized_for_accessibility();
  struct optparse_cmd main_cmd = {
    .about       = "ls-win v1.00 - List Darwin Windows.",
    .description = "This program lists Darwin Windows.",
    .name        = "ls-win",
    .operands    = "[COMMAND...]",
    .options     = (struct optparse_opt[]) {
      {
        .short_name  = 'h',
        .long_name   = "help",
        .description = "Print help information and quit.",
        .function    = optparse_print_help,
      },
      {
        .short_name  = 'v',
        .long_name   = "verbose",
        .description = "Increase verbosity.",
        .flag_type   = FLAG_TYPE_SET_TRUE,
        .flag        = &(args->verbose),
      },
      { END_OF_OPTIONS },
    },
    .subcommands         = (struct optparse_cmd[]) {
      {
        .description = "Print a subcommand's help information and quit.",
        .name        = "help",
        .operands    = "COMMAND",
        .function    = optparse_print_help_subcmd,
      },
      {
        .name    = "displays",                                       .description  = "List Displays", .function = _command_displays,
        .about   = "List Darwin Displays",
        .options = (struct optparse_opt[]){
          {
            .short_name  = 'x',
            .long_name   = "xxx",
            .description = "xxx desc",
            .flag_type   = FLAG_TYPE_SET_TRUE,
            .flag        = &(args->current_space_only),
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name    = "windows",                                        .description  = "List Windows",  .function = _command_windows,
        .about   = "List Darwin Windows",
        .options = (struct optparse_opt[]){
          {
            .short_name  = 'c',
            .long_name   = "current-space",
            .description = "Windows in current space only",
            .flag_type   = FLAG_TYPE_SET_TRUE,
            .flag        = &(args->current_space_only),
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name    = "spaces",                                         .description  = "List Spaces",   .function = _command_spaces,
        .about   = "List Darwin Spaces",
        .options = (struct optparse_opt[]){
          {
            .short_name  = 'x',
            .long_name   = "xxx",
            .description = "xxx desc",
            .flag_type   = FLAG_TYPE_SET_TRUE,
            .flag        = &(args->current_space_only),
          },
          { END_OF_OPTIONS },
        },
      },
      { END_OF_SUBCOMMANDS },
    },
  };
  optparse_parse(&main_cmd, &argc, &argv);
  debug_args();
  return(0);
} /* main */
