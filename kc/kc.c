#include "kc/kc.h"
#include "kitty/msg/msg.h"
struct list_args_t {
  bool all_items, enabled_items;
};
struct args_t {
  bool               verbose_mode, debug_mode;
  char               *file;
  struct list_args_t *list;
};
static struct args_t *args = &(struct args_t){
  .verbose_mode = false,
  .debug_mode   = false,
  .list         = &(struct list_args_t){
    .all_items     = false,
    .enabled_items = false,
  },
};

static void _command_cat(int argc, char **argv){
  if (argc == 1) {
    fprintf(stderr, AC_RED "No file path arguments provided\n"AC_RESETALL);
    optparse_print_help_subcmd(argc, argv);
    exit(EXIT_FAILURE);
  }
  for (size_t i = 1; i < argc; i++)
    if (fsio_file_exists(argv[i]))
      if (kitty_display_image_path(argv[i]))
        printf("\n");

  exit(EXIT_SUCCESS);
}

static void _command_list(){
  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Debug Arguments" AC_RESETALL
    AC_CYAN    "\n\t Verbose Mode   :    %s" AC_RESETALL
    AC_MAGENTA "\n\t Debug   Mode   :    %s" AC_RESETALL
    AC_RED     "\n\t All     Items  :    %s" AC_RESETALL
    AC_GREEN   "\n\t Enabled Items  :    %s" AC_RESETALL
    "\n%s",
    args->debug_mode == true ? AC_GREEN "Yes" : AC_RED "No",
    args->verbose_mode == true ? AC_GREEN "Yes" : AC_RED "No",
    args->list->all_items == true ? AC_GREEN "Yes" : AC_RED "No",
    args->list->enabled_items == true ? AC_GREEN "Yes" : AC_RED "No",
    ""
    );
  exit(EXIT_SUCCESS);
}

static void _command_debug_args(){
  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Debug Arguments" AC_RESETALL
    AC_CYAN    "\n\t Verbose Mode   :    %s" AC_RESETALL
    AC_MAGENTA "\n\t Debug   Mode   :    %s" AC_RESETALL
    "\n%s",
    args->debug_mode == true ? AC_GREEN "Yes" : AC_RED "No",
    args->verbose_mode == true ? AC_GREEN "Yes" : AC_RED "No",
    ""
    );
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  struct optparse_cmd main_cmd = {
    .about       = "kc v1.00 - CLI Tool",
    .description = "This program does something useful",
    .name        = "kc",
    .operands    = "[COMMAND...]",
    .options     = (struct optparse_opt[]) {
      {
        .short_name  = 'h',
        .long_name   = "help",
        .description = "Print help information and quit",
        .function    = optparse_print_help,
      },
      {
        .short_name  = 'v',
        .long_name   = "verbose",
        .description = "Increase verbosity",
        .flag_type   = FLAG_TYPE_SET_TRUE,
        .flag        = &(args->verbose_mode),
      },
      {
        .short_name  = 'd',
        .long_name   = "debug",
        .description = "Debug Mode",
        .flag_type   = FLAG_TYPE_SET_TRUE,
        .flag        = &(args->debug_mode),
      },
      { END_OF_OPTIONS },
    },
    .subcommands         = (struct optparse_cmd[]) {
      {
        .description = "Print a subcommand's help information and quit.",
        .name        = "help",
        .operands    = "COMMAND",
        .about       = "Command Help",
        .function    = optparse_print_help_subcmd,
      },
      {
        .name        = "cat",
        .description = "Cat File(s)",
        .function    = _command_cat,
        .about       = "Cat File(s)",
        .options     = (struct optparse_opt[]){
          {
            .short_name  = 'a',
            .long_name   = "all",
            .description = "All Items",
            .flag_type   = FLAG_TYPE_SET_TRUE,
            .flag        = &(args->list->all_items),
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "list",
        .description = "List",
        .function    = _command_list,
        .about       = "List Items",
        .options     = (struct optparse_opt[]){
          {
            .short_name  = 'e',
            .long_name   = "enabled",
            .description = "Enabled Items",
            .flag_type   = FLAG_TYPE_SET_TRUE,
            .flag        = &(args->list->enabled_items),
          },
          {
            .short_name  = 'a',
            .long_name   = "all",
            .description = "All Items",
            .flag_type   = FLAG_TYPE_SET_TRUE,
            .flag        = &(args->list->all_items),
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "args",
        .description = "Debug Arguments",
        .function    = _command_debug_args,
        .about       = "Debug Arguments",
      },
      { END_OF_SUBCOMMANDS },
    },
  };

  optparse_parse(&main_cmd, &argc, &argv);
  optparse_print_help();
  return(EXIT_FAILURE);
} /* main */
