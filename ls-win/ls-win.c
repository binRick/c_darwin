#include "optparse99/optparse99.h"
////////////////////////////////////////////
#include "ls-win/ls-win.h"
////////////////////////////////////////////
#include "ls-win/ls-win-commands.c"

////////////////////////////////////////////
void _check_window_id(uint16_t window_id){
  struct window_t *w = get_window_id((size_t)window_id);

  if (w == NULL || w->window_id != (size_t)window_id) {
    log_error("Invalid Window ID %lu", (size_t)window_id);
    exit(EXIT_FAILURE);
  }
  free(w);
}

int main(int argc, char **argv) {
  assert(is_authorized_for_accessibility() == true);
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
        .name        = "move-window",
        .description = "Move Window",
        .function    = cmds[COMMAND_MOVE_WINDOW].fxn,
        .about       = "Move Window",
        .options     = (struct optparse_opt[]){
          {
            .short_name    = 'w',
            .long_name     = "window-id",
            .description   = "Window ID",
            .arg_name      = "WINDOW-ID",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->window_id),
            .function      = (void (*)(void)) _check_window_id,
          },
          {
            .short_name    = 'x',
            .long_name     = "window-x",
            .description   = "Window X",
            .arg_name      = "WINDOW-X",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->x),
          },
          {
            .short_name    = 'y',
            .long_name     = "window-y",
            .description   = "Window y",
            .arg_name      = "WINDOW-Y",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->y),
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "resize-window",
        .description = "Resize Window",
        .function    = cmds[COMMAND_RESIZE_WINDOW].fxn,
        .about       = "Resize Window",
        .options     = (struct optparse_opt[]){
          {
            .short_name    = 'w',
            .long_name     = "window-id",
            .description   = "Window ID",
            .arg_name      = "WINDOW-ID",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->window_id),
            .function      = (void (*)(void)) _check_window_id,
          },
          {
            .short_name    = 'W',
            .long_name     = "window-width",
            .description   = "Window Width",
            .arg_name      = "WINDOW-WIDTH",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->width),
          },
          {
            .short_name    = 'H',
            .long_name     = "window-height",
            .description   = "Window Height",
            .arg_name      = "WINDOW-HEIGHT",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->height),
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "start-focused",
        .description = "Start Focused",
        .function    = cmds[COMMAND_FOCUSED_START].fxn,
        .about       = "Start Focused",
        .options     = (struct optparse_opt[]){
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "dock",
        .description = "Dock Info",
        .function    = cmds[COMMAND_DOCK].fxn,
        .about       = "Dock Info",
      },
      {
        .name        = "menu-bar",
        .description = "Menu Bar Info",
        .function    = cmds[COMMAND_MENU_BAR].fxn,
        .about       = "Menu Bar Info",
      },
      {
        .name        = "sticky-window",
        .description = "Sticky Window",
        .function    = cmds[COMMAND_STICKY_WINDOW].fxn,
        .about       = "Sticky Window",
        .options     = (struct optparse_opt[]){
          {
            .short_name    = 'w',
            .long_name     = "window-id",
            .description   = "window id",
            .arg_name      = "WINDOW-ID",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->window_id),
            .function      = (void (*)(void)) _check_window_id,
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "focus-window",
        .description = "Focus Window",
        .function    = cmds[COMMAND_FOCUS_WINDOW].fxn,
        .about       = "Focus Window",
        .options     = (struct optparse_opt[]){
          {
            .short_name    = 'w',
            .long_name     = "window-id",
            .description   = "window id",
            .arg_name      = "WINDOW-ID",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->window_id),
            .function      = (void (*)(void)) _check_window_id,
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "set-window-all-spaces",
        .description = "Set Window All Spaces",
        .function    = cmds[COMMAND_SET_WINDOW_ALL_SPACES].fxn,
        .about       = "Set Window To All Spaces",
        .options     = (struct optparse_opt[]){
          {
            .short_name    = 'w',
            .long_name     = "window-id",
            .description   = "window id",
            .arg_name      = "WINDOW-ID",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->window_id),
            .function      = (void (*)(void)) _check_window_id,
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "set-window-space",
        .description = "Set Window Space",
        .function    = cmds[COMMAND_SET_WINDOW_SPACE].fxn,
        .about       = "Set Window Space",
        .options     = (struct optparse_opt[]){
          {
            .short_name    = 'w',
            .long_name     = "window-id",
            .description   = "window id",
            .arg_name      = "WINDOW-ID",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->window_id),
            .function      = (void (*)(void)) _check_window_id,
          },
          {
            .short_name    = 's',
            .long_name     = "space-id",
            .description   = "space id",
            .arg_name      = "SPACE-ID",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->space_id),
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name     = "set-space",                                      .description  = "Set Space",
        .function = cmds[COMMAND_SET_SPACE].fxn,
        .about    = "List Displays",
        .options  = (struct optparse_opt[]){
          {
            .short_name    = 's',
            .long_name     = "space-id",
            .description   = "space id",
            .arg_name      = "SPACE-ID",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->space_id),
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name     = "displays",                                       .description  = "List Displays",
        .function = cmds[COMMAND_DISPLAYS].fxn,
        .about    = "List Displays",
        .options  = (struct optparse_opt[]){
          { END_OF_OPTIONS },
        },
      },
      {
        .name     = "windows",                                        .description  = "List Windows",
        .function = cmds[COMMAND_WINDOWS].fxn,
        .about    = "List Windows",
        .options  = (struct optparse_opt[]){
          {
            .short_name  = 'c',
            .long_name   = "current-space",
            .description = "Windows in current space only",
            .flag_type   = FLAG_TYPE_SET_TRUE,
            .flag        = &(args->current_space_only),
          },
          {
            .short_name    = 's',
            .long_name     = "space-id",
            .description   = "space id",
            .arg_name      = "SPACE-ID",
            .arg_data_type = DATA_TYPE_UINT16,
            .arg_dest      = &(args->space_id),
          },
          { END_OF_OPTIONS },
        },
      },
      {
        .name     = "spaces",                                         .description  = "List Spaces",
        .function = cmds[COMMAND_SPACES].fxn,
        .about    = "List Spaces",
        .options  = (struct optparse_opt[]){
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
