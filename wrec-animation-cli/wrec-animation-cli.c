#define OPTPARSE_HELP_USAGE_STYLE    1
////////////////////////////////////////////
#include "wrec-animation-cli/wrec-animation-cli.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log.h/log.h"
#include "optparse99/optparse99.h"
#define DEFAULT_PNG_DIR    "."
#define DEFAULT_GIF_DIR    "."

struct args_t {
  const char *png_directory, *gif_directory;
  bool       verbose;
};

static const struct args_t *args = &(struct args_t){
  .png_directory = DEFAULT_PNG_DIR,
  .gif_directory = DEFAULT_GIF_DIR,
  .verbose       = false,
};

void _command_exec(){
  log_debug("exec ");
}

void _command_animate(){
  log_debug("animating |png dir:%s|gif dir:%s|verbose:%d|",
            args->png_directory,
            args->gif_directory,
            args->verbose
            );
}

int main(int argc, char *argv[]) {
  struct optparse_cmd main_cmd = {
    .about       = "wrec-animation-cli v1.00 - Create animated gifs.",
    .description = "This program creates animated gifs.",
    .name        = "wrec-animation-cli",
    .operands    = "OPERAND [OPERAND...]",
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
    .subcommands           = (struct optparse_cmd[]) {
      {
        .description = "Print a subcommand's help information and quit.",
        .name        = "help",
        .operands    = "COMMAND",
        .function    = optparse_print_help_subcmd,
      },
      {
        .name        = "animate",
        .about       = "GIF Animation",
        .description = "Animate PNG Files to GIF",
        .function    = _command_animate,
        .options     = (struct optparse_opt[]) {
          { .short_name    = 'i',
            .long_name     = "input-directory",
            .description   = "Load PNG files from this directory.",
            .arg_name      = "PNG_DIRECTORY",
            .arg_data_type = DATA_TYPE_STR,
            .arg_dest      = &(args->png_directory), },
          { .short_name    = 'o',
            .long_name     = "output-directory",
            .description   = "Save Animated GIFs to this directory.",
            .arg_name      = "GIF_DIRECTORY",
            .arg_data_type = DATA_TYPE_STR,
            .arg_dest      = &(args->gif_directory), },
          /*
           * {
           * .short_name  = 's',
           * .long_name   = "start-timestamp",
           * .description = "Exclude images older then this timetamp.",
           * },
           * {
           * .short_name  = 'e',
           * .long_name   = "end-timestamp",
           * .description = "Exclude images newer then this timetamp.",
           * },
           * {
           * .short_name  = 'm',
           * .long_name   = "max-age-minutes",
           * .description = "Exclude images older then this quantity of minutes.",
           * },
           * {
           * .short_name  = 'h',
           * .long_name   = "max-age-hours",
           * .description = "Excludes images older then this quantity of hours.",
           * },
           */
          { END_OF_OPTIONS },
        }
      },
      {
        .name        = "exec",
        .about       = "about exec",
        .description = "yyyyyyyyyyyyyyyy",
        .usage       = "xxxxxxxxxxxxxxxx",
        .function    = _command_exec,
        .options     = (struct optparse_opt[]) {
          { END_OF_OPTIONS },
        },
      },
      { END_OF_SUBCOMMANDS },
    },
  };

  optparse_parse(&main_cmd, &argc, &argv);

  // Print remaining operands
  if (argc > 1) {
    for (int i = 0; i <= argc; i++) {
      printf("argv[%d]: %s\n", i, argv[i] ? argv[i] : "NULL");
    }
  } else {
//        optparse_print_help();
  }
} /* main */
