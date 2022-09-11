#include "darwin-ls/darwin-ls-commands.h"
#include "darwin-ls/darwin-ls.h"
#include "memory_leak_detector/leak.h"
////////////////////////////////////////////
const enum output_mode_type_t DEFAULT_OUTPUT_MODE = OUTPUT_MODE_TABLE;
struct args_t                 *args               = &(struct args_t){
  .verbose            = false,
  .current_space_only = false,
  .space_id           = 0,
  .window_id          = 0,
  .output_mode        = DEFAULT_OUTPUT_MODE,
};

char *common_option_width_or_height_name(enum common_option_width_or_height_t width_or_height){
  switch (width_or_height) {
  case COMMON_OPTION_WIDTH_OR_HEIGHT_HEIGHT: return("height"); break;
  case COMMON_OPTION_WIDTH_OR_HEIGHT_WIDTH: return("width"); break;
  default: return("UNKNOWN"); break;
  }
}

////////////////////////////////////////////
int main(int argc, char **argv) {
  assert(is_authorized_for_accessibility() == true);
  args->window_id = 123;//get_focused_window_id();
  struct optparse_cmd main_cmd = {
    .about       = "darwin-ls v1.00 - List Darwin Objects",
    .description = "This program lists Darwin Objects",
    .name        = "darwin-ls",
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
        .name        = "capture-window",
        .description = "Capture Window",
        .function    = cmds[COMMAND_CAPTURE_WINDOW].fxn,
        .about       = "🙀" "\t" COLOR_CAPTURE "Capture Window Screenshot" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          common_options_b[COMMON_OPTION_OUTPUT_FILE](args),
          common_options_b[COMMON_OPTION_WINDOW_WIDTH_GROUP](args),
          common_options_b[COMMON_OPTION_WINDOW_HEIGHT_GROUP](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "displays",
        .description = "List Displays",
        .function    = cmds[COMMAND_DISPLAYS].fxn,
        .about       = "🐾" "\t" COLOR_LIST "List Displays" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "windows",
        .description = "List Windows",
        .function    = cmds[COMMAND_WINDOWS].fxn,
        .about       = "🥑" "\t" COLOR_LIST "List Windows" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_CURRENT_SPACE](args),
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "alacrittys",
        .description = "Alacritty Terminals",
        .function    = cmds[COMMAND_ALACRITTYS].fxn,
        .about       = "♻️" "\t" COLOR_LIST "List Alacrittys" AC_RESETALL,
      },
      {
        .name        = "kittys",
        .description = "Kitty Terminals",
        .function    = cmds[COMMAND_KITTYS].fxn,
        .about       = "💤" "\t" COLOR_LIST "List Kittys" AC_RESETALL,
      },
      {
        .name        = "processes",
        .description = "Processes",
        .function    = cmds[COMMAND_PROCESSES].fxn,
        .about       = "💒" "\t" COLOR_LIST "List Processes" AC_RESETALL,
      },
      {
        .name        = "monitors",
        .description = "Monitors",
        .function    = cmds[COMMAND_MONITORS].fxn,
        .about       = "💮" "\t" COLOR_LIST "List Monitors" AC_RESETALL,
      },
      {
        .name        = "usb",
        .description = "USB Devices",
        .function    = cmds[COMMAND_USB_DEVICES].fxn,
        .about       = "📡" "\t" COLOR_LIST "List USB Devices" AC_RESETALL,
      },
      {
        .name        = "fonts",
        .description = "List Fonts",
        .function    = cmds[COMMAND_FONTS].fxn,
        .about       = "🚥" "\t" COLOR_LIST "List FONTS" AC_RESETALL,
      },
      {
        .name        = "apps",
        .description = "List Applications",
        .function    = cmds[COMMAND_APPS].fxn,
        .about       = "🚥" "\t" COLOR_LIST "List Applications" AC_RESETALL,
      },
      {
        .name        = "spaces",
        .description = "List Spaces",
        .function    = cmds[COMMAND_SPACES].fxn,
        .about       = "💊" "\t" COLOR_LIST "List Spaces" AC_RESETALL,
      },
      {
        .name        = "move-window",
        .description = "Move Window",
        .function    = cmds[COMMAND_MOVE_WINDOW].fxn,
        .about       = "🍏" "\t" COLOR_MOVE "Move Window" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          common_options_b[COMMON_OPTION_WINDOW_X](args),
          common_options_b[COMMON_OPTION_WINDOW_Y](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "resize-window",
        .description = "Resize Window",
        .function    = cmds[COMMAND_RESIZE_WINDOW].fxn,
        .about       = "💡" "\t" COLOR_WINDOW "Resize Window" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          common_options_b[COMMON_OPTION_WINDOW_WIDTH](args),
          common_options_b[COMMON_OPTION_WINDOW_HEIGHT](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "focus-window",
        .description = "Focus Window",
        .function    = cmds[COMMAND_FOCUS_WINDOW].fxn,
        .about       = "🔅" "\t" COLOR_WINDOW "Focus Window" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "sticky-window",
        .description = "Sticky Window",
        .function    = cmds[COMMAND_STICKY_WINDOW].fxn,
        .about       = "🕰" "\t" COLOR_WINDOW "Set Window Sticky" AC_RESETALL,
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
        .name        = "set-space",
        .description = "Set Space",
        .function    = cmds[COMMAND_SET_SPACE].fxn,
        .about       = "💣" "\t" COLOR_SPACE "Set Active Space" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "focused-space",
        .description = "Show Focused Space",
        .function    = cmds[COMMAND_FOCUSED_SPACE].fxn,
        .about       = "🍕" "\t" COLOR_SHOW "Show Focused Space" AC_RESETALL,
      },
      {
        .name        = "focused-pid",
        .description = "Show Focused PID",
        .function    = cmds[COMMAND_FOCUSED_PID].fxn,
        .about       = "🌈" "\t" COLOR_SHOW "Show Focused PID" AC_RESETALL,
      },
      {
        .name        = "focused-window",
        .description = "Show Focused Window",
        .function    = cmds[COMMAND_FOCUSED_WINDOW].fxn,
        .about       = "🗯" "\t" COLOR_SHOW "Show Focused Window" AC_RESETALL,
      },
      {
        .name        = "dock",
        .description = "Dock Info",
        .function    = cmds[COMMAND_DOCK].fxn,
        .about       = "📡" "\t" COLOR_SHOW "Show Dock Info" AC_RESETALL,
      },
      {
        .name        = "menu-bar",
        .description = "Menu Bar Info",
        .function    = cmds[COMMAND_MENU_BAR].fxn,
        .about       = "📀" "\t" COLOR_SHOW "Show Menu Bar Info" AC_RESETALL,
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
        .name        = "get-icon-png",
        .description = "Get Application Icon as PNG",
        .function    = cmds[COMMAND_SAVE_APP_ICON_PNG].fxn,
        .about       = "🪐" "\t" COLOR_GET "Get Application Icon PNG" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_APPLICATION_PATH](args),
          common_options_b[COMMON_OPTION_OUTPUT_PNG_FILE](args),
          common_options_b[COMMON_OPTION_ICON_SIZE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "get-icon",
        .description = "Get Application Icon",
        .function    = cmds[COMMAND_SAVE_APP_ICON_ICNS].fxn,
        .about       = "🪐" "\t" COLOR_GET "Get Application Icon" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_APPLICATION_PATH](args),
          common_options_b[COMMON_OPTION_OUTPUT_ICNS_FILE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "set-icon",
        .description = "Set Application Icon",
        .function    = cmds[COMMAND_SET_APP_ICON_ICNS].fxn,
        .about       = "🪐" "\t" COLOR_SET "Set Application Icon" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_APPLICATION_PATH](args),
          common_options_b[COMMON_OPTION_INPUT_ICNS_FILE](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "httpserver",
        .description = "Start HTTP Server",
        .function    = cmds[COMMAND_HTTPSERVER].fxn,
        .about       = "🪐" "\t" COLOR_START "Start HTTP Server" AC_RESETALL,
      },
      {
        .name        = "args",
        .description = "Debug Arguments",
        .function    = cmds[COMMAND_DEBUG_ARGS].fxn,
        .about       = "🐛" "\t" COLOR_DEBUG "Debug Arguments" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          { END_OF_OPTIONS },
        },
      },
      { END_OF_SUBCOMMANDS },
    },
  };
  optparse_parse(&main_cmd, &argc, &argv);
  optparse_print_help();
  return(0);
} /* main */
