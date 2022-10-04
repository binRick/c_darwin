#ifndef MT_C
#define MT_C
#include "mt/mt-include.h"

int main(int argc, char **argv) {
  market_stats->periods_v = vector_new();
  args->csv->name         = get_largest_embedded_csv_name();
  _command_load_csv_name_data();
  struct optparse_cmd main_cmd = {
    .about       = "mt v1.00 - Automation Tool",
    .description = "This program does something useful",
    .name        = "mt",
    .operands    = "[COMMAND...]",
    .options     = (struct optparse_opt[]) {
      {
        .short_name  = 'h',
        .long_name   = "help",
        .description = "Print help information and quit",
        .function    = optparse_print_help,
      },
      {
        .short_name  = 'v',
        .long_name   = "verbose",
        .description = "Increase verbosity",
        .flag_type   = FLAG_TYPE_SET_TRUE,
        .flag        = &(args->verbose_mode),
      },
      {
        .short_name  = 'd',
        .long_name   = "debug",
        .description = "Debug Mode",
        .flag_type   = FLAG_TYPE_SET_TRUE,
        .flag        = &(args->debug_mode),
      },
      {
        .short_name    = 'c',
        .long_name     = "csv",
        .description   = "CSV File to Use (csvs to list)",
        .arg_name      = "CSV",
        .arg_dest      = &(args->csv->name),
        .arg_data_type = DATA_TYPE_STR,
        .function      = _command_load_csv_name_data,
      },
      {
        .short_name    = 's',
        .long_name     = "start-date",
        .description   = "CSV Data Start Date (YYYY-MM-DD)",
        .arg_name      = "START-DATE",
        .arg_dest      = &(args->csv->start_date),
        .arg_data_type = DATA_TYPE_STR,
        .function      = _command_set_csv_start_date,
      },
      {
        .short_name    = 'e',
        .long_name     = "end-date",
        .description   = "CSV Data End Date (YYYY-MM-DD)",
        .arg_name      = "END-DATE",
        .arg_dest      = &(args->csv->end_date),
        .arg_data_type = DATA_TYPE_STR,
        .function      = _command_set_csv_end_date,
      },
      { END_OF_OPTIONS },
    },
    .subcommands       = (struct optparse_cmd[]) {
      {
        .description = "Print a subcommand's help information and quit.",
        .name        = "help",
        .operands    = "COMMAND",
        .about       = "Command Help",
        .function    = optparse_print_help_subcmd,
      },
      {
        .name        = "csvs",
        .description = "CSVs Info",
        .function    = _command_list_csvs,
        .about       = "CSV Information",
        .options     = (struct optparse_opt[]){
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "ema-test",
        .description = "EMA Test",
        .function    = _command_ema_test,
        .about       = "EMA Test",
        .options     = (struct optparse_opt[]){
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "ema",
        .description = "EMA Info",
        .function    = _command_ema_info,
        .about       = "EMA Information",
        .options     = (struct optparse_opt[]){
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "parse",
        .description = "Parse CSV",
        .function    = _command_parse_csv,
        .about       = "Parse CSV File",
        .options     = (struct optparse_opt[]){
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "table",
        .description = "Table",
        .function    = _command_table,
        .about       = "Table",
        .options     = (struct optparse_opt[]){
          { END_OF_OPTIONS },
        },
      },
      {
        .name        = "args",
        .description = "Debug Arguments",
        .function    = _command_debug_args,
        .about       = "Debug Arguments",
      },
      { END_OF_SUBCOMMANDS },
    },
  };

  optparse_parse(&main_cmd, &argc, &argv);
  optparse_print_help();
  return(EXIT_FAILURE);
} /* main */

#endif
