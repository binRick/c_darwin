#include "dls/dls.h"
static void __attribute__((constructor)) __constructor__dls(void);
const enum output_mode_type_t DEFAULT_OUTPUT_MODE = OUTPUT_MODE_TABLE;
struct args_t                 *args               = &(struct args_t){
  .verbose            = false,
  .space_id           = 0,
  .window_id          = 0,
  .output_mode        = DEFAULT_OUTPUT_MODE,
  .sort_key           = "pid",
  .sort_direction     = "asc",
  .current_space_only = false,
  .clear_screen       = false,
  .minimized_only     = false,
  .pid                = -1,
  .application_name   = NULL,
  .width_greater      = -1, .width_less = -1,
  .height_greater     = -1, .height_less = -1,
  .width              = -1, .height = -1,
  .output_file        = "output.png",
  .concurrency        = 1,
  .limit              = -1,
  .font_name          = NULL, .font_family = NULL, .font_style = NULL,
  .exact_match        = false, .case_sensitive = false,
  .retries            = 0,
};

////////////////////////////////////////////
int main(int argc, char **argv) {
  struct optparse_cmd main_cmd = {
    .about       = "dls v1.00 - List Darwin Objects",
    .description = "This program lists Darwin Objects",
    .name        = "dls",
    .operands    = "[COMMAND...]",
    .options     = (struct optparse_opt[]) {
      common_options_b[COMMON_OPTION_HELP](args),
      common_options_b[COMMON_OPTION_VERBOSE](args),
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
      {
        .name        = cmds[COMMAND_WINDOWS].name,
        .description = cmds[COMMAND_WINDOWS].description,
        .function    = cmds[COMMAND_WINDOWS].fxn,
        .about       = get_command_about(COMMAND_WINDOWS),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_CURRENT_SPACE](args),
          common_options_b[COMMON_OPTION_CURRENT_DISPLAY](args),
          common_options_b[COMMON_OPTION_PID](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          common_options_b[COMMON_OPTION_DISPLAY_ID](args),
          common_options_b[COMMON_OPTION_APPLICATION_NAME](args),
          common_options_b[COMMON_OPTION_SORT_WINDOW_KEYS](args),
          common_options_b[COMMON_OPTION_SORT_DIRECTION](args),
          common_options_b[COMMON_OPTION_MINIMIZED](args),
          common_options_b[COMMON_OPTION_NON_MINIMIZED](args),
          common_options_b[COMMON_OPTION_WIDTH](args),
          common_options_b[COMMON_OPTION_HEIGHT](args),
          common_options_b[COMMON_OPTION_HEIGHT_GREATER](args),
          common_options_b[COMMON_OPTION_HEIGHT_LESS](args),
          common_options_b[COMMON_OPTION_WIDTH_GREATER](args),
          common_options_b[COMMON_OPTION_WIDTH_LESS](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "extract-window",
        .description = "Extract Window",
        .function    = cmds[COMMAND_EXTRACT_WINDOW].fxn,
        .about       = "🙀" "\t" COLOR_CAPTURE "Extract Window Screenshot" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_CONCURRENCY](args),
          common_options_b[COMMON_OPTION_ALL_WINDOWS](args),
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          common_options_b[COMMON_OPTION_RANDOM_WINDOW_ID](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "capture-window",
        .description = "Capture Window",
        .function    = cmds[COMMAND_CAPTURE_WINDOW].fxn,
        .about       = "🙀" "\t" COLOR_CAPTURE "Capture Window Screenshot" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          common_options_b[COMMON_OPTION_CURRENT_SPACE](args),
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          common_options_b[COMMON_OPTION_ALL_WINDOWS](args),
          common_options_b[COMMON_OPTION_PID](args),
          common_options_b[COMMON_OPTION_OUTPUT_FILE](args),
          common_options_b[COMMON_OPTION_DISPLAY_OUTPUT_FILE](args),
          common_options_b[COMMON_OPTION_RANDOM_WINDOW_ID](args),
          common_options_b[COMMON_OPTION_CLEAR_SCREEN](args),
          common_options_b[COMMON_OPTION_WINDOW_WIDTH_GROUP](args),
          common_options_b[COMMON_OPTION_WINDOW_HEIGHT_GROUP](args),
          common_options_b[COMMON_OPTION_APPLICATION_NAME](args),
          common_options_b[COMMON_OPTION_DISPLAY_ID](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_FOCUS_WINDOW].name,
        .description = cmds[COMMAND_FOCUS_WINDOW].description,
        .function    = cmds[COMMAND_FOCUS_WINDOW].fxn,
        .about       = get_command_about(COMMAND_FOCUS_WINDOW),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          common_options_b[COMMON_OPTION_RANDOM_WINDOW_ID](args),
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
      },
      {
        .name        = cmds[COMMAND_DOCK].name,
        .description = cmds[COMMAND_DOCK].description,
        .function    = cmds[COMMAND_DOCK].fxn,
        .about       = get_command_about(COMMAND_DOCK),
      },
      {
        .name        = cmds[COMMAND_DISPLAYS].name,
        .description = cmds[COMMAND_DISPLAYS].description,
        .function    = cmds[COMMAND_DISPLAYS].fxn,
        .about       = get_command_about(COMMAND_DISPLAYS),
        .options     = (struct optparse_opt[]){
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
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "usb",
        .description = "USB Devices",
        .function    = cmds[COMMAND_USB_DEVICES].fxn,
        .about       = "📡" "\t" COLOR_LIST "List USB Devices" AC_RESETALL,
        .options     = (struct optparse_opt[]){
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
          common_options_b[COMMON_OPTION_LIMIT](args),
          common_options_b[COMMON_OPTION_FONT_NAME](args),
          common_options_b[COMMON_OPTION_FONT_FAMILY](args),
          common_options_b[COMMON_OPTION_FONT_TYPE](args),
          common_options_b[COMMON_OPTION_FONT_STYLE](args),
          common_options_b[COMMON_OPTION_SORT_FONT_KEYS](args),
          common_options_b[COMMON_OPTION_SORT_DIRECTION](args),
          common_options_b[COMMON_OPTION_EXACT_MATCH](args),
          common_options_b[COMMON_OPTION_CASE_SENSITIVE](args),
          common_options_b[COMMON_OPTION_DUPLICATE](args),
          common_options_b[COMMON_OPTION_NON_DUPLICATE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "apps",
        .description = "List Applications",
        .function    = cmds[COMMAND_APPS].fxn,
        .about       = "🚥" "\t" COLOR_LIST "List Applications" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_APPLICATION_NAME](args),
          common_options_b[COMMON_OPTION_SORT_APP_KEYS](args),
          common_options_b[COMMON_OPTION_SORT_DIRECTION](args),
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_LIST_HOTKEYS].name,
        .description = cmds[COMMAND_LIST_HOTKEYS].description,
        .function    = cmds[COMMAND_LIST_HOTKEYS].fxn,
        .about       = get_command_about(COMMAND_LIST_HOTKEYS),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_LIMIT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_HOTKEYS].name,
        .description = cmds[COMMAND_HOTKEYS].description,
        .function    = cmds[COMMAND_HOTKEYS].fxn,
        .about       = get_command_about(COMMAND_HOTKEYS),
      },
      {
        .name        = cmds[COMMAND_SPACES].name,
        .description = cmds[COMMAND_SPACES].description,
        .function    = cmds[COMMAND_SPACES].fxn,
        .about       = get_command_about(COMMAND_SPACES),
        .options     = (struct optparse_opt[]){
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
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
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
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          common_options_b[COMMON_OPTION_RANDOM_WINDOW_ID](args),
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
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_WINDOW_ID_INFO].name,
        .description = cmds[COMMAND_WINDOW_ID_INFO].description,
        .function    = cmds[COMMAND_WINDOW_ID_INFO].fxn,
        .about       = get_command_about(COMMAND_WINDOW_ID_INFO),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_WINDOW_LEVEL].name,
        .description = cmds[COMMAND_WINDOW_LEVEL].description,
        .function    = cmds[COMMAND_WINDOW_LEVEL].fxn,
        .about       = get_command_about(COMMAND_WINDOW_LEVEL),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_WINDOW_LAYER].name,
        .description = cmds[COMMAND_WINDOW_LAYER].description,
        .function    = cmds[COMMAND_WINDOW_LAYER].fxn,
        .about       = get_command_about(COMMAND_WINDOW_LAYER),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_PID_IS_MINIMIZED].name,
        .description = cmds[COMMAND_PID_IS_MINIMIZED].description,
        .function    = cmds[COMMAND_PID_IS_MINIMIZED].fxn,
        .about       = get_command_about(COMMAND_PID_IS_MINIMIZED),
        .options     = (struct optparse_opt[]){
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
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_STICKY_WINDOW].name,
        .description = cmds[COMMAND_STICKY_WINDOW].description,
        .function    = cmds[COMMAND_STICKY_WINDOW].fxn,
        .about       = get_command_about(COMMAND_STICKY_WINDOW),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "set-window-all-spaces",
        .description = "Set Window All Spaces",
        .function    = cmds[COMMAND_SET_WINDOW_ALL_SPACES].fxn,
        .about       = "👽" "\t" COLOR_WINDOW "Set Window To All Spaces" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_COPY].name,
        .description = cmds[COMMAND_COPY].description,
        .function    = cmds[COMMAND_COPY].fxn,
        .about       = get_command_about(COMMAND_COPY),
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_CONTENT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_PASTE].name,
        .description = cmds[COMMAND_PASTE].description,
        .function    = cmds[COMMAND_PASTE].fxn,
        .about       = get_command_about(COMMAND_PASTE),
      },
      {
        .name        = cmds[COMMAND_FOCUSED_WINDOW].name,
        .description = cmds[COMMAND_FOCUSED_WINDOW].description,
        .function    = cmds[COMMAND_FOCUSED_WINDOW].fxn,
        .about       = get_command_about(COMMAND_FOCUSED_WINDOW),
      },
      {
        .name        = "set-window-space",
        .description = "Set Window Space",
        .function    = cmds[COMMAND_SET_WINDOW_SPACE].fxn,
        .about       = "💫" "\t" COLOR_WINDOW "Set Window Space" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
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
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = cmds[COMMAND_FOCUSED_SPACE].name,
        .description = cmds[COMMAND_FOCUSED_SPACE].description,
        .function    = cmds[COMMAND_FOCUSED_SPACE].fxn,
        .about       = get_command_about(COMMAND_FOCUSED_SPACE),
      },
      {
        .name        = cmds[COMMAND_FOCUSED_PID].name,
        .description = cmds[COMMAND_FOCUSED_PID].description,
        .function    = cmds[COMMAND_FOCUSED_PID].fxn,
        .about       = get_command_about(COMMAND_FOCUSED_PID),
      },
      {
        .name        = "focused-server",
        .description = "Start Focused Server",
        .function    = cmds[COMMAND_FOCUSED_SERVER].fxn,
        .about       = "💾" "\t" COLOR_START "Start Focused Server" AC_RESETALL,
      },
      {
        .name        = "focused-client",
        .description = "Start Focused Client",
        .function    = cmds[COMMAND_FOCUSED_CLIENT].fxn,
        .about       = "🍏" "\t" COLOR_START "Start Focused Client" AC_RESETALL,
      },
      {
        .name        = "grayscale-png",
        .description = "Grayscale PNG",
        .function    = cmds[COMMAND_GRAYSCALE_PNG_FILE].fxn,
        .about       = "☁️" "\t" COLOR_GET "Grayscale PNG" AC_RESETALL,
        .options     = (struct optparse_opt[]){
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
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "parse-xml",
        .description = "Parse XML File",
        .function    = cmds[COMMAND_PARSE_XML_FILE].fxn,
        .about       = "🍉" "\t" COLOR_SET "Parse XML File" AC_RESETALL,
        .options     = (struct optparse_opt[]){
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
          common_options_b[COMMON_OPTION_APPLICATION_PATH](args),
          common_options_b[COMMON_OPTION_INPUT_PNG_FILE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "httpserver",
        .description = "Start HTTP Server",
        .function    = cmds[COMMAND_HTTPSERVER].fxn,
        .about       = "🇺" "\t" COLOR_START "Start HTTP Server" AC_RESETALL,
      },
      { END_OF_SUBCOMMANDS },
    },
  };

  optparse_parse(&main_cmd, &argc, &argv);
  optparse_print_help();
  return(EXIT_SUCCESS);
} /* main */
static void __attribute__((constructor)) __constructor__dls(void){
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
