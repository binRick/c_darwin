#pragma once
#ifndef MT_COMMANDS_C
#define MT_COMMANDS_C
#include "mt/mt-include.h"

void _command_ema_info(){
  log_info("ema info");
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
      AC_RED     "\n\t All     Items  :    %s" AC_RESETALL
      AC_GREEN   "\n\t Enabled Items  :    %s" AC_RESETALL
      "\n%s",
      args->debug_mode == true ? AC_GREEN "Yes" : AC_RED "No",
      args->verbose_mode == true ? AC_GREEN "Yes" : AC_RED "No",
      args->list->all_items == true ? AC_GREEN "Yes" : AC_RED "No",
      args->list->enabled_items == true ? AC_GREEN "Yes" : AC_RED "No",
      ""
      );
    exit(EXIT_SUCCESS);
}

void _command_parse_csv(){
    size_t                 items_qty = 0;
    struct StringFNStrings csv_lines = stringfn_split_lines_and_trim(inc_market_csv_data);
    char                   **items = NULL, **buf;
    unsigned long          started = timestamp();

    for (int i = 1; i < csv_lines.count; i++) {
      if (strlen(csv_lines.strings[i]) < MIN_CSV_LINE_LENGTH || strlen(csv_lines.strings[i]) > MAX_CSV_LINE_LENGTH) {
        continue;
      }
      items     = parse_csv(csv_lines.strings[i]);
      items_qty = 0;
      buf       = items;
      while (*buf++) {
        items_qty++;
      }
      assert(new_market_period(items, items_qty) == EXIT_SUCCESS);
    }

    fprintf(stderr, "Loaded %lu CSV Items\n\t\tCSV File Size: %s\n\t\tCSV Data Lines: %d\n\t\tTime Spent: %s\n", vector_size(market_stats->periods_v), bytes_to_string(inc_market_csv_size), csv_lines.count, milliseconds_to_string(timestamp() - started));
    struct market_period_t *p, *pp, *first = NULL, *last = NULL;
    int                    hours = 0; size_t periods = 0, close_values_averaged = 0, checked_periods_qty = 0;

    started = timestamp();
    for (size_t i = 0; i < vector_size(market_stats->periods_v); i++) {
      p = (struct market_period_t *)vector_get(market_stats->periods_v, i);
      for (int q = 0; q < MARKET_PERIOD_DURATION_TYPES_QTY; q++) {
        hours   = market_period_duration_type_hours[q];
        periods = (size_t)(hours / MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS);
        if (i >= periods) {
          if (!first) {
            first = (struct market_period_t *)vector_get(market_stats->periods_v, i);
          }
          checked_periods_qty += periods;
          p->close_values[q]   = get_past_market_period_closes_v(i, periods);
          close_values_averaged++;
          if (i == vector_size(market_stats->periods_v) - 1) {
            if (args->debug_mode) {
              pp = (struct market_period_t *)vector_get(market_stats->periods_v, i - periods);
              log_debug("Period #%lu/%lu (%s - %s) has %lu close values averaging %f for duration \"%s\" which consists of %d hours and %lu periods",
                        i, vector_size(market_stats->periods_v),
                        p->ymdhm, pp->ymdhm,
                        vector_size(p->close_values[q]),
                        avg_float_pointers_v(p->close_values[q]),
                        market_period_duration_type_names[q], hours, periods
                        );
            }
          }
        }
      }
    }
    last = (struct market_period_t *)vector_get(market_stats->periods_v, vector_size(market_stats->periods_v) - 1);
    fprintf(stderr, "Performed EMA Calculations for Average Close Price\n\t\tCalculations Performed:%lu\n\t\tQuantity EMA Duration Types: %d\n%s\n\t\tMarket Period Close Values Analyzed: %lu\n\t\tMarket Periods: %lu\n\t\tStart Date:   %s\n\t\tEnd Date: %s\n\t\tDuration: %s\n\t\tTime Spent: %s\n",
            close_values_averaged,
            MARKET_PERIOD_DURATION_TYPES_QTY,
            market_period_durations_csv(),
            checked_periods_qty,
            vector_size(market_stats->periods_v),
            first->ymdhm, last->ymdhm, milliseconds_to_string((last->timestamp - first->timestamp) * 1000),
            milliseconds_to_long_string(timestamp() - started)
            );
    if (args->debug_mode) {
      for (size_t i = 0; i < vector_size(market_stats->periods_v); i++) {
        p = (struct market_period_t *)vector_get(market_stats->periods_v, i);
        print_market_period(p);
      }
    }
    exit(EXIT_SUCCESS);
  }

#endif
