#pragma once
#ifndef MT_EMA_C
#define MT_EMA_C
#include "mt-include.h"

float avg_float_pointers_v(struct Vector *v){
  float total = 0;

  for (size_t i = 0; i < vector_size(v); i++) {
    total += *((float *)vector_get(v, i));
  }
  return((float)total / vector_size(v));
}

struct Vector *get_past_market_period_closes_v(size_t period_index, size_t past_periods_qty){
  struct Vector          *v    = vector_new();
  size_t                 index = 0;
  struct market_period_t *pp;

  for (size_t i = 1; i <= past_periods_qty; i++) {
    index = period_index - i;
    pp    = (struct market_period_t *)(vector_get(market_stats->periods_v, index));
    vector_push(v, (void *)&(pp->close));
  }
  return(v);
}
int print_market_period(struct market_period_t *p){
  fprintf(stderr,
          " [%.4d-%.2d-%.2d %.2d:%.2d] (" AC_GREEN AC_BOLD "%3s ago" AC_RESETALL ")"
          "|Open:"AC_CYAN "%.3f"AC_RESETALL "|Close:"AC_MAGENTA "%.3f"AC_RESETALL
          "|High:"AC_GREEN "%.3f"AC_RESETALL "|Low:"AC_RED "%.3f"AC_RESETALL
          "|Volume:"AC_YELLOW "%.*ld"AC_RESETALL
          "|\n",
          p->year, p->month, p->day, p->hour, p->minute, milliseconds_to_string(p->age_ms),
          p->open, p->close,
          p->high, p->low,
          (int)(market_stats->max_volume % 10) + 1, p->volume
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
  market_stats->max_volume = (p->volume > market_stats->max_volume) ? p->volume : market_stats->max_volume;
  asprintf(&p->ymdhm, "%.4d-%.2d-%.2d %.2d:%.2d", p->year, p->month, p->day, p->hour, p->minute);
    p->time = timelib_strtotime(p->ymdhm, strlen(p->ymdhm), NULL, timelib_builtin_db(),  timelib_parse_tzfile);
    timelib_update_ts(p->time, NULL);
    p->timestamp = p->time->sse;
    p->age_ms    = timestamp() - p->timestamp * 1000;
    vector_push(market_stats->periods_v, (void *)p);
    return(EXIT_SUCCESS);
  }


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
        stringbuffer_append_string(sb, "\n");      }
    }
    return(stringbuffer_to_string(sb));
}

#endif
