#include "dls/dls.h"
#include "c_forever/include/forever.h"
#define DEBUG_ARGV false
#define NORMALIZE_ARGV false
#define DEFAULT_PROGRESS_BAR_ENABLED true
#define DEFAULT_CAPTURE_TYPE CAPTURE_TYPE_WINDOW
static void __attribute__((constructor)) __constructor__dls(void);
static bool dls_normalize_arguments(int *argc, char *argv[]);
static struct Vector *dls_argv_to_arg_v(int argc, char *argv[]);
static void *dls_print_arg_v(char *title, char *color, int argc, char *argv[]);
const enum output_mode_type_t DEFAULT_OUTPUT_MODE  = OUTPUT_MODE_TABLE;
static bool                   DARWIN_LS_DEBUG_MODE = false;
struct normalized_argv_t {
  char *mode, *executable;
  struct Vector *pre_mode_arg_v, *post_mode_arg_v, *arg_v;
};
struct args_t                 *args                = &(struct args_t){
  .verbose_mode        = false, .debug_mode = false,
  .space_id            = -1,
  .display_id          = -1,
  .purge_write_directory_before_write = false,
  .id           = 0,
  .capture_type         = DEFAULT_CAPTURE_TYPE,
  .capture_mode[DEFAULT_CAPTURE_TYPE] = true,
  .progress_bar_mode = DEFAULT_PROGRESS_BAR_ENABLED,
  .output_mode         = DEFAULT_OUTPUT_MODE,
  .sort_key            = NULL,
  .sort_direction      = "asc",
  .current_space_only  = false,
  .clear_screen        = false,
  .minimized_only      = false,
  .pid                 = -1,
  .application_name    = NULL,
  .width_greater       = -1, .width_less = -1,
  .height_greater      = -1, .height_less = -1,
  .width               = -1, .height = -1,
  .output_file         = NULL,
  .concurrency         = 1,
  .display_mode = false,
  .all_mode         = false,
  .limit               = 50,
  .font_name           = NULL, .font_family = NULL, .font_style = NULL,
  .exact_match         = false, .case_sensitive = false,
  .retries             = 0,
  .image_format        = "png",
  .image_format_type   = IMAGE_TYPE_PNG,
  .duration_seconds    = 10,
  .write_images_mode = true,
};

////////////////////////////////////////////
int main(int argc, char *argv[]) {
  VIPS_INIT(argv[0]);
  if(DEBUG_ARGV)
    dls_print_arg_v("pre",AC_YELLOW,argc, argv);
  if(NORMALIZE_ARGV)
    dls_normalize_arguments(&argc, argv);
  if(DEBUG_ARGV)
    dls_print_arg_v("post",AC_MAGENTA,argc, argv);
  struct optparse_cmd main_cmd = {
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
#define COMMON_OPTIONS_BASE          \
        common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
#define COMMON_OPTIONS_UI          \
        common_options_b[COMMON_OPTION_VERBOSE_MODE](args),\
        common_options_b[COMMON_OPTION_DEBUG_MODE](args),\
        common_options_b[COMMON_OPTION_ENABLE_PROGRESS_BAR_MODE](args),\
        common_options_b[COMMON_OPTION_DISABLE_PROGRESS_BAR_MODE](args),\
        common_options_b[COMMON_OPTION_CLEAR_SCREEN](args),
#define COMMON_OPTIONS_CAPTURE_RESULT_IMAGE_OPTIONS          \
        common_options_b[COMMON_OPTION_WRITE_IMAGES_MODE](args),
#define COMMON_OPTIONS_CAPTURE_RESULT_FILE_OPTIONS          \
        common_options_b[COMMON_OPTION_OUTPUT_FILE](args),\
        common_options_b[COMMON_OPTION_DISPLAY_OUTPUT_FILE](args),
#define COMMON_OPTIONS_CAPTURE_RESULT_OPTIONS          \
        common_options_b[COMMON_OPTION_PURGE_WRITE_DIRECTORY_BEFORE_WRITE](args),\
        common_options_b[COMMON_OPTION_WRITE_DIRECTORY](args),\
        COMMON_OPTIONS_CAPTURE_RESULT_FILE_OPTIONS \
        COMMON_OPTIONS_CAPTURE_RESULT_IMAGE_OPTIONS
#define COMMON_OPTIONS_CAPTURE_SIZE_FILTERS          \
        common_options_b[COMMON_OPTION_HEIGHT_GREATER](args),\
        common_options_b[COMMON_OPTION_HEIGHT_LESS](args),\
        common_options_b[COMMON_OPTION_WIDTH_GREATER](args),\
        common_options_b[COMMON_OPTION_WIDTH_LESS](args),
#define COMMON_OPTIONS_CAPTURE_RESULT_FILTERS          \
        common_options_b[COMMON_OPTION_CURRENT_SPACE](args),\
        common_options_b[COMMON_OPTION_NOT_CURRENT_SPACE](args),\
        common_options_b[COMMON_OPTION_CURRENT_DISPLAY](args),\
        common_options_b[COMMON_OPTION_NOT_CURRENT_DISPLAY](args),\
        common_options_b[COMMON_OPTION_SPACE_ID](args),\
        common_options_b[COMMON_OPTION_APPLICATION_NAME](args),\
        common_options_b[COMMON_OPTION_DISPLAY_ID](args),\
        common_options_b[COMMON_OPTION_PID](args),\
        common_options_b[COMMON_OPTION_APPLICATION_NAME](args),\
        common_options_b[COMMON_OPTION_NOT_MINIMIZED](args),\
        common_options_b[COMMON_OPTION_MINIMIZED](args),\
        COMMON_OPTIONS_CAPTURE_SIZE_FILTERS
#define COMMON_OPTIONS_LIMIT_OPTIONS          \
        common_options_b[COMMON_OPTION_LIMIT](args),
#define COMMON_OPTIONS_IMAGE_CAPTURE_OPTIONS          \
        common_options_b[COMMON_OPTION_GRAYSCALE_MODE](args),\
        common_options_b[COMMON_OPTION_IMAGE_FORMAT](args),
#define COMMON_OPTIONS_CAPTURE_OPTIONS          \
        common_options_b[COMMON_OPTION_COMPRESS](args),\
        common_options_b[COMMON_OPTION_CONCURRENCY](args),\
        COMMON_OPTIONS_LIMIT_OPTIONS \
        COMMON_OPTIONS_IMAGE_CAPTURE_OPTIONS
#define COMMON_OPTIONS_ID\
        common_options_b[COMMON_OPTION_ID](args),\
        common_options_b[COMMON_OPTION_RANDOM_ID](args),\
        common_options_b[COMMON_OPTION_ALL_MODE](args),
#define COMMON_OPTIONS_SORT \
        common_options_b[COMMON_OPTION_SORT_DIRECTION_ASC](args),\
        common_options_b[COMMON_OPTION_SORT_DIRECTION_DESC](args),
#define COMMON_OPTIONS_SIZE \
        common_options_b[COMMON_OPTION_WIDTH](args),\
        common_options_b[COMMON_OPTION_HEIGHT](args),
///////////////////////////////////////////////////////////////////////////////////////////////////
#define COMMON_OPTIONS_CAPTURE_TYPE\
        common_options_b[COMMON_OPTION_CAPTURE_WINDOW_MODE](args),\
        common_options_b[COMMON_OPTION_CAPTURE_SPACE_MODE](args),\
        common_options_b[COMMON_OPTION_CAPTURE_DISPLAY_MODE](args),\
        COMMON_OPTIONS_ID
#define COMMON_OPTIONS_TABLE\
        COMMON_OPTIONS_SORT \
        COMMON_OPTIONS_SIZE\
        COMMON_OPTIONS_LIMIT_OPTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////
#define COMMON_OPTIONS_ANIMATE \
        COMMON_OPTIONS_BASE\
        COMMON_OPTIONS_UI\
        COMMON_OPTIONS_CAPTURE_RESULT_OPTIONS\
        COMMON_OPTIONS_CAPTURE_OPTIONS\
        COMMON_OPTIONS_CAPTURE_TYPE\
        COMMON_OPTIONS_SIZE \
        common_options_b[COMMON_OPTION_DURATION_SECONDS](args),\
        common_options_b[COMMON_OPTION_FRAME_RATE](args),
#define COMMON_OPTIONS_WINDOWS \
        COMMON_OPTIONS_BASE \
        COMMON_OPTIONS_UI\
        COMMON_OPTIONS_CAPTURE_RESULT_FILTERS\
        COMMON_OPTIONS_TABLE\
        common_options_b[COMMON_OPTION_SORT_WINDOW_KEYS](args),
#define COMMON_OPTIONS_CAPTURE\
        COMMON_OPTIONS_BASE\
        COMMON_OPTIONS_UI\
        COMMON_OPTIONS_CAPTURE_RESULT_OPTIONS\
        COMMON_OPTIONS_CAPTURE_OPTIONS\
        COMMON_OPTIONS_CAPTURE_TYPE\
        COMMON_OPTIONS_SIZE
#define COMMON_OPTIONS_EXTRACT\
        COMMON_OPTIONS_BASE\
        COMMON_OPTIONS_UI\
        COMMON_OPTIONS_CAPTURE_RESULT_OPTIONS\
        COMMON_OPTIONS_CAPTURE_OPTIONS\
        COMMON_OPTIONS_CAPTURE_TYPE
///////////////////////////////////////////////////////////////////////////////////////////////////
    .subcommands     = (struct optparse_cmd[]) {
      {
        .description = "Print a subcommand's help information and quit.",
        .name        = "help",
        .operands    = "COMMAND",
        .about       = "🌍" "\t" COLOR_HELP "Command Help" AC_RESETALL,
        .function    = optparse_print_help_subcmd,
      },
//////////////////////////////////////////
#define CREATE_SUBCOMMAND(NAME)\
      {\
        .name        = cmds[COMMAND_##NAME].name,\
        .description = cmds[COMMAND_##NAME].description,\
        .function    = cmds[COMMAND_##NAME].fxn,\
        .about       = get_command_about(COMMAND_##NAME),\
        .options     = (struct optparse_opt[]){\
          COMMON_OPTIONS_##NAME\
          { END_OF_OPTIONS },\
        },\
      }
//////////////////////////////////////////
      CREATE_SUBCOMMAND(WINDOWS),
      CREATE_SUBCOMMAND(CAPTURE),
      CREATE_SUBCOMMAND(EXTRACT),
      CREATE_SUBCOMMAND(ANIMATE),
#undef CREATE_SUBCOMMAND
      {
        .name        = cmds[COMMAND_FOCUS_SPACE].name,
        .description = cmds[COMMAND_FOCUS_SPACE].description,
        .function    = cmds[COMMAND_FOCUS_SPACE].fxn,
        .about       = get_command_about(COMMAND_FOCUS_SPACE),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_FOCUS_WINDOW].name,
        .description = cmds[COMMAND_FOCUS_WINDOW].description,
        .function    = cmds[COMMAND_FOCUS_WINDOW].fxn,
        .about       = get_command_about(COMMAND_FOCUS_WINDOW),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_ID](args),
          common_options_b[COMMON_OPTION_RANDOM_ID](args),
          common_options_b[COMMON_OPTION_APPLICATION_NAME](args),
          common_options_b[COMMON_OPTION_CURRENT_SPACE](args),
          common_options_b[COMMON_OPTION_DISPLAY_ID](args),
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          common_options_b[COMMON_OPTION_PID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_MENU_BAR].name,
        .description = cmds[COMMAND_MENU_BAR].description,
        .function    = cmds[COMMAND_MENU_BAR].fxn,
        .about       = get_command_about(COMMAND_MENU_BAR),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_DOCK].name,
        .description = cmds[COMMAND_DOCK].description,
        .function    = cmds[COMMAND_DOCK].fxn,
        .about       = get_command_about(COMMAND_DOCK),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_DISPLAYS].name,
        .description = cmds[COMMAND_DISPLAYS].description,
        .function    = cmds[COMMAND_DISPLAYS].fxn,
        .about       = get_command_about(COMMAND_DISPLAYS),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_ALACRITTYS].name,
        .description = cmds[COMMAND_ALACRITTYS].description,
        .function    = cmds[COMMAND_ALACRITTYS].fxn,
        .about       = get_command_about(COMMAND_ALACRITTYS),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_KITTYS].name,
        .description = cmds[COMMAND_KITTYS].description,
        .function    = cmds[COMMAND_KITTYS].fxn,
        .about       = get_command_about(COMMAND_KITTYS),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_SECURITY].name,
        .description = cmds[COMMAND_SECURITY].description,
        .function    = cmds[COMMAND_SECURITY].fxn,
        .about       = get_command_about(COMMAND_SECURITY),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_RETRIES](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "processes",
        .description = "Processes",
        .function    = cmds[COMMAND_PROCESSES].fxn,
        .about       = "💒" "\t" COLOR_LIST "List Processes" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "monitors",
        .description = "Monitors",
        .function    = cmds[COMMAND_MONITORS].fxn,
        .about       = "💮" "\t" COLOR_LIST "List Monitors" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "usb",
        .description = "USB Devices",
        .function    = cmds[COMMAND_USBS].fxn,
        .about       = "📡" "\t" COLOR_LIST "List USB Devices" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_IMAGE_CONVERSIONS].name,
        .description = cmds[COMMAND_IMAGE_CONVERSIONS].description,
        .function    = cmds[COMMAND_IMAGE_CONVERSIONS].fxn,
        .about       = get_command_about(COMMAND_IMAGE_CONVERSIONS),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_INPUT_FILE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_FONTS].name,
        .description = cmds[COMMAND_FONTS].description,
        .function    = cmds[COMMAND_FONTS].fxn,
        .about       = get_command_about(COMMAND_FONTS),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          common_options_b[COMMON_OPTION_FONT_NAME](args),
          common_options_b[COMMON_OPTION_FONT_FAMILY](args),
          common_options_b[COMMON_OPTION_FONT_TYPE](args),
          common_options_b[COMMON_OPTION_FONT_STYLE](args),
          common_options_b[COMMON_OPTION_SORT_FONT_KEYS](args),
          common_options_b[COMMON_OPTION_SORT_DIRECTION_ASC](args),
          common_options_b[COMMON_OPTION_SORT_DIRECTION_DESC](args),
          common_options_b[COMMON_OPTION_EXACT_MATCH](args),
          common_options_b[COMMON_OPTION_CASE_SENSITIVE](args),
          common_options_b[COMMON_OPTION_DUPLICATE](args),
          common_options_b[COMMON_OPTION_NOT_DUPLICATE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "apps",
        .description = "List Applications",
        .function    = cmds[COMMAND_APPS].fxn,
        .about       = "🚥" "\t" COLOR_LIST "List Applications" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_APPLICATION_NAME](args),
          common_options_b[COMMON_OPTION_SORT_APP_KEYS](args),
          common_options_b[COMMON_OPTION_SORT_DIRECTION_ASC](args),
          common_options_b[COMMON_OPTION_SORT_DIRECTION_DESC](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_HOTKEYS].name,
        .description = cmds[COMMAND_HOTKEYS].description,
        .function    = cmds[COMMAND_HOTKEYS].fxn,
        .about       = get_command_about(COMMAND_HOTKEYS),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_RUN_HOTKEYS](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_SPACES].name,
        .description = cmds[COMMAND_SPACES].description,
        .function    = cmds[COMMAND_SPACES].fxn,
        .about       = get_command_about(COMMAND_SPACES),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_MOVE_WINDOW].name,
        .description = cmds[COMMAND_MOVE_WINDOW].description,
        .function    = cmds[COMMAND_MOVE_WINDOW].fxn,
        .about       = get_command_about(COMMAND_MOVE_WINDOW),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_ID](args),
          common_options_b[COMMON_OPTION_WINDOW_X](args),
          common_options_b[COMMON_OPTION_WINDOW_Y](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_RESIZE_WINDOW].name,
        .description = cmds[COMMAND_RESIZE_WINDOW].description,
        .function    = cmds[COMMAND_RESIZE_WINDOW].fxn,
        .about       = get_command_about(COMMAND_RESIZE_WINDOW),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_ID](args),
          common_options_b[COMMON_OPTION_RANDOM_ID](args),
          common_options_b[COMMON_OPTION_WINDOW_WIDTH](args),
          common_options_b[COMMON_OPTION_WINDOW_HEIGHT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_WINDOW_IS_MINIMIZED].name,
        .description = cmds[COMMAND_WINDOW_IS_MINIMIZED].description,
        .function    = cmds[COMMAND_WINDOW_IS_MINIMIZED].fxn,
        .about       = get_command_about(COMMAND_WINDOW_IS_MINIMIZED),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_WINDOW_ID_INFO].name,
        .description = cmds[COMMAND_WINDOW_ID_INFO].description,
        .function    = cmds[COMMAND_WINDOW_ID_INFO].fxn,
        .about       = get_command_about(COMMAND_WINDOW_ID_INFO),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_WINDOW_LEVEL].name,
        .description = cmds[COMMAND_WINDOW_LEVEL].description,
        .function    = cmds[COMMAND_WINDOW_LEVEL].fxn,
        .about       = get_command_about(COMMAND_WINDOW_LEVEL),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_WINDOW_LAYER].name,
        .description = cmds[COMMAND_WINDOW_LAYER].description,
        .function    = cmds[COMMAND_WINDOW_LAYER].fxn,
        .about       = get_command_about(COMMAND_WINDOW_LAYER),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_PID_IS_MINIMIZED].name,
        .description = cmds[COMMAND_PID_IS_MINIMIZED].description,
        .function    = cmds[COMMAND_PID_IS_MINIMIZED].fxn,
        .about       = get_command_about(COMMAND_PID_IS_MINIMIZED),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_PID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_MINIMIZE_WINDOW].name,
        .description = cmds[COMMAND_MINIMIZE_WINDOW].description,
        .function    = cmds[COMMAND_MINIMIZE_WINDOW].fxn,
        .about       = get_command_about(COMMAND_MINIMIZE_WINDOW),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_STICKY_WINDOW].name,
        .description = cmds[COMMAND_STICKY_WINDOW].description,
        .function    = cmds[COMMAND_STICKY_WINDOW].fxn,
        .about       = get_command_about(COMMAND_STICKY_WINDOW),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "set-window-all-spaces",
        .description = "Set Window All Spaces",
        .function    = cmds[COMMAND_SET_WINDOW_ALL_SPACES].fxn,
        .about       = "👽" "\t" COLOR_WINDOW "Set Window To All Spaces" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_COPY].name,
        .description = cmds[COMMAND_COPY].description,
        .function    = cmds[COMMAND_COPY].fxn,
        .about       = get_command_about(COMMAND_COPY),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_CONTENT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_PASTE].name,
        .description = cmds[COMMAND_PASTE].description,
        .function    = cmds[COMMAND_PASTE].fxn,
        .about       = get_command_about(COMMAND_PASTE),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_FOCUSED_WINDOW].name,
        .description = cmds[COMMAND_FOCUSED_WINDOW].description,
        .function    = cmds[COMMAND_FOCUSED_WINDOW].fxn,
        .about       = get_command_about(COMMAND_FOCUSED_WINDOW),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "set-window-space",
        .description = "Set Window Space",
        .function    = cmds[COMMAND_SET_WINDOW_SPACE].fxn,
        .about       = "💫" "\t" COLOR_WINDOW "Set Window Space" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_ID](args),
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "set-space-index",
        .description = "Set Space Index",
        .function    = cmds[COMMAND_SET_SPACE].fxn,
        .about       = "🍌" "\t" COLOR_SPACE "Set Active Space Index" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_SET_SPACE].name,
        .description = cmds[COMMAND_SET_SPACE].description,
        .function    = cmds[COMMAND_SET_SPACE].fxn,
        .about       = get_command_about(COMMAND_SET_SPACE),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_FOCUSED_SPACE].name,
        .description = cmds[COMMAND_FOCUSED_SPACE].description,
        .function    = cmds[COMMAND_FOCUSED_SPACE].fxn,
        .about       = get_command_about(COMMAND_FOCUSED_SPACE),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_FOCUSED_PID].name,
        .description = cmds[COMMAND_FOCUSED_PID].description,
        .function    = cmds[COMMAND_FOCUSED_PID].fxn,
        .about       = get_command_about(COMMAND_FOCUSED_PID),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "focused-server",
        .description = "Start Focused Server",
        .function    = cmds[COMMAND_FOCUSED_SERVER].fxn,
        .about       = "💾" "\t" COLOR_START "Start Focused Server" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "focused-client",
        .description = "Start Focused Client",
        .function    = cmds[COMMAND_FOCUSED_CLIENT].fxn,
        .about       = "🍏" "\t" COLOR_START "Start Focused Client" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "grayscale-png",
        .description = "Grayscale PNG",
        .function    = cmds[COMMAND_GRAYSCALE_PNG_FILE].fxn,
        .about       = "☁️" "\t" COLOR_GET "Grayscale PNG" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_INPUT_FILE](args),
          common_options_b[COMMON_OPTION_OUTPUT_FILE](args),
          common_options_b[COMMON_OPTION_RESIZE_FACTOR](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "get-icon-png",
        .description = "Get App Icon as PNG",
        .function    = cmds[COMMAND_SAVE_APP_ICON_PNG].fxn,
        .about       = "☀️" "\t" COLOR_ICNS "Get App Icon PNG" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_APPLICATION_PATH](args),
          common_options_b[COMMON_OPTION_OUTPUT_PNG_FILE](args),
          common_options_b[COMMON_OPTION_ICON_SIZE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "icon-info",
        .description = "Get Icon Info",
        .function    = cmds[COMMAND_ICON_INFO].fxn,
        .about       = "♦️" "\t" COLOR_ICNS "Get Icon Info" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_INPUT_ICNS_FILE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "get-icon",
        .description = "Get App Icon",
        .function    = cmds[COMMAND_SAVE_APP_ICON_ICNS].fxn,
        .about       = "♻️" "\t" COLOR_ICNS "Get App Icon" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_APPLICATION_PATH](args),
          common_options_b[COMMON_OPTION_OUTPUT_ICNS_FILE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "app-icns-file",
        .description = "Get App Icns File Path",
        .function    = cmds[COMMAND_APP_ICNS_PATH].fxn,
        .about       = "🎱" "\t" COLOR_ICNS "Get App Icns File Path" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_APPLICATION_PATH](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "info-plist",
        .description = "Get App Info Plist Path",
        .function    = cmds[COMMAND_APP_PLIST_INFO_PATH].fxn,
        .about       = "🐳" "\t" COLOR_ICNS "Get App Info Plist Path" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_APPLICATION_PATH](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "set-app-icon",
        .description = "Set App Icon from ICNS",
        .function    = cmds[COMMAND_SET_APP_ICON_ICNS].fxn,
        .about       = "🦄" "\t" COLOR_ICNS "Set App Icon from ICNS" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_APPLICATION_PATH](args),
          common_options_b[COMMON_OPTION_INPUT_ICNS_FILE](args),
          common_options_b[COMMON_OPTION_CLEAR_ICONS_CACHE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "clear-icons-cache",
        .description = "Clear Icons Cache",
        .function    = cmds[COMMAND_CLEAR_ICONS_CACHE].fxn,
        .about       = "🦄" "\t" COLOR_ICNS "Clear Icons Cache" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "parse-xml",
        .description = "Parse XML File",
        .function    = cmds[COMMAND_PARSE_XML_FILE].fxn,
        .about       = "🍉" "\t" COLOR_SET "Parse XML File" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_XML_FILE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "set-icon-png",
        .description = "Set App Icon From PNG",
        .function    = cmds[COMMAND_SET_APP_ICON_PNG].fxn,
        .about       = "🚦" "\t" COLOR_SET "Set App Icon From PNG" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_APPLICATION_PATH](args),
          common_options_b[COMMON_OPTION_INPUT_PNG_FILE](args),
          common_options_b[COMMON_OPTION_OUTPUT_PNG_FILE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "httpserver",
        .description = "Start HTTP Server",
        .function    = cmds[COMMAND_HTTPSERVER].fxn,
        .about       = "🇺" "\t" COLOR_START "Start HTTP Server" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          { END_OF_OPTIONS },
        },
      },
      { END_OF_SUBCOMMANDS },
    },
  };

  optparse_parse(&main_cmd, &argc, &argv);
  optparse_print_help();
  return(EXIT_SUCCESS);
} /* main */
static void *dls_print_arg_v(char *title, char *color, int argc, char *argv[]){
  printf(AC_GREEN "%s\n" AC_RESETALL,title);
  for(int i=0;i<argc;i++){
    printf("#%d> %s%s"AC_RESETALL "\n", i, color,argv[i]);
  }
  char *cmd = stringfn_join(argv," ",0,argc);
  printf("\t\t\t%s%s"AC_RESETALL "\n", color,cmd);
  free(cmd);

}
static struct normalized_argv_t *dls_argv_normalized_argv_t(int argc, char *argv[]){
  struct normalized_argv_t *r = calloc(1,sizeof(struct normalized_argv_t));
  r->arg_v = dls_argv_to_arg_v(argc,argv);
  char *cur = NULL;
  struct Vector *prev_v = vector_new();
  for(size_t i=0;i<vector_size(r->arg_v);i++){
    cur = (char*)vector_get(r->arg_v,i);
  log_info("%lu", vector_size(prev_v));
    if(!cur)continue;
    if(i==0){
        r->executable = cur;
    }
    for(size_t x=0;x<vector_size(prev_v);x++){
      char *prev = (x>0) ? (char*)vector_get(prev_v,x-1) : NULL;
      Dbg(prev,%s);
      if(prev && dls_get_arg_is_ordered(cur,prev)){
        log_info("#%lu> args need to be swapped: %s should be after %s",i, prev,cur);
        vector_set(r->arg_v,x,strdup(cur));
        vector_set(r->arg_v,i,strdup(prev));
      }
    }
    vector_push(prev_v,(void*)cur);
    if(i > 0 && !r->mode && !stringfn_starts_with(cur,"-")){
      char *aliased = dls_get_alias_wildcard_glob_name(cur);
      if(aliased){
        r->mode = strdup(aliased);
        vector_set(r->arg_v,i,(void*)r->mode);
        argv[i] = r->mode;
      }else{
        r->mode = cur;
      }
    }else{
      if(!r->mode)
        vector_push(r->pre_mode_arg_v,cur);
      else
        vector_push(r->post_mode_arg_v,cur);
    }
  }
  for(size_t i=0;i<vector_size(r->arg_v);i++){
    char *s = (char*)vector_get(r->arg_v,i);
    if(strcmp(argv[i],s) != 0)
      argv[i] = s;
  }
  return(r);
}
static struct Vector *dls_argv_to_arg_v(int argc, char *argv[]){
  struct Vector *arg_v = vector_new();
  for(int i=0;i<argc;i++){
    vector_push(arg_v,(void*)strdup(argv[i]));
  }
  return(arg_v);
}
static bool dls_normalize_arguments(int *argc, char *argv[]){
  struct normalized_argv_t *r = dls_argv_normalized_argv_t(*argc,argv);
  log_info("Mode: %s", r->mode);
  *argc = vector_size(r->arg_v);
}
static void __attribute__((constructor)) __constructor__dls(void){
  return;
  if (getenv("DEBUG") != NULL || getenv("DEBUG_DARWIN_LS") != NULL) {
    DARWIN_LS_DEBUG_MODE = true;
  }
  errno    = 0;
  whereami = core_utils_whereami_report();
  if (whereami) {
    if (DARWIN_LS_DEBUG_MODE) {
      log_debug("  >Whereami Report<\n\tExecutable:%s\n\tDirectory:%s\n\tBasename:%s\n",
                whereami->executable, whereami->executable_directory, whereami->executable_basename
                );
    }
  }else{
    log_error("Whereami Error");
  }
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
}
