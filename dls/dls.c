#include "dls/dls.h"
static bool initialized;
static void __attribute__((constructor)) __constructor__dls(void);
static void __attribute__((destructor)) __destructor__dls(void);
static bool dls_normalize_arguments(int *argc, char *argv[]);
static struct Vector *dls_argv_to_arg_v(int argc, char *argv[]);
static void *dls_print_arg_v(char *title, char *color, int argc, char *argv[]);
static bool                   DARWIN_LS_DEBUG_MODE = false;
const enum output_mode_type_t DEFAULT_OUTPUT_MODE  = OUTPUT_MODE_TABLE;
static void __at_exit(void);
static void __at_exit(void){
  fprintf(stdout,"%s",AC_SHOW_CURSOR);
  fprintf(stdout,"%s",AC_RESTORE_PALETTE);
  fflush(stdout);
  return;
}
#define CREATE_SUBCOMMAND(NAME, SUBCOMMANDS)                                     \
  {                                                                              \
    .name        = cmds[COMMAND_ ## NAME].name,                                  \
    .description = cmds[COMMAND_ ## NAME].description,                           \
    .function    = cmds[COMMAND_ ## NAME].fxn,                                   \
    .about       = get_command_about(COMMAND_ ## NAME),                          \
    .options     = (struct optparse_opt[]){                                      \
      COMMON_OPTIONS_ ## NAME                                                    \
      { END_OF_OPTIONS },                                                        \
    },                                                                           \
    .subcommands = (struct optparse_cmd[]) { SUBCOMMANDS { END_OF_SUBCOMMANDS }, \
    }                                                                            \
  }
#define COMMON_OPTIONS_BASE \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
#define COMMON_OPTIONS_APP\
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args),\
  common_options_b[COMMON_OPTION_VERBOSE_MODE](args),              \
  common_options_b[COMMON_OPTION_DEBUG_MODE](args),                \
  common_options_b[COMMON_OPTION_CLEAR_SCREEN](args),
#define COMMON_OPTIONS_APP_LIST\
  common_options_b[COMMON_OPTION_OFFSET](args),\
  common_options_b[COMMON_OPTION_LIMIT](args),
#define COMMON_OPTIONS_PROCESS\
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args),\
  common_options_b[COMMON_OPTION_VERBOSE_MODE](args),              \
  common_options_b[COMMON_OPTION_DEBUG_MODE](args),                \
  common_options_b[COMMON_OPTION_CLEAR_SCREEN](args),
#define COMMON_OPTIONS_PROCESS_LIST\
  common_options_b[COMMON_OPTION_OFFSET](args),\
  common_options_b[COMMON_OPTION_LIMIT](args),
#define COMMON_OPTIONS_UI                                          \
  common_options_b[COMMON_OPTION_VERBOSE_MODE](args),              \
  common_options_b[COMMON_OPTION_DEBUG_MODE](args),                \
  common_options_b[COMMON_OPTION_ENABLE_PROGRESS_BAR_MODE](args),  \
  common_options_b[COMMON_OPTION_DISABLE_PROGRESS_BAR_MODE](args), \
  common_options_b[COMMON_OPTION_CLEAR_SCREEN](args),
#define COMMON_OPTIONS_LIMIT_OPTIONS \
  common_options_b[COMMON_OPTION_LIMIT](args),
#define COMMON_OPTIONS_ID                          \
  common_options_b[COMMON_OPTION_ID](args),        \
  common_options_b[COMMON_OPTION_RANDOM_ID](args), \
  common_options_b[COMMON_OPTION_ALL_MODE](args),
#define COMMON_OPTIONS_SORT                                 \
  common_options_b[COMMON_OPTION_SORT_DIRECTION_ASC](args), \
  common_options_b[COMMON_OPTION_SORT_DIRECTION_DESC](args),
#define COMMON_OPTIONS_SIZE                    \
  common_options_b[COMMON_OPTION_WIDTH](args), \
  common_options_b[COMMON_OPTION_HEIGHT](args),
///////////////////////////////////////////////////////////////////////////////////////////////////
#define SUBCOMMANDS_DB\
  CREATE_SUBCOMMAND(DB_INIT, ),      \
  CREATE_SUBCOMMAND(DB_INSERT_ROW_APP_ICON, ),      \
  CREATE_SUBCOMMAND(DB_TEST, ),      \
  CREATE_SUBCOMMAND(DB_INFO, ),      \
  CREATE_SUBCOMMAND(DB_LOAD, ),      \
  CREATE_SUBCOMMAND(DB_TABLES, ),    \
  CREATE_SUBCOMMAND(DB_ROWS, ),      \
  CREATE_SUBCOMMAND(DB_TABLE_IDS, ), \
  CREATE_SUBCOMMAND(DB,),
#define SUBCOMMANDS_APP_LIST
#define SUBCOMMANDS_APP\
  CREATE_SUBCOMMAND(APP_LIST, ),
#define SUBCOMMANDS_PROCESS_LIST
#define SUBCOMMANDS_PROCESS\
  CREATE_SUBCOMMAND(PROCESS_LIST, ),
#define COMMON_OPTIONS_TABLE                         \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args), \
  COMMON_OPTIONS_SORT                                \
  COMMON_OPTIONS_SIZE                                \
  COMMON_OPTIONS_LIMIT_OPTIONS                       \
  COMMON_OPTIONS_UI
#define COMMON_OPTIONS_WINDOWS                            \
  COMMON_OPTIONS_BASE                                     \
  COMMON_OPTIONS_UI                                       \
  COMMON_OPTIONS_CAPTURE_RESULT_FILTERS                   \
  COMMON_OPTIONS_TABLE                                    \
  common_options_b[COMMON_OPTION_SORT_WINDOW_KEYS](args), \
  common_options_b[COMMON_OPTION_HIDE_COLUMNS](args),     \
  common_options_b[COMMON_OPTION_SHOW_COLUMNS](args),
#define COMMON_OPTIONS_DB_INSERT_ROW_APP_ICON 
#define COMMON_OPTIONS_DB_SAVE \
  common_options_b[COMMON_OPTION_DB_SAVE](args),
#define COMMON_OPTIONS_PROCESSES \
  COMMON_OPTIONS_TABLE
#define COMMON_OPTIONS_CREATE_SPACE \
  COMMON_OPTIONS_TABLE
#define COMMON_OPTIONS_FOCUS \
  COMMON_OPTIONS_BASE        \
  COMMON_OPTIONS_UI          \
  COMMON_OPTIONS_WINDOW_IDS  \
  common_options_b[COMMON_OPTION_PID](args),
#define COMMON_OPTIONS_FOCUSED \
  COMMON_OPTIONS_FOCUS
#define COMMON_OPTIONS_MENU_BAR \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
#define COMMON_OPTIONS_DB \
  COMMON_OPTIONS_BASE     \
  COMMON_OPTIONS_UI       \
  common_options_b[COMMON_OPTION_CLEAR_SCREEN](args),
#define COMMON_OPTIONS_DB_TABLES \
  COMMON_OPTIONS_DB
#define COMMON_OPTIONS_DB_INFO \
  COMMON_OPTIONS_DB
#define COMMON_OPTIONS_DB_INIT \
  COMMON_OPTIONS_DB
#define COMMON_OPTIONS_DB_TEST \
  COMMON_OPTIONS_DB
#define COMMON_OPTIONS_DB_LOAD \
  COMMON_OPTIONS_DB            \
  common_options_b[COMMON_OPTION_DB_TABLES](args),
#define COMMON_OPTIONS_DB_TABLE_IDS \
  COMMON_OPTIONS_DB                 \
  common_options_b[COMMON_OPTION_DB_TABLES](args),
#define COMMON_OPTIONS_ICON
#define COMMON_OPTIONS_DB_ROWS \
  COMMON_OPTIONS_DB            \
  common_options_b[COMMON_OPTION_DB_TABLES](args),
#define COMMON_OPTIONS_SPACE
#define COMMON_OPTIONS_SPACE_LIST\
  common_options_b[COMMON_OPTION_OFFSET](args),\
  common_options_b[COMMON_OPTION_LIMIT](args),
#define COMMON_OPTIONS_SPACE_CREATE
#define COMMON_OPTIONS_WINDOW
#define COMMON_OPTIONS_HOTKEYS                       \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args), \
  common_options_b[COMMON_OPTION_LIMIT](args),
#define COMMON_OPTIONS_HOTKEYS_LIST \
  common_options_b[COMMON_OPTION_LIMIT](args),
#define COMMON_OPTIONS_HOTKEYS_SERVER
#define COMMON_OPTIONS_WINDOW_LIST \
  common_options_b[COMMON_OPTION_OFFSET](args),\
  common_options_b[COMMON_OPTION_LIMIT](args),
#define COMMON_OPTIONS_WINDOW_PROPS \
  common_options_b[COMMON_OPTION_OFFSET](args),\
  common_options_b[COMMON_OPTION_LIMIT](args),
#define COMMON_OPTIONS_WINDOW_NAMES \
  common_options_b[COMMON_OPTION_OFFSET](args),\
  common_options_b[COMMON_OPTION_LIMIT](args),
#define COMMON_OPTIONS_WINDOW_QTY
#define COMMON_OPTIONS_WINDOW_IDS \
  common_options_b[COMMON_OPTION_OFFSET](args),\
  common_options_b[COMMON_OPTION_LIMIT](args),
#define COMMON_OPTIONS_WINDOW_STICKY
#define COMMON_OPTIONS_WINDOW_UNSTICKY
#define COMMON_OPTIONS_WINDOW_ALL_SPACES
#define COMMON_OPTIONS_WINDOW_NOT_ALL_SPACES
#define COMMON_OPTIONS_WINDOW_SPACE         \
  common_options_b[COMMON_OPTION_ID](args), \
  common_options_b[COMMON_OPTION_SPACE_ID](args),
#define COMMON_OPTIONS_WINDOW_MINIMIZE               \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args), \
  common_options_b[COMMON_OPTION_ID](args),
#define COMMON_OPTIONS_WINDOW_UNMINIMIZE             \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args), \
  common_options_b[COMMON_OPTION_ID](args),
#define COMMON_OPTIONS_WINDOW_RESIZE                  \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args),  \
  common_options_b[COMMON_OPTION_ID](args),           \
  common_options_b[COMMON_OPTION_RANDOM_ID](args),    \
  common_options_b[COMMON_OPTION_WINDOW_WIDTH](args), \
  common_options_b[COMMON_OPTION_WINDOW_HEIGHT](args),
#define COMMON_OPTIONS_WINDOW_MOVE                   \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args), \
  common_options_b[COMMON_OPTION_ID](args),          \
  common_options_b[COMMON_OPTION_WINDOW_X](args),    \
  common_options_b[COMMON_OPTION_WINDOW_Y](args),
#define COMMON_OPTIONS_ICON
#define COMMON_OPTIONS_ICON_LIST
#define COMMON_OPTIONS_DOCK
//#########################################
#define COMMON_OPTIONS_PASTE \
  common_options_b[COMMON_OPTION_OUTPUT_FILE](args),
#define COMMON_OPTIONS_COPY                         \
  common_options_b[COMMON_OPTION_INPUT_FILE](args), \
  common_options_b[COMMON_OPTION_CONTENT](args),
#define SUBCOMMANDS_ICON_LIST
#define SUBCOMMANDS_ICON \
  CREATE_SUBCOMMAND(ICON_LIST, ),
#define SUBCOMMANDS_WINDOW                    \
  CREATE_SUBCOMMAND(WINDOW_IDS, ),            \
  CREATE_SUBCOMMAND(WINDOW_LIST, ),           \
  CREATE_SUBCOMMAND(WINDOW_QTY, ),            \
  CREATE_SUBCOMMAND(WINDOW_NAMES, ),          \
  CREATE_SUBCOMMAND(WINDOW_PROPS, ),          \
  CREATE_SUBCOMMAND(WINDOW_MOVE, ),           \
  CREATE_SUBCOMMAND(WINDOW_SPACE, ),          \
  CREATE_SUBCOMMAND(WINDOW_MINIMIZE, ),       \
  CREATE_SUBCOMMAND(WINDOW_UNMINIMIZE, ),     \
  CREATE_SUBCOMMAND(WINDOW_STICKY, ),         \
  CREATE_SUBCOMMAND(WINDOW_UNSTICKY, ),       \
  CREATE_SUBCOMMAND(WINDOW_ALL_SPACES, ),     \
  CREATE_SUBCOMMAND(WINDOW_NOT_ALL_SPACES, ), \
  CREATE_SUBCOMMAND(WINDOW_RESIZE, ),
#define SUBCOMMANDS_SPACE          \
  CREATE_SUBCOMMAND(SPACE_LIST, ), \
  CREATE_SUBCOMMAND(SPACE_CREATE, ),
#define SUBCOMMANDS_HOTKEYS          \
  CREATE_SUBCOMMAND(HOTKEYS_LIST, ), \
  CREATE_SUBCOMMAND(HOTKEYS_SERVER, ),
//#########################################
struct normalized_argv_t {
  char          *mode, *executable;
  struct Vector *pre_mode_arg_v, *post_mode_arg_v, *arg_v;
};
struct args_t *args = &(struct args_t){
  .limit                              = 999,
  .verbose_mode                       = false, .debug_mode = false,
  .space_id                           = -1,
  .display_id                         = -1,
  .id                                 = 0,
  .capture_type                       = DEFAULT_CAPTURE_TYPE,
  .capture_mode[DEFAULT_CAPTURE_TYPE] = true,
  .progress_bar_mode                  = DEFAULT_PROGRESS_BAR_ENABLED,
  .output_mode                        = DEFAULT_OUTPUT_MODE,
  .sort_key                           = NULL,
  .sort_direction                     = "asc",
  .current_space_only                 = false,
  .dls_clear_screen                       = false,
  .minimized_only                     = false,
  .application_name                   = NULL,
  .width_greater                      = -1, .width_less = -1,
  .height_greater                     = -1, .height_less = -1,
  .width                              = -1, .height = -1,
  .output_file                        = NULL,
  .concurrency                        = 1,
  .display_mode                       = false,
  .all_mode                           = false,
  .font_name                          = NULL, .font_family = NULL, .font_style = NULL,
  .exact_match                        = false, .case_sensitive = false,
  .retries                            = 0,
  .duration_seconds                   = 10,
  .write_images_mode                  = false,
  .purge_write_directory_before_write = false,
  .write_directory                    = "/tmp",
  .formats_v                          = NULL,
  .format_ids_v                       = NULL,
  .pid                                = -1, .windowids = NULL,
  .icon_sizes_v                       = NULL,
  .format_names_qty                   = 0,
  .hide_columns_qty                   = 0,
  .offset = 0,
//  .fd = stdout,
};
char                *DLS_RE_EXEC_CMD = NULL;
struct optparse_cmd *dls_cmd         = NULL;
int                 dls_cmd_argc;

////////////////////////////////////////////
int main(int argc, char *argv[]) {
  atexit(__at_exit);
  return(handle_main(argc, argv));
}

int handle_main(int argc, char *argv[]) {
  DLS_EXECUTABLE_ARGC = argc;
  char *_cmd = stringfn_join(argv, " ", 0, argc);
  DLS_EXECUTABLE_ARGV = stringfn_split(_cmd, ' ').strings;

  VIPS_INIT(argv[0]);
  asprintf(&DLS_EXECUTABLE, "%s/%s",
           whereami->executable_directory,
           whereami->executable_basename);
  asprintf(&DLS_RE_EXEC_CMD, "%s/%s %s",
           whereami->executable_directory,
           whereami->executable_basename,
           stringfn_join(argv, " ", 1, argc - 1)
           );
  args->pid = getpid();
  if (DEBUG_ARGV)
    dls_print_arg_v("pre", AC_YELLOW, argc, argv);
  if (NORMALIZE_ARGV)
    dls_normalize_arguments(&argc, argv);
  if (DEBUG_ARGV)
    dls_print_arg_v("post", AC_MAGENTA, argc, argv);

////////////////////////////////////////////
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
  struct optparse_cmd cmd = {
    .about       = "dls v1.00 - List Darwin Objects",
    .description = "This program lists Darwin Objects",
    .name        = "dls",
    .operands    = "[COMMAND...]",
    .options     = (struct optparse_opt[]) {
      common_options_b[COMMON_OPTION_HELP](args),
      common_options_b[COMMON_OPTION_VERBOSE_MODE](args),
      common_options_b[COMMON_OPTION_DEBUG_MODE](args),
      common_options_b[COMMON_OPTION_OUTPUT_MODE](args),
      { END_OF_OPTIONS },
    },
    .subcommands     = (struct optparse_cmd[]) {
      {
        .description = "Print a subcommand's help information and quit.",
        .name        = "help",
        .operands    = "COMMAND",
        .about       = "🌍" "\t" COLOR_HELP "Command Help" AC_RESETALL,
        .function    = optparse_print_help_subcmd,
      },
      ADD_SUBCOMMANDS()
#undef ADD_SUBCOMMANDS
      CREATE_SUBCOMMAND(DB, SUBCOMMANDS_DB),
      CREATE_SUBCOMMAND(DOCK, ),
      CREATE_SUBCOMMAND(MENU_BAR, ),
      CREATE_SUBCOMMAND(APP, SUBCOMMANDS_APP),
      CREATE_SUBCOMMAND(WINDOW, SUBCOMMANDS_WINDOW),
      CREATE_SUBCOMMAND(PROCESS, SUBCOMMANDS_PROCESS),
      CREATE_SUBCOMMAND(HOTKEYS, SUBCOMMANDS_HOTKEYS),
      CREATE_SUBCOMMAND(SPACE, SUBCOMMANDS_SPACE),
      CREATE_SUBCOMMAND(ICON, SUBCOMMANDS_ICON),
#undef CREATE_SUBCOMMAND
      {
        END_OF_SUBCOMMANDS
      },
    },
  };
#pragma GCC diagnostic pop
  dls_cmd      = &cmd;
  dls_cmd_argc = argc;
  dls_cmd_argv = calloc(argc, 1024);
  for (int i = 0; i < dls_cmd_argc; i++)
    dls_cmd_argv[i] = strdup(argv[i]);
  dls_cmd_argv[dls_cmd_argc] = "\0";
  optparse_parse(&cmd, &argc, &argv);
  optparse_print_help();
  return(EXIT_FAILURE);
} /* main */

static void *dls_print_arg_v(char *title, char *color, int argc, char *argv[]){
  printf(AC_GREEN "%s\n" AC_RESETALL, title);
  for (int i = 0; i < argc; i++)
    printf("#%d> %s%s"AC_RESETALL "\n", i, color, argv[i]);
  char *cmd = stringfn_join(argv, " ", 0, argc);
  printf("\t\t\t%s%s"AC_RESETALL "\n", color, cmd);
  free(cmd);
}

static struct normalized_argv_t *dls_argv_normalized_argv_t(int argc, char *argv[]){
  struct normalized_argv_t *r = calloc(1, sizeof(struct normalized_argv_t));

  r->arg_v = dls_argv_to_arg_v(argc, argv);
  char          *cur    = NULL;
  struct Vector *prev_v = vector_new();

  for (size_t i = 0; i < vector_size(r->arg_v); i++) {
    cur = (char *)vector_get(r->arg_v, i);
    log_info("%lu", vector_size(prev_v));
    if (!cur)
      continue;
    if (i == 0)
      r->executable = cur;
    for (size_t x = 0; x < vector_size(prev_v); x++) {
      char *prev = (x > 0) ? (char *)vector_get(prev_v, x - 1) : NULL;
      Dbg(prev, %s);
      if (prev && dls_get_arg_is_ordered(cur, prev)) {
        log_info("#%lu> args need to be swapped: %s should be after %s", i, prev, cur);
        vector_set(r->arg_v, x, strdup(cur));
        vector_set(r->arg_v, i, strdup(prev));
      }
    }
    vector_push(prev_v, (void *)cur);
    if (i > 0 && !r->mode && !stringfn_starts_with(cur, "-")) {
      char *aliased = dls_get_alias_wildcard_glob_name(cur);
      if (aliased) {
        r->mode = strdup(aliased);
        vector_set(r->arg_v, i, (void *)r->mode);
        argv[i] = r->mode;
      }else
        r->mode = cur;
    }else{
      if (!r->mode)
        vector_push(r->pre_mode_arg_v, cur);
      else
        vector_push(r->post_mode_arg_v, cur);
    }
  }
  for (size_t i = 0; i < vector_size(r->arg_v); i++) {
    char *s = (char *)vector_get(r->arg_v, i);
    if (strcmp(argv[i], s) != 0)
      argv[i] = s;
  }
  return(r);
} /* dls_argv_normalized_argv_t */
static struct Vector *dls_argv_to_arg_v(int argc, char *argv[]){
  struct Vector *arg_v = vector_new();

  for (int i = 0; i < argc; i++)
    vector_push(arg_v, (void *)strdup(argv[i]));
  return(arg_v);
}

static bool dls_normalize_arguments(int *argc, char *argv[]){
  struct normalized_argv_t *r = dls_argv_normalized_argv_t(*argc, argv);

  log_info("Mode: %s", r->mode);
  *argc = vector_size(r->arg_v);
  log_info("argc: %d", *argc);
}
static void __attribute__((destructor)) __destructor__dls(void){
}

static void __attribute__((constructor)) __constructor__dls(void){
  log_debug("%lu options", sizeof(__optparse_opt) / sizeof(__optparse_opt[0]));
//^[\^[]4;1;#cc6666^[\^[]4;2;#66cc99^[\^[]4;3;#cc9966^[\^[]4;4;#6699cc^[\^[]4;5;#cc6699^[\^[]4;6;#66cccc^[\^[]4;7;#cccccc^[\^[]4;8;#999999^[\^[]4;9;#cc6666^[\^[]4;10;#66cc99^[\^[]4;11;#cc9966^[\^[]4;12;#6699cc^[\^[]4;13;#cc6699^[\^[]4;14;#66cccc^[\^[]4;15;#cccccc^[\^[[21D"
  char *__ansi = ""\
  "\033]11;#000000"\
  "\033]10;#ffffff"\
  "\033]12;#ff9999"\
  "\033]4;13;#cc6699"\
  "\033]4;14;#66cccc"\
  "\033]4;15;#cccccc"\
  "\033[?1049l"\
  "\033[21D"\
  "";
  if (false)
    printf(
      "%s"
      "%s"
      "%s",
      AC_SAVE_PALETTE,
      AC_HIDE_CURSOR,
      __ansi
      );
  initialized = true;
//  fprintf(stdout, AC_SAVE_PALETTE);
  fprintf(stdout, AC_HIDE_CURSOR);
  fflush(stdout);

  if (getenv("DEBUG") != NULL || getenv("DEBUG_DARWIN_LS") != NULL)
    DARWIN_LS_DEBUG_MODE = true;
  errno    = 0;
  whereami = core_utils_whereami_report();
  if (DARWIN_LS_DEBUG_MODE)
    log_debug("  >Whereami Report<\n\tExecutable:%s\n\tDirectory:%s\n\tBasename:%s\n",
              whereami->executable, whereami->executable_directory, whereami->executable_basename
              );
  errno = 0;
  if (is_authorized_for_accessibility() == false) {
    log_error("Not Authorized for Accessibility");
    exit(EXIT_FAILURE);
  }
  errno = 0;
  if (is_authorized_for_screen_recording() == false) {
    log_error("Not Authorized for Screen Recording");
    exit(EXIT_FAILURE);
  }
} /* __constructor__dls */
