#pragma once
#ifndef MT_MARKET_C
#define MT_MARKET_C
#include "mt/mt-include.h"

int print_market_period(struct market_period_t *p){
  fprintf(stderr,
          " [%.4d-%.2d-%.2d %.2d:%.2d] (" AC_GREEN AC_BOLD "%3s ago" AC_RESETALL ")"
          "|Open:"AC_CYAN "%.3f"AC_RESETALL "|Close:"AC_MAGENTA "%.3f"AC_RESETALL
          "|High:"AC_GREEN "%.3f"AC_RESETALL "|Low:"AC_RED "%.3f"AC_RESETALL
          "|Volume:"AC_YELLOW "%.*ld"AC_RESETALL
          "|Min/Max MA:"AC_YELLOW "%.5f/%.5f"AC_RESETALL
          "|\n%s",
          p->year, p->month, p->day, p->hour, p->minute, milliseconds_to_string(p->age_ms),
          p->open, p->close,
          p->high, p->low,
          (int)(market_stats->max_volume % 10) + 1, p->volume,
          market_stats->min_ma, market_stats->max_ma,
          ""
          );
}

int new_market_period(char **items, size_t items_qty){
  char                   *item;
  struct market_period_t *p = calloc(1, sizeof(struct market_period_t));

  if (items_qty != CSV_COLUMNS_QTY) {
    log_error("Invalid CSV Line (%lu items)", items_qty);
    return(EXIT_FAILURE);
  }

  struct StringFNStrings s;

  for (size_t i = 0; i < CSV_COLUMNS_QTY; i++) {
    item = stringfn_trim(items[i]);
    switch (i) {
    case CSV_COLUMN_YEAR_MONTH_DAY:
      s        = stringfn_split(item, '.');
      p->year  = atoi(s.strings[0]);
      p->month = atoi(s.strings[1]);
      p->day   = atoi(s.strings[2]);
      stringfn_release_strings_struct(s);
      break;
    case CSV_COLUMN_HOUR_MINUTE:
      s         = stringfn_split(item, ':');
      p->hour   = atoi(s.strings[0]);
      p->minute = atoi(s.strings[1]);
      stringfn_release_strings_struct(s);
      break;
    case CSV_COLUMN_OPEN: p->open     = atof(item); break;
    case CSV_COLUMN_HIGH: p->high     = atof(item); break;
    case CSV_COLUMN_LOW: p->low       = atof(item); break;
    case CSV_COLUMN_CLOSE: p->close   = atof(item); break;
    case CSV_COLUMN_VOLUME: p->volume = atoi(item); break;
    }
  }
  asprintf(&p->ymdhm, "%.4d-%.2d-%.2d %.2d:%.2d %s", p->year, p->month, p->day, p->hour, p->minute, MT4_TIMEZONE);
  p->time = timelib_strtotime(p->ymdhm, strlen(p->ymdhm), NULL, timelib_builtin_db(), timelib_parse_tzfile);
  timelib_update_ts(p->time, NULL);
  p->timestamp = p->time->sse;
  p->age_ms    = timestamp() - p->timestamp * 1000;
  if ((args->csv->end && p->time->sse > args->csv->end->sse) || (args->csv->start && p->time->sse < args->csv->start->sse)) {
    if (args->debug_mode) {
      log_debug("Excluding CSV Item (%s not within start/end range)", p->ymdhm);
    }
    return(EXIT_SUCCESS);
  }
  market_stats->max_volume = (p->volume > market_stats->max_volume) ? p->volume : market_stats->max_volume;

  vector_push(market_stats->periods_v, (void *)p);
  return(EXIT_SUCCESS);
} /* new_market_period */

int release_market_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur){
  if (p->stats->averages[dur]->buffer) {
    free(p->stats->averages[dur]->buffer);
  }
  p->stats->averages[dur]->buffer = NULL;
  if (p->stats->averages[dur]) {
    free(p->stats->averages[dur]);
  }
  return(EXIT_SUCCESS);
}

char *market_period_durations_csv(){
  struct StringBuffer *sb = stringbuffer_new();

  for (size_t i = 0; i < MARKET_PERIOD_DURATION_TYPES_QTY; i++) {
    stringbuffer_append_string(sb, "\t\t\t - ");
    stringbuffer_append_string(sb, market_period_duration_type_names[i]);
    if (i < MARKET_PERIOD_DURATION_TYPES_QTY - 1) {
      stringbuffer_append_string(sb, "\n");
    }
  }
  return(stringbuffer_to_string(sb));
}

struct ma_t *get_ma_type_from_current_and_previous_market_period(enum ema_moving_average_type_t q, struct market_period_t *p, struct market_period_t *prev_p){
  if (args->debug_mode) {
    log_debug("Duration               : %d (%s) [%lu periods]", q, market_period_duration_type_names[q], market_period_duration_type_periods_qty[q]);
    log_debug(
      "Previous Market Period  : |Close values:%lu|MA Sum:%f|Qty:%lu|"
      "%s",
      vector_size(prev_p->close_values[q]),
      prev_p->moving_averages[q]->sum,
      prev_p->moving_averages[q]->qty,
      ""
      );
  }
  float                   cur             = *(float *)vector_get(p->close_values[q], vector_size(p->close_values[q]) - 1);
  struct moving_average_t *moving_average = calloc(1, sizeof(struct moving_average_t));
  moving_average->average = 0;
  moving_average->qty     = prev_p->moving_averages[q]->qty;
  moving_average->values  = prev_p->moving_averages[q]->values;
  moving_average->sum     = prev_p->moving_averages[q]->sum;
  if (args->debug_mode) {
    log_debug(
      "Current Market Period  : |Close values:%lu|latest:%f|MA Sum:%f|pos:%lu|qty:%lu|cur val:%f|"
      "%s",
      vector_size(p->close_values[q]),
      *(float *)vector_get(p->close_values[q], vector_size(p->close_values[q]) - 1),
      moving_average->sum,
      (moving_average->qty) - 1,
      moving_average->qty,
      moving_average->values[moving_average->qty - 1],
      ""
      );
  }
  moving_average->average = ma_moving_average_float(moving_average->values, &(moving_average->sum), (moving_average->qty) - 1, moving_average->qty, cur);
  if (args->debug_mode) {
    log_debug("              -> avg:%f|sum:%f",
              moving_average->average,
              moving_average->sum
              );
  }
  return(moving_average);
}

struct Vector *get_past_market_period_closes_v(size_t period_index, size_t past_periods_qty){
  struct Vector          *v    = vector_new();
  size_t                 index = 0;
  struct market_period_t *pp;

  for (size_t i = 0; i < past_periods_qty && i <= period_index; i++) {
    index = period_index - i;
    if (index < vector_size(market_stats->periods_v) - 1 && index >= 0) {
      pp = (struct market_period_t *)(vector_get(market_stats->periods_v, index));
      if (pp) {
        vector_push(v, (void *)&(pp->close));
      }
    }
  }
  return(v);
}

bool create_market_periods_table(void){
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
  return(true);
}
#endif
