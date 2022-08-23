////////////////////////////////////////////////////
#include "wrec-cli/wrec-cli.h"
#include "wrec-utils/wrec-utils.h"
////////////////////////////////////////////////////
static struct args_t args = {
  .mode                        = DEFAULT_MODE,
  .verbose                     = DEFAULT_VERBOSE,
  .window_id                   = DEFAULT_WINDOW_ID,
  .max_recorded_frames         = DEFAULT_MAX_RECORD_FRAMES,
  .max_record_duration_seconds = DEFAULT_MAX_RECORD_DURATION_SECONDS,
  .frames_per_second           = DEFAULT_FRAMES_PER_SECOND,
  .mode_list                   = DEFAULT_MODE_LIST,
  .mode_capture                = DEFAULT_MODE_CAPTURE,
  .mode_debug_args             = DEFAULT_MODE_DEBUG_ARGS,
  .resize_type                 = DEFAULT_RESIZE_TYPE,
  .resize_value                = DEFAULT_RESIZE_VALUE,
  .application_name_glob       = DEFAULT_APPLICATION_NAME_GLOB,
};
static int select_window(void);
static int parse_args(int argc, char *argv[]);
static int debug_args();
static struct modes_t    modes[] = {
  { .name = "debug_args",    .description = "Debug Arguments", .handler = debug_args         },
  { .name = "list",          .description = "List Windows",    .handler = list_windows_table },
  { .name = "record",        .description = "Capture Window",  .handler = capture_window     },
  { .name = "select_window", .description = "Select Window",   .handler = select_window      },
  { 0 },
};
static struct cag_option options[] = {
  { .identifier     = 'm',
    .access_letters = "m",
    .access_name    = "mode",
    .value_name     = "MODE",
    .description    = "Mode" },
  { .identifier     = 'v',.access_letters  = "v", .access_name = "verbose",          .value_name = NULL,                    .description = "Verbose Mode"                                                              },
  { .identifier     = 'f',.access_letters  = "f", .access_name = "max-frames",       .value_name = "MAX_FRAMES",            .description = "Max Recorded Frames"                                                       },
  { .identifier     = 'S',.access_letters  = "S", .access_name = "select-window",    .value_name = NULL,                    .description = "Select Window"                                                             },
  { .identifier     = 'F',.access_letters  = "F", .access_name = "fps",              .value_name = "RECORD_FPS",            .description = "Frames Per Second"                                                         },
  { .identifier     = 's',.access_letters  = "s", .access_name = "max-seconds",      .value_name = "MAX_SECONDS",           .description = "Max Recorded Seconds"                                                      },
  { .identifier     = 'F',.access_letters  = "F", .access_name = "fps",              .value_name = "RECORD_FPS",            .description = "Frames Per Second"                                                         },
  { .identifier     = 'w',
    .access_letters = "w",
    .access_name    = "window",
    .value_name     = "WINDOW_ID",
    .description    = "Window ID" },
  { .identifier     = 'c',.access_letters  = "c", .access_name = "capture",          .value_name = "MODE_CAPTURE",          .description = "Capture Mode"                                                              },
  { .identifier     = 'l',.access_letters  = "l", .access_name = "list",             .value_name = "MODE_LIST",             .description = "List Mode"                                                                 },
  { .identifier     = 'a',.access_letters  = "a", .access_name = "debug-args",       .value_name = "MODE_DEBUG_ARGS",       .description = "Debug Arguments"                                                           },
  { .identifier     = 'W',
    .access_letters = "W",
    .access_name    = "resize-width",
    .value_name     = "RESIZE_WIDTH",
    .description    = "Resize To Width" },
  { .identifier     = 'H',
    .access_letters = "H",
    .access_name    = "resize-height",
    .value_name     = "RESIZE_HEIGHT",
    .description    = "Resize To Height" },
  { .identifier     = 'R',
    .access_letters = "R",
    .access_name    = "resize-factor",
    .value_name     = "RESIZE_FACTOR",
    .description    = "Resize Factor" },
  { .identifier     = 'A',.access_letters  = "A", .access_name = "application-name", .value_name = "APPLICATION_NAME_GLOB", .description = "Application Name [Default: " DEFAULT_APPLICATION_NAME_GLOB "] (ex: *itty)" },
  { .identifier     = 'h',
    .access_letters = "h",
    .access_name    = "help",
    .description    = "Shows the command help" }
};

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
          acs(AC_BRIGHT_YELLOW_BLACK AC_ITALIC  "Max Recorded Frames: %lu") "\n"
          acs(AC_BRIGHT_YELLOW_BLACK AC_ITALIC  "Max Record Seconds: %lu") "\n"
          acs(AC_BRIGHT_YELLOW_BLACK AC_ITALIC  "Frames Per Second: %d") "\n"
          acs(AC_BRIGHT_YELLOW_BLACK AC_ITALIC  "Capture Mode: %d") "\n"
          acs(AC_BRIGHT_YELLOW_BLACK AC_ITALIC  "List Mode: %d") "\n"
          acs(AC_BRIGHT_YELLOW_BLACK AC_ITALIC  "Debug Args Mode: %d") "\n"
          acs(AC_BRIGHT_YELLOW_BLACK AC_ITALIC  "Resize Type: %s") "\n"
          acs(AC_BRIGHT_YELLOW_BLACK AC_ITALIC  "Resize Value: %d") "\n"
          acs(AC_BRIGHT_BLUE_BLACK AC_ITALIC  "Application Name (glob): %s") "\n"
          ,
          args.verbose,
          args.mode,
          args.window_id,
          args.max_recorded_frames,
          args.max_record_duration_seconds,
          args.frames_per_second,
          args.mode_capture,
          args.mode_list,
          args.mode_debug_args,
          resize_type_name(args.resize_type),
          args.resize_value,
          args.application_name_glob
          );

  return(EXIT_SUCCESS);
}

static int parse_args(int argc, char *argv[]){
  cag_option_context context;

  cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&context)) {
    char identifier = cag_option_get(&context);
    switch (identifier) {
    case 'm': args.mode                        = cag_option_get_value(&context); break;
    case 'S': args.mode                        = "select_window"; break;
    case 'v': args.verbose                     = true; break;
    case 'w': args.window_id                   = atoi(cag_option_get_value(&context)); break;
    case 'f': args.max_recorded_frames         = atoi(cag_option_get_value(&context)); break;
    case 's': args.max_record_duration_seconds = atoi(cag_option_get_value(&context)); break;
    case 'F': args.frames_per_second           = atoi(cag_option_get_value(&context)); break;
    case 'c': args.mode_capture                = true; args.mode = "capture"; break;
    case 'a': args.mode_debug_args             = true; args.mode = "debug_args"; break;
    case 'l': args.mode_list                   = true; args.mode = "list"; break;
    case 'W':
      args.resize_type  = RESIZE_BY_WIDTH;
      args.resize_value = atoi(cag_option_get_value(&context));
      break;
    case 'H':
      args.resize_type  = RESIZE_BY_HEIGHT;
      args.resize_value = atoi(cag_option_get_value(&context));
      break;
    case 'R':
      args.resize_type  = RESIZE_BY_FACTOR;
      args.resize_value = atoi(cag_option_get_value(&context));
      break;
    case 'h':
      fprintf(stderr, AC_RESETALL AC_YELLOW AC_BOLD "Usage: wrec [OPTION]\n" AC_RESETALL);
      cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
      exit(EXIT_SUCCESS);
    case 'A': args.application_name_glob = cag_option_get_value(&context); break;
    }
  }
  return(EXIT_SUCCESS);
} /* parse_args */

static int select_window(void){
  int               res = -1;

  struct fzf_exec_t *fe = exec_fzf_setup();

  assert(fe != NULL);
  struct Vector *windows = get_windows();

  for (size_t i = 0; i < vector_size(windows); i++) {
    window_t *w = (window_t *)vector_get(windows, i);
    vector_push(fe->input_options, w->app_name);
  }

  fe->header       = "Select Window";
  fe->debug_mode   = true;
  fe->height       = 100;
  fe->preview_size = 0;

  res = exec_fzf(fe);
  assert(res == 0);

  log_info("Selected %lu/%lu options", vector_size(fe->selected_options), vector_size(fe->input_options));
  exec_fzf_release(fe);
  return(EXIT_SUCCESS);
}
