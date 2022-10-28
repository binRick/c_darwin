#pragma once
#ifndef MT_COMMANDS_C
#define MT_COMMANDS_C
#include "mt/mt-include.h"

void _command_ema_test_float(void){
  float sample[]      = { 0.5, 0.03, 0.2, .18, .20, .100, .18, .10, .13, .500, 50.5, 5.40, 1.10 };
  float arrNumbers[5] = { 0 };

  int   pos    = 0;
  float newAvg = 0;
  float sum    = 0;
  int   len    = sizeof(arrNumbers) / sizeof(float);
  int   count  = sizeof(sample) / sizeof(float);

  for (int i = 0; i < count; i++) {
    newAvg = ma_moving_average_float(arrNumbers, &sum, pos, len, sample[i]);
    printf("The new average is %f\n", newAvg);
    pos++;
    if (pos >= len)
      pos = 0;
  }

  exit(EXIT_SUCCESS);
}

void _command_ema_test_int(void){
  log_info("ema int test");
  int  sample[]      = { 50, 10, 20, 18, 20, 100, 18, 10, 13, 500, 50, 40, 10 };
  int  arrNumbers[5] = { 0 };

  int  pos    = 0;
  int  newAvg = 0;
  long sum    = 0;
  int  len    = sizeof(arrNumbers) / sizeof(int);
  int  count  = sizeof(sample) / sizeof(int);

  for (int i = 0; i < count; i++) {
    newAvg = ma_moving_average(arrNumbers, &sum, pos, len, sample[i]);
    printf("The new average is %d\n", newAvg);
    pos++;
    if (pos >= len)
      pos = 0;
  }
}

#define MT4_TIMEZONE    "UTC"

void _command_ema_test(void){
  _command_ema_test_int();
  _command_ema_test_float();
  exit(EXIT_SUCCESS);
}

void _command_set_csv_end_date(){
  asprintf(&args->csv->end_date, "%s %.2d:%.2d %s", args->csv->end_date, 0, 0, MT4_TIMEZONE);
  args->csv->end = timelib_strtotime(args->csv->end_date, strlen(args->csv->end_date), NULL, timelib_builtin_db(), timelib_parse_tzfile);
  if (!args->csv->end) {
    log_error("Invalid End Date");
    exit(EXIT_FAILURE);
  }
  timelib_update_ts(args->csv->end, NULL);
  if (args->debug_mode)
    log_debug("end ts:%lld", args->csv->end->sse);
  return;
}

void _command_set_csv_start_date(){
  asprintf(&args->csv->start_date, "%s %.2d:%.2d %s", args->csv->start_date, 0, 0, MT4_TIMEZONE);
  args->csv->start = timelib_strtotime(args->csv->start_date, strlen(args->csv->start_date), NULL, timelib_builtin_db(), timelib_parse_tzfile);
  if (!args->csv->start) {
    log_error("Invalid Start Date");
    exit(EXIT_FAILURE);
  }
  timelib_update_ts(args->csv->start, NULL);
  if (args->debug_mode)
    log_debug("start ts:%lld", args->csv->start->sse);
  return;
}

void _command_load_csv_name_data(void){
  if (args->debug_mode)
    log_debug("Loading CSV %s", args->csv->name);
  stringfn_release_strings_struct(args->csv->lines);
  if (fsio_file_exists(args->csv->name))
    args->csv->data = fsio_read_text_file(args->csv->name);
  else
    args->csv->data = get_embedded_csv_name_data(args->csv->name);
  if (args->csv->data)
    args->csv->lines = stringfn_split_lines_and_trim(args->csv->data);
  if (args->csv->lines.count < 1) {
    log_error("CSV %s not valid", args->csv->name);
    exit(EXIT_FAILURE);
  }
  return;
}

void _command_list_csvs(){
  printf(AC_GREEN_BLACK "%lu CSVs:" AC_RESETALL "\t" AC_YELLOW "%s" AC_RESETALL "\n",
         EMBEDDED_CSV_NAMES_QTY, get_embedded_csv_names_list()
         );
  exit(EXIT_SUCCESS);
}

void _command_ema_info(){
  printf(AC_YELLOW AC_BOLD "https://www.metatrader4.com/en/trading-platform/help/analytics/tech_indicators/moving_average#exponential_moving_average\n"AC_RESETALL);
  kitty_display_image_buffer(inc_ema_png_data, inc_ema_png_size);
  exit(EXIT_SUCCESS);
}

void _command_debug_args(){
  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Debug Arguments" AC_RESETALL
    AC_CYAN    "\n\t Verbose Mode   :    %s" AC_RESETALL
    AC_MAGENTA "\n\t Debug   Mode   :    %s" AC_RESETALL
    "\n%s",
    args->debug_mode == true ? AC_GREEN "Yes" : AC_RED "No",
    args->verbose_mode == true ? AC_GREEN "Yes" : AC_RED "No",
    ""
    );
  exit(EXIT_SUCCESS);
}

void _command_list(){
  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Debug Arguments" AC_RESETALL
    AC_CYAN    "\n\t Verbose Mode   :    %s" AC_RESETALL
    AC_MAGENTA "\n\t Debug   Mode   :    %s" AC_RESETALL
    "\n%s",
    args->debug_mode == true ? AC_GREEN "Yes" : AC_RED "No",
    args->verbose_mode == true ? AC_GREEN "Yes" : AC_RED "No",
    ""
    );
  exit(EXIT_SUCCESS);
}

void _command_parse_csv(){
  unsigned long time_spent_started = timestamp();

  if (args->verbose_mode)
    log_info("CSV %s> |Lines: %d|Size:%s|", args->csv->name, args->csv->lines.count, bytes_to_string(strlen(args->csv->data)));
  size_t        items_qty = 0;
  char          **items = NULL, **buf;
  unsigned long started = timestamp();

  for (int i = 1; i < args->csv->lines.count; i++) {
    if (strlen(args->csv->lines.strings[i]) < MIN_CSV_LINE_LENGTH || strlen(args->csv->lines.strings[i]) > MAX_CSV_LINE_LENGTH)
      continue;
    items     = parse_csv(args->csv->lines.strings[i]);
    items_qty = 0;
    buf       = items;
    while (*buf++)
      items_qty++;
    assert(new_market_period(items, items_qty) == EXIT_SUCCESS);
  }

  fprintf(stderr,
          AC_GREEN "Loaded %lu CSV Items in " AC_RESETALL AC_CYAN "%s"AC_RESETALL
          "\n\tCSV              : "AC_YELLOW "%s"AC_RESETALL
          "\n\tCSV Size         : "AC_BLUE "%s"AC_RESETALL
          "\n\tCSV Lines        : "AC_MAGENTA "%d"AC_RESETALL
          "\n\tMarket Periods   : "AC_MAGENTA "%lu"AC_RESETALL
          "\n%s",
          vector_size(market_stats->periods_v),
          milliseconds_to_string(timestamp() - started),
          args->csv->name,
          bytes_to_string(strlen(args->csv->data)),
          args->csv->lines.count,
          vector_size(market_stats->periods_v),
          ""
          );
  if (vector_size(market_stats->periods_v) == 0) {
    errno = 0;
    log_error("Failed to find any market periods in the specified date range using the specified CSV data");
    exit(EXIT_FAILURE);
  }

  struct market_period_t *p, *prev_p, *first = NULL, *last = NULL;
  int                    hours = 0; size_t periods = 0, close_values_averaged = 0, checked_periods_qty = 0;
  unsigned long          _ts     = 0;
  struct market_period_t *next_p = NULL;

  started = timestamp();
  for (size_t i = 0; i < vector_size(market_stats->periods_v); i++) {
    p = (struct market_period_t *)vector_get(market_stats->periods_v, i);
    if (i == 0) {
      first  = p;
      prev_p = calloc(1, sizeof(struct market_period_t));
      for (int q = 0; q < MARKET_PERIOD_DURATION_TYPES_QTY; q++) {
        prev_p->close_values[q]             = vector_new();
        prev_p->moving_averages[q]          = calloc(1, sizeof(struct moving_average_t));
        prev_p->moving_averages[q]->sum     = 0;
        prev_p->moving_averages[q]->average = 0;
        prev_p->moving_averages[q]->qty     = market_period_duration_type_hours[q] / MARKET_PERIOD_LENGTH_HOURS;
        prev_p->moving_averages[q]->values  = calloc(prev_p->moving_averages[q]->qty, sizeof(float));
      }
    }else
      prev_p = (struct market_period_t *)vector_get(market_stats->periods_v, i - 1);
    for (int q = 0; q < MARKET_PERIOD_DURATION_TYPES_QTY; q++) {
      hours                                   = market_period_duration_type_hours[q];
      periods                                 = (size_t)(hours / MARKET_PERIOD_LENGTH_HOURS);
      _ts                                     = timestamp();
      p->close_values[q]                      = get_past_market_period_closes_v(i, periods);
      market_stats->time_spent.close_periods += timestamp() - _ts;
      close_values_averaged++;
      checked_periods_qty                     += periods;
      _ts                                      = timestamp();
      p->moving_averages[q]                    = get_ma_type_from_current_and_previous_market_period(q, p, prev_p);
      market_stats->time_spent.moving_average += timestamp() - _ts;
      _ts                                      = timestamp();
      p->averages[q]                           = avg_float_pointers_v(p->close_values[q]);
      market_stats->time_spent.average        += timestamp() - _ts;
    }
  }
  market_stats->time_spent.total = timestamp() - time_spent_started;
  last                           = (struct market_period_t *)vector_get(market_stats->periods_v, vector_size(market_stats->periods_v) - 1);
  for (size_t i = 0; i < vector_size(market_stats->periods_v); i++) {
    p = (struct market_period_t *)vector_get(market_stats->periods_v, i);
    if (i == 0) {
      first  = p;
      prev_p = NULL;
    }else{
    }
    next_p = (i < vector_size(market_stats->periods_v) - 1) ? (struct market_period_t *)vector_get(market_stats->periods_v, i + 1) : NULL;
    if (args->verbose_mode)
      for (int q = 0; q < MARKET_PERIOD_DURATION_TYPES_QTY; q++)
        fprintf(stderr,
                "Market Period                      :      #%lu/%lu"
                "\n\tPeriod type                    :      %s"
                "\n\tDate Range                     :      %s - %s (%s)"
                "\n\tClose Values/Hours/Periods     :      %lu/%d/%lu"
                "\n\tAverage                        :      %f"
                "\n\tMoving Average                 :      "AC_GREEN "%f" AC_RESETALL
                "%s",
                i + 1, vector_size(market_stats->periods_v),
                market_period_duration_type_names[q],
                p->ymdhm,
                ((next_p != NULL) ? next_p->ymdhm : "None"),
                ((next_p != NULL) ? milliseconds_to_string((next_p->timestamp - p->timestamp) * 1000) : "None"),
                vector_size(p->close_values[q]),
                hours,
                periods,
                p->averages[q],
                p->moving_averages[q]->average,
                "\n"
                );
  }
  fprintf(stderr,
          "Performed " AC_RED "%lu" AC_RESETALL " EMA Calculations for Average Close Price"
          "\n\tQuantity EMA Duration Types: %d"
          "\n"            AC_YELLOW AC_ITALIC "%s" AC_RESETALL
          "\n\tMarket Period Close Values Analyzed: %lu"
          "\n\tMarket Periods: %lu"
          "\n\tStart Date:   %s"
          "\n\tEnd Date: %s"
          "\n\tDuration: %s"
          "\n\tTime Spent:"
          "\n\t       Calculating Period Close Values   : %s"
          "\n\t       Calculating Averages              : %s"
          "\n\t       Calculating Moving Averages       : %s"
          "\n\t       Total                             : %s"
          "\n%s",
          close_values_averaged,
          MARKET_PERIOD_DURATION_TYPES_QTY,
          market_period_durations_csv(),
          checked_periods_qty,
          vector_size(market_stats->periods_v),
          first->ymdhm, last->ymdhm, milliseconds_to_string((last->timestamp - first->timestamp) * 1000),
          milliseconds_to_string(market_stats->time_spent.close_periods),
          milliseconds_to_string(market_stats->time_spent.average),
          milliseconds_to_string(market_stats->time_spent.moving_average),
          milliseconds_to_string(market_stats->time_spent.total),
          ""
          );
  if (args->debug_mode)
    for (size_t i = 0; i < vector_size(market_stats->periods_v); i++) {
      p = (struct market_period_t *)vector_get(market_stats->periods_v, i);
      print_market_period(p);
    }
  exit(EXIT_SUCCESS);
} /* _command_parse_csv */

void _command_table(void){
  ft_table_t *table = ft_create_table();

  ft_set_border_style(table, FT_NICE_STYLE);
  ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);

  ft_u8write_ln(table, "#", "Period Type", "Date", "Closed", "Hours", "Periods", "Avg", "Moving Avg");
  ft_add_separator(table);
  ft_u8write_ln(table, "1", "x", "y", "n-body", "1000", "1.6", "1,500,000", "✔");
  ft_u8write_ln(table, "1", "x", "y", "n-body", "1000", "1.6", "1,500,000", "✖");

  ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
  ft_set_cell_prop(table, 8, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
  ft_set_cell_prop(table, FT_ANY_ROW, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
  ft_set_cell_prop(table, FT_ANY_ROW, 4, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
  ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);

  ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
  ft_set_cell_prop(table, FT_ANY_ROW, 2, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
  ft_set_cell_prop(table, FT_ANY_ROW, 3, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
  ft_set_cell_prop(table, FT_ANY_ROW, 4, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
  ft_set_cell_prop(table, 8, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);

  ft_set_cell_prop(table, 1, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
  ft_set_cell_prop(table, 2, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
  ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);

  ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
  ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 10);

  const char *table_str = ft_to_u8string(table);

  printf("%s", table_str);
  ft_destroy_table(table);
  exit(EXIT_SUCCESS);
}
#endif
