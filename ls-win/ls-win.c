#include "ls-win/ls-win-commands.h"
#include "ls-win/ls-win.h"
////////////////////////////////////////////
const enum output_mode_type_t DEFAULT_OUTPUT_MODE = OUTPUT_MODE_TABLE;
struct args_t                 *args               = &(struct args_t){
  .verbose            = false,
  .current_space_only = false,
  .space_id           = 0,
  .window_id          = 0,
  .output_mode        = DEFAULT_OUTPUT_MODE,
};

////////////////////////////////////////////
int main(int argc, char **argv) {
  assert(is_authorized_for_accessibility() == true);
  args->window_id = 123;//get_focused_window_id();
  struct optparse_cmd main_cmd = {
    .about       = "ls-win v1.00 - List Darwin Items.",
    .description = "This program lists Darwin Items.",
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
        .short_name    = 'm',
        .long_name     = "mode",
        .description   = "Output Mode.",
        .arg_name      = "OUTPUT-MODE",
        .arg_data_type = check_cmds[CHECK_COMMAND_OUTPUT_MODE].arg_data_type,
        .function      = check_cmds[CHECK_COMMAND_OUTPUT_MODE].fxn,
        .arg_dest      = &(args->output_mode_s),
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
        .about       = "❔" "\t" COLOR_HELP "Command Help" AC_RESETALL,
        .function    = optparse_print_help_subcmd,
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
        .about       = "🦠" "\t" COLOR_WINDOW "Set Window To All Spaces" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_WINDOW_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "set-window-space",
        .description = "Set Window Space",
        .function    = cmds[COMMAND_SET_WINDOW_SPACE].fxn,
        .about       = "⛰" "\t" COLOR_WINDOW "Set Window Space" AC_RESETALL,
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
        .about       = "🦜" "\t" COLOR_SPACE "Set Active Space Index" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "set-space",
        .description = "Set Space",
        .function    = cmds[COMMAND_SET_SPACE].fxn,
        .about       = "🦜" "\t" COLOR_SPACE "Set Active Space" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          common_options_b[COMMON_OPTION_SPACE_ID](args),
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
        .about       = "⚙" "\t" COLOR_LIST "List Windows" AC_RESETALL,
        .options     = (struct optparse_opt[]){
          {
            .short_name  = 'c',
            .long_name   = "current-space",
            .description = "Windows in current space only",
            .flag_type   = FLAG_TYPE_SET_TRUE,
            .flag        = &(args->current_space_only),
          },
          common_options_b[COMMON_OPTION_SPACE_ID](args),
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "spaces",
        .description = "List Spaces",
        .function    = cmds[COMMAND_SPACES].fxn,
        .about       = "💊" "\t" COLOR_LIST "List Spaces" AC_RESETALL,
      },
      {
        .name        = "focused-space",
        .description = "Show Focused Space",
        .function    = cmds[COMMAND_FOCUSED_SPACE].fxn,
        .about       = "🍕" "\t" COLOR_SHOW "Focused Space" AC_RESETALL,
      },
      {
        .name        = "focused-pid",
        .description = "Show Focused PID",
        .function    = cmds[COMMAND_FOCUSED_PID].fxn,
        .about       = "🌈" "\t" COLOR_SHOW "Focused PID" AC_RESETALL,
      },
      {
        .name        = "focused-window",
        .description = "Show Focused Window",
        .function    = cmds[COMMAND_FOCUSED_WINDOW].fxn,
        .about       = "🗯" "\t" COLOR_SHOW "Focused Window" AC_RESETALL,
      },
      {
        .name        = "dock",
        .description = "Dock Info",
        .function    = cmds[COMMAND_DOCK].fxn,
        .about       = "📡" "\t" COLOR_INFO "Dock Info" AC_RESETALL,
      },
      {
        .name        = "menu-bar",
        .description = "Menu Bar Info",
        .function    = cmds[COMMAND_MENU_BAR].fxn,
        .about       = "☘" "\t" COLOR_INFO "Menu Bar Info" AC_RESETALL,
      },
      {
        .name        = "focused-server",
        .description = "Focused Server",
        .function    = cmds[COMMAND_FOCUSED_SERVER].fxn,
        .about       = "⭐" "\t" COLOR_START "Focused Server" AC_RESETALL,
      },
      {
        .name        = "focused-client",
        .description = "Focused Client",
        .function    = cmds[COMMAND_FOCUSED_CLIENT].fxn,
        .about       = "⭐" "\t" COLOR_START "Focused Client" AC_RESETALL,
      },
      {
        .name        = "args",
        .description = "Debug Args",
        .function    = cmds[COMMAND_DEBUG_ARGS].fxn,
        .about       = "🐛" "\t" COLOR_DEBUG "Debug Args" AC_RESETALL,
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
