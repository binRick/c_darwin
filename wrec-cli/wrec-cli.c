////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "wrec-cli/wrec-cli.h"
#include "wrec/wrec.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


////////////////////////////////////////////////////

static struct args_t args = {
  DEFAULT_MODE,
  DEFAULT_VERBOSE,
  DEFAULT_WINDOW_ID,
};


static struct modes_t    modes[] = {
  { .name = "debug_args",     .description = "Debug Arguments", .handler = debug_args     },
  { .name = "wrec0",          .description = "Wrec0",           .handler = wrec0          },
  { .name = "list",           .description = "List Windows",    .handler = list_windows   },
  { .name = "capture_window", .description = "Capture Window",  .handler = capture_window },
  { NULL },
};

static struct cag_option options[] = {
  { .identifier     = 'm',
    .access_letters = "m",
    .access_name    = "mode",
    .value_name     = "MODE",
    .description    = "Mode" },
  { .identifier     = 'v',
    .access_letters = "v",
    .access_name    = "verbose",
    .value_name     = NULL,
    .description    = "Verbose Mode" },
  { .identifier     = 'w',
    .access_letters = "w",
    .access_name    = "window",
    .value_name     = "WINDOW_ID",
    .description    = "Window ID" },
  { .identifier     = 'h',
    .access_letters = "h",
    .access_name    = "help",
    .description    = "Shows the command help" }
};
////////////////////////////////////////////////////


static int iterate_modes(){
  struct modes_t *m = modes;

  while (m->name != NULL) {
    if ((strcmp(args.mode, m->name) == 0)) {
      if (args.verbose) {
        fprintf(stderr, AC_GREEN "Running mode: '%s'" AC_RESETALL "\n", m->name);
      }
      return(m->handler((void *)&args));
    }
    m++;
  }
  printf(AC_RESETALL AC_RED "Mode \"%s\" not found.\n" AC_RESETALL, args.mode);
  return(1);
}


int main(int argc, char **argv) {
  parse_args(argc, argv);
  if ((argc >= 2) && (strcmp(argv[1], "--test") == 0)) {
    printf("Test OK\n"); return(0);
  }

  return(iterate_modes());
} /* main */


int debug_args(){
  fprintf(stderr,
          acs(AC_BRIGHT_BLUE_BLACK AC_ITALIC  "Verbose: %d") "\n"
          ansistr(AC_RESETALL AC_BRIGHT_GREEN_BLACK "Mode: %s") "\n"
          acs(AC_BRIGHT_MAGENTA_BLACK AC_ITALIC  "Window ID: %d") "\n"
          ,
          args.verbose,
          args.mode,
          args.window_id
          );

  return(EXIT_SUCCESS);
}


static int parse_args(int argc, char *argv[]){
  char               identifier;
  const char         *value;
  cag_option_context context;

  cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&context)) {
    identifier = cag_option_get(&context);
    switch (identifier) {
    case 'm':
      value     = cag_option_get_value(&context);
      args.mode = value;
      break;
    case 'v':
      args.verbose = true;
      break;
    case 'w':
      value          = cag_option_get_value(&context);
      args.window_id = atoi(value);
      break;
    case 'h':
      fprintf(stderr, AC_RESETALL AC_YELLOW AC_BOLD "Usage: parse-colors [OPTION]\n" AC_RESETALL);
      cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
      exit(EXIT_SUCCESS);
    }
  }
  return(EXIT_SUCCESS);
} /* parse_args */

