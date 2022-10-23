#pragma once
#ifndef GPP_DLS_COMMANDS_C
#define GPP_DLS_COMMANDS_C
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
struct __args_t {
  char *n, *font_name;
  int space_id,window_id;
  int *ids;
  bool verbose_mode, debug_mode;
};
#include "optparse99/optparse99.h"
struct __option_t {
  struct optparse_opt (^opt)(void);
};
struct __args_t __args;


#include "dls/dls-commands.h"
#include "dls/dls-options.h"
#include "dls/gpp-dls-options.h"
const struct dls_command_t dls_commands[] = {
  [DLS_COMMAND_ID_LAYOUT_LS] = {
    .name = "layout ls", .desc = "List Layouts",
    .options_qty = 0, .commands_qty = 0,
    .options = {
      0,
    },
    .commands = {
      0,
    },
    .opts = ^struct optparse_opt *(void){
      log_info("Getting %s %lu Options", "layout_ls", 0);
      struct optparse_opt opts[] = {
       { END_OF_OPTIONS },
      };
      return(opts);
    },
    .cmds = ^struct optparse_cmd *(void){
      log_info("Getting %s %lu Commands", "layout_ls", 0);
      struct optparse_cmd cmds[] = {
       { END_OF_SUBCOMMANDS },
      };
      return(cmds);
    },
  },
  [DLS_COMMAND_ID_LAYOUT_NAMES] = {
    .name = "layout names", .desc = "Layout Names",
    .options_qty = 0, .commands_qty = 0,
    .options = {
      0,
    },
    .commands = {
      0,
    },
    .opts = ^struct optparse_opt *(void){
      log_info("Getting %s %lu Options", "layout_names", 0);
      struct optparse_opt opts[] = {
       { END_OF_OPTIONS },
      };
      return(opts);
    },
    .cmds = ^struct optparse_cmd *(void){
      log_info("Getting %s %lu Commands", "layout_names", 0);
      struct optparse_cmd cmds[] = {
       { END_OF_SUBCOMMANDS },
      };
      return(cmds);
    },
  },
  [DLS_COMMAND_ID_LAYOUT_APPLY] = {
    .name = "layout apply", .desc = "Apply Layout",
    .options_qty = 0, .commands_qty = 0,
    .options = {
      0,
    },
    .commands = {
      0,
    },
    .opts = ^struct optparse_opt *(void){
      log_info("Getting %s %lu Options", "layout_apply", 0);
      struct optparse_opt opts[] = {
       { END_OF_OPTIONS },
      };
      return(opts);
    },
    .cmds = ^struct optparse_cmd *(void){
      log_info("Getting %s %lu Commands", "layout_apply", 0);
      struct optparse_cmd cmds[] = {
       { END_OF_SUBCOMMANDS },
      };
      return(cmds);
    },
  },
  [DLS_COMMAND_ID_LAYOUT_SHOW] = {
    .name = "layout show", .desc = "Show Layout",
    .options_qty = 0, .commands_qty = 0,
    .options = {
      0,
    },
    .commands = {
      0,
    },
    .opts = ^struct optparse_opt *(void){
      log_info("Getting %s %lu Options", "layout_show", 0);
      struct optparse_opt opts[] = {
       { END_OF_OPTIONS },
      };
      return(opts);
    },
    .cmds = ^struct optparse_cmd *(void){
      log_info("Getting %s %lu Commands", "layout_show", 0);
      struct optparse_cmd cmds[] = {
       { END_OF_SUBCOMMANDS },
      };
      return(cmds);
    },
  },
  [DLS_COMMAND_ID_LAYOUT_RENDER] = {
    .name = "layout render", .desc = "Render Layout",
    .options_qty = 0, .commands_qty = 0,
    .options = {
      0,
    },
    .commands = {
      0,
    },
    .opts = ^struct optparse_opt *(void){
      log_info("Getting %s %lu Options", "layout_render", 0);
      struct optparse_opt opts[] = {
       { END_OF_OPTIONS },
      };
      return(opts);
    },
    .cmds = ^struct optparse_cmd *(void){
      log_info("Getting %s %lu Commands", "layout_render", 0);
      struct optparse_cmd cmds[] = {
       { END_OF_SUBCOMMANDS },
      };
      return(cmds);
    },
  },
  [DLS_COMMAND_ID_LAYOUT_TEST] = {
    .name = "layout test", .desc = "Test Layout",
    .options_qty = 0, .commands_qty = 0,
    .options = {
      0,
    },
    .commands = {
      0,
    },
    .opts = ^struct optparse_opt *(void){
      log_info("Getting %s %lu Options", "layout_test", 0);
      struct optparse_opt opts[] = {
       { END_OF_OPTIONS },
      };
      return(opts);
    },
    .cmds = ^struct optparse_cmd *(void){
      log_info("Getting %s %lu Commands", "layout_test", 0);
      struct optparse_cmd cmds[] = {
       { END_OF_SUBCOMMANDS },
      };
      return(cmds);
    },
  },
  [DLS_COMMAND_ID_LAYOUT] = {
    .name = "layout", .desc = "Layout Manager",
    .options_qty = 2, .commands_qty = 6,
    .options = {
      DLS_OPTION_ID_VERBOSE_MODE,
      DLS_OPTION_ID_DEBUG_MODE,
      0,
    },
    .commands = {
      DLS_COMMAND_ID_LAYOUT_LS,
      DLS_COMMAND_ID_LAYOUT_NAMES,
      DLS_COMMAND_ID_LAYOUT_APPLY,
      DLS_COMMAND_ID_LAYOUT_SHOW,
      DLS_COMMAND_ID_LAYOUT_RENDER,
      DLS_COMMAND_ID_LAYOUT_TEST,
      0,
    },
    .opts = ^struct optparse_opt *(void){
      log_info("Getting %s %lu Options", "layout", 2);
      struct optparse_opt opts[] = {
       { END_OF_OPTIONS },
      };
      return(opts);
    },
    .cmds = ^struct optparse_cmd *(void){
      log_info("Getting %s %lu Commands", "layout", 6);
      struct optparse_cmd cmds[] = {
       { END_OF_SUBCOMMANDS },
      };
      return(cmds);
    },
  },
  [DLS_COMMAND_ID_CAPTURE_WINDOW] = {
    .name = "capture window", .desc = "Layout Manager",
    .options_qty = 0, .commands_qty = 0,
    .options = {
      0,
    },
    .commands = {
      0,
    },
    .opts = ^struct optparse_opt *(void){
      log_info("Getting %s %lu Options", "capture_window", 0);
      struct optparse_opt opts[] = {
       { END_OF_OPTIONS },
      };
      return(opts);
    },
    .cmds = ^struct optparse_cmd *(void){
      log_info("Getting %s %lu Commands", "capture_window", 0);
      struct optparse_cmd cmds[] = {
       { END_OF_SUBCOMMANDS },
      };
      return(cmds);
    },
  },
  [DLS_COMMAND_ID_CAPTURE_SPACE] = {
    .name = "capture space", .desc = "Layout Manager",
    .options_qty = 0, .commands_qty = 0,
    .options = {
      0,
    },
    .commands = {
      0,
    },
    .opts = ^struct optparse_opt *(void){
      log_info("Getting %s %lu Options", "capture_space", 0);
      struct optparse_opt opts[] = {
       { END_OF_OPTIONS },
      };
      return(opts);
    },
    .cmds = ^struct optparse_cmd *(void){
      log_info("Getting %s %lu Commands", "capture_space", 0);
      struct optparse_cmd cmds[] = {
       { END_OF_SUBCOMMANDS },
      };
      return(cmds);
    },
  },
  [DLS_COMMAND_ID_CAPTURE_DISPLAY] = {
    .name = "capture display", .desc = "Layout Manager",
    .options_qty = 0, .commands_qty = 0,
    .options = {
      0,
    },
    .commands = {
      0,
    },
    .opts = ^struct optparse_opt *(void){
      log_info("Getting %s %lu Options", "capture_display", 0);
      struct optparse_opt opts[] = {
       { END_OF_OPTIONS },
      };
      return(opts);
    },
    .cmds = ^struct optparse_cmd *(void){
      log_info("Getting %s %lu Commands", "capture_display", 0);
      struct optparse_cmd cmds[] = {
       { END_OF_SUBCOMMANDS },
      };
      return(cmds);
    },
  },
  [DLS_COMMAND_ID_CAPTURE] = {
    .name = "capture", .desc = "Layout Manager",
    .options_qty = 0, .commands_qty = 3,
    .options = {
      0,
    },
    .commands = {
      DLS_COMMAND_ID_CAPTURE_WINDOW,
      DLS_COMMAND_ID_CAPTURE_SPACE,
      DLS_COMMAND_ID_CAPTURE_DISPLAY,
      0,
    },
    .opts = ^struct optparse_opt *(void){
      log_info("Getting %s %lu Options", "capture", 0);
      struct optparse_opt opts[] = {
       { END_OF_OPTIONS },
      };
      return(opts);
    },
    .cmds = ^struct optparse_cmd *(void){
      log_info("Getting %s %lu Commands", "capture", 3);
      struct optparse_cmd cmds[] = {
       { END_OF_SUBCOMMANDS },
      };
      return(cmds);
    },
  },
  { 0 },
};

void get_dls_command_commands(enum dls_command_id_t id){
  log_info("\tGetting %lu Commands for Command #%d", 123, id);
}

void get_dls_command_options(enum dls_command_id_t id){
  log_info("\tGetting %lu Options for Command #%d", 123, id);
}

void get_dls_command(enum dls_command_id_t id){
  log_info("Getting %lu Commands, %lu Options from Command #%lu: %s",
    dls_commands[id].commands_qty,
    dls_commands[id].options_qty, 
    id,
    dls_commands[id].name
  );
  for(size_t i=0;i<dls_commands[id].options_qty;i++){
    log_info("%lu|%lu",i,dls_commands[id].options[i]);
    get_dls_command_options(dls_commands[id].options[i]);
  }
  for(size_t i=0;i<dls_commands[id].commands_qty;i++){
    log_info("%lu|%lu",i,dls_commands[id].commands[i]);
    get_dls_command_commands(dls_commands[id].commands[i]);
  }
  //get_dls_command_options(id);
}

void get_cmd(){
    struct optparse_cmd cmd = {
      .about       = "dls v1.00 - List Darwin Objects",
      .description = "This program lists Darwin Objects",
      .name        = "dls",
      .operands    = "[COMMAND...]",
      .options     = (struct optparse_opt[]) {
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
        { END_OF_SUBCOMMANDS },
      },
    };
}


/*
enum __option_id { 
 DLS_OPTION_ID_DEBUG_MODE,DLS_OPTION_ID_VERBOSE_MODE,
DLS_OPTION_ID_FONT_NAME,
DLS_OPTION_ID_SPACE_ID,DLS_OPTION_ID_IDS,DLS_OPTION_ID_WINDOW_ID,

  COMMON_OPTION_IDS_QTY,
};
*/
struct optparse_opt __optparse_opt[] = {

  [DLS_OPTION_ID_DEBUG_MODE] = (struct optparse_opt){ 
      .short_name = 'd',
      .description = "debug mode",
      .long_name = "debug",
      .flag = &(__args.debug_mode),
      .flag_type = FLAG_TYPE_SET_TRUE,
      .arg_data_type = NULL,
      .arg_dest = NULL,
      .arg_dest_size = 0,
      .arg_delim = NULL,
      .function = NULL,
  },
  [DLS_OPTION_ID_VERBOSE_MODE] = (struct optparse_opt){ 
      .short_name = 'v',
      .description = "verbose mode",
      .long_name = "verbose",
      .flag = &(__args.verbose_mode),
      .flag_type = FLAG_TYPE_SET_TRUE,
      .arg_data_type = NULL,
      .arg_dest = NULL,
      .arg_dest_size = 0,
      .arg_delim = NULL,
      .function = NULL,
  },

  [DLS_OPTION_ID_FONT_NAME] = (struct optparse_opt){ 
      .short_name = NULL,
      .description = "Specify Font Name",
      .long_name = "font-name",
      .flag = NULL,
      .flag_type = NULL,
      .arg_data_type = DATA_TYPE_STR,
      .arg_dest = &(__args.font_name),
      .arg_dest_size = 0,
      .arg_delim = NULL,
      .function = NULL,
  },

  [DLS_OPTION_ID_SPACE_ID] = (struct optparse_opt){ 
      .short_name = 's',
      .description = "Specify Space ID",
      .long_name = "space-id",
      .flag = NULL,
      .flag_type = NULL,
      .arg_data_type = NULL,
      .arg_dest = &(__args.space_id),
      .arg_dest_size = 0,
      .arg_delim = NULL,
      .function = NULL,
  },
  [DLS_OPTION_ID_IDS] = (struct optparse_opt){ 
      .short_name = 'i',
      .description = "Specify Space IDs",
      .long_name = "ids",
      .flag = NULL,
      .flag_type = NULL,
      .arg_data_type = NULL,
      .arg_dest = &(__args.ids),
      .arg_dest_size = 0,
      .arg_delim = ",",
      .function = NULL,
  },
  [DLS_OPTION_ID_WINDOW_ID] = (struct optparse_opt){ 
      .short_name = 'w',
      .description = "Specify Window ID",
      .long_name = "window-id",
      .flag = NULL,
      .flag_type = NULL,
      .arg_data_type = NULL,
      .arg_dest = &(__args.window_id),
      .arg_dest_size = 0,
      .arg_delim = NULL,
      .function = NULL,
  },

};
/*
char *__option_names[] = {
 
  [DLS_OPTION_ID_DEBUG_MODE] = "debug mode",
  [DLS_OPTION_ID_VERBOSE_MODE] = "verbose mode",

  [DLS_OPTION_ID_FONT_NAME] = "font name",

  [DLS_OPTION_ID_SPACE_ID] = "Space ID",
  [DLS_OPTION_ID_IDS] = "IDs",
  [DLS_OPTION_ID_WINDOW_ID] = "Window ID",

};

*/


#endif
