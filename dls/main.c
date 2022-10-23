#include "dls/types.h"
#include "dls/vars.h"
#include "dls/static.c"
int main(int argc, char *argv[]) {
  VIPS_INIT(argv[0]);
  /*
   * signal(SIGINT, __at_exit);
   * signal(SIGTERM, __at_exit);
   * signal(SIGQUIT, __at_exit);
   * atexit(__at_exit);
   */
  args->pid = getpid();
  if (DEBUG_ARGV) {
    dls_print_arg_v("pre", AC_YELLOW, argc, argv);
  }
  if (NORMALIZE_ARGV) {
    dls_normalize_arguments(&argc, argv);
  }
  if (DEBUG_ARGV) {
    dls_print_arg_v("post", AC_MAGENTA, argc, argv);
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
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
      CREATE_SUBCOMMAND(WINDOWS, ),
      CREATE_SUBCOMMAND(PROCESSES, ),
      CREATE_SUBCOMMAND(FOCUS, ),
      CREATE_SUBCOMMAND(FOCUSED, ),
      CREATE_SUBCOMMAND(DISPLAYS, ),
      CREATE_SUBCOMMAND(DOCK, ),
      CREATE_SUBCOMMAND(MENU_BAR, ),
      CREATE_SUBCOMMAND(DB, SUBCOMMANDS_DB),
      CREATE_SUBCOMMAND(CLIPBOARD, SUBCOMMANDS_CLIPBOARD),
      CREATE_SUBCOMMAND(LAYOUT, SUBCOMMANDS_LAYOUT),
      CREATE_SUBCOMMAND(HOTKEYS, SUBCOMMANDS_HOTKEYS),
      CREATE_SUBCOMMAND(WINDOW, SUBCOMMANDS_WINDOW),
      CREATE_SUBCOMMAND(SPACE, SUBCOMMANDS_SPACE),
      CREATE_SUBCOMMAND(ICON, SUBCOMMANDS_ICON),
      CREATE_SUBCOMMAND(CAPTURE, SUBCOMMANDS_CAPTURE),
      CREATE_SUBCOMMAND(ANIMATE, SUBCOMMANDS_ANIMATE),
      CREATE_SUBCOMMAND(EXTRACT, SUBCOMMANDS_EXTRACT),
#undef CREATE_SUBCOMMAND
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
          common_options_b[COMMON_OPTION_SPACE_ID](args),
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
      },/*
         * {
         * .name        = "focused-server",
         * .description = "Start Focused Server",
         * .function    = cmds[COMMAND_FOCUSED_SERVER].fxn,
         * .about       = "💾" "\t" COLOR_START "Start Focused Server" AC_RESETALL,
         * .options     = (struct optparse_opt[]){
         * common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
         * { END_OF_OPTIONS },
         * },
         * },
         * {
         * .name        = "focused-client",
         * .description = "Start Focused Client",
         * .function    = cmds[COMMAND_FOCUSED_CLIENT].fxn,
         * .about       = "🍏" "\t" COLOR_START "Start Focused Client" AC_RESETALL,
         * .options     = (struct optparse_opt[]){
         * common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
         * { END_OF_OPTIONS },
         * },
         * },
         * {
         * .name        = "grayscale-png",
         * .description = "Grayscale PNG",
         * .function    = cmds[COMMAND_GRAYSCALE_PNG_FILE].fxn,
         * .about       = "☁️" "\t" COLOR_GET "Grayscale PNG" AC_RESETALL,
         * .options     = (struct optparse_opt[]){
         * common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
         * common_options_b[COMMON_OPTION_INPUT_FILE](args),
         * common_options_b[COMMON_OPTION_OUTPUT_FILE](args),
         * common_options_b[COMMON_OPTION_RESIZE_FACTOR](args),
         * { END_OF_OPTIONS },
         * },
         * },*/
      {
        .name        = "get-icon-png",
        .description = "Get App Icon as PNG",
        .function    = cmds[COMMAND_SAVE_APP_ICON_PNG].fxn,
        .about       = "☀️" "\t" COLOR_ICNS "Get App Icon PNG" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_HELP_SUBCMD](args),
          common_options_b[COMMON_OPTION_APPLICATION_PATH](args),
          common_options_b[COMMON_OPTION_OUTPUT_PNG_FILE](args),
          common_options_b[COMMON_OPTION_ICON_SIZES](args),
          common_options_b[COMMON_OPTION_DISPLAY_OUTPUT_FILE](args),
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
#pragma GCC diagnostic pop
  optparse_parse(&main_cmd, &argc, &argv);
  optparse_print_help();
  return(EXIT_FAILURE);
} /* main */
