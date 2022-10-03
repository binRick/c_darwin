#include "mt/mt.h"
////////
#include "csv_parser/csv.h"
#include "timelib/timelib.h"
#ifdef INCBIN_STYLE
#undef INCBIN_STYLE
#endif
#ifdef INCBIN_PREFIX
#undef INCBIN_PREFIX
#endif
#define MAX_CSV_LINE_LENGTH    1024
#define MIN_CSV_LINE_LENGTH    CSV_COLUMNS_QTY *2
#include "incbin/incbin.h"
#define MARKET_CSV_FILE        "XRPUSD240.csv"
INCBIN(market_csv, MARKET_CSV_FILE);
struct market_period_t;
static int new_market_period(char **items, size_t items_qty);
static int print_market_period(struct market_period_t *p);
enum csv_column_t {
  CSV_COLUMN_YEAR_MONTH_DAY,
  CSV_COLUMN_HOUR_MINUTE,
  CSV_COLUMN_OPEN,
  CSV_COLUMN_HIGH,
  CSV_COLUMN_LOW,
  CSV_COLUMN_CLOSE,
  CSV_COLUMN_VOLUME,
  CSV_COLUMNS_QTY,
};
enum market_period_length_type_t {
  MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS = 4,
};
enum ema_moving_average_type_t {
  EMA_MOVING_AVERAGE_TYPE_EXPONENTIAL_AVERAGE,
};
enum ema_timeframe_type_t {
  EMA_TIMEFRAME_TYPE_CURRENT,
};
enum ema_shift_type_t {
  EMA_SHIFT_TYPE_CURRENT,
};
enum ema_applied_price_type_t {
  EMA_APPLIED_PRICE_TYPE_CLOSE,
};
enum ema_period_duration_type_t {
  MARKET_PERIOD_DURATION_TYPE_LAST_EIGHT_HOURS,
  MARKET_PERIOD_DURATION_TYPE_LAST_TWELVE_HOURS,
  MARKET_PERIOD_DURATION_TYPE_LAST_DAY,
  MARKET_PERIOD_DURATION_TYPE_LAST_TWO_DAYS,
  MARKET_PERIOD_DURATION_TYPE_LAST_WEEK,
  MARKET_PERIOD_DURATION_TYPE_LAST_MONTH,
  MARKET_PERIOD_DURATION_TYPE_LAST_THREE_MONTHS,
  MARKET_PERIOD_DURATION_TYPE_LAST_SIX_MONTHS,
  MARKET_PERIOD_DURATION_TYPE_LAST_YEAR,
  MARKET_PERIOD_DURATION_TYPES_QTY,
};
static char *market_period_duration_type_names[] = {
  [MARKET_PERIOD_DURATION_TYPE_LAST_EIGHT_HOURS]  = "Last 8 Hours",
  [MARKET_PERIOD_DURATION_TYPE_LAST_TWELVE_HOURS] = "Last 12 Hours",
  [MARKET_PERIOD_DURATION_TYPE_LAST_DAY]          = "Last Day",
  [MARKET_PERIOD_DURATION_TYPE_LAST_TWO_DAYS]     = "Last Two Days",
  [MARKET_PERIOD_DURATION_TYPE_LAST_WEEK]         = "Last Week",
  [MARKET_PERIOD_DURATION_TYPE_LAST_MONTH]        = "Last Month",
  [MARKET_PERIOD_DURATION_TYPE_LAST_THREE_MONTHS] = "Last Three Months",
  [MARKET_PERIOD_DURATION_TYPE_LAST_SIX_MONTHS]   = "Last Six Months",
  [MARKET_PERIOD_DURATION_TYPE_LAST_YEAR]         = "Last Year",
};

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
static int market_period_duration_type_hours[] = {
  [MARKET_PERIOD_DURATION_TYPE_LAST_EIGHT_HOURS]  = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 2,
  [MARKET_PERIOD_DURATION_TYPE_LAST_TWELVE_HOURS] = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 3,
  [MARKET_PERIOD_DURATION_TYPE_LAST_DAY]          = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 6,
  [MARKET_PERIOD_DURATION_TYPE_LAST_TWO_DAYS]     = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 12,
  [MARKET_PERIOD_DURATION_TYPE_LAST_WEEK]         = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 42,
  [MARKET_PERIOD_DURATION_TYPE_LAST_MONTH]        = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 180,
  [MARKET_PERIOD_DURATION_TYPE_LAST_THREE_MONTHS] = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 540,
  [MARKET_PERIOD_DURATION_TYPE_LAST_SIX_MONTHS]   = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 1080,
  [MARKET_PERIOD_DURATION_TYPE_LAST_YEAR]         = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 2160,
};
struct list_args_t {
  bool all_items, enabled_items;
};
struct market_stats_t {
  unsigned long          max_volume, min_volume, max_open, min_open, max_close, min_close;
  struct Vector          *periods_v;
  struct market_period_t *newest_period, *oldest_period;
};
struct market_period_t {
  char          *ymdhm;
  unsigned int  year, month, day, hour, minute;
  float         open, high, low, close;
  unsigned long timestamp, age_ms, volume;
  timelib_time  *time;
  struct Vector *close_values[MARKET_PERIOD_DURATION_TYPES_QTY];
  struct stat_t {
    enum ema_applied_price_type_t applied_price_type;
    enum ema_shift_type_t         shift_type;
    struct avg_t {
      int   pos, length;
      float *buffer;
      long  sum;
      bool  is_filled;
    } *averages[MARKET_PERIOD_DURATION_TYPES_QTY];
  } *stats;
};
struct args_t {
  bool               verbose_mode, debug_mode;
  char               *file;
  struct list_args_t *list;
};

static int calculate_market_period_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur){
  p->stats->averages[dur]->sum                                  = p->stats->averages[dur]->sum - p->stats->averages[dur]->buffer[p->stats->averages[dur]->pos] + p->close;
  p->stats->averages[dur]->buffer[p->stats->averages[dur]->pos] = p->close;
  p->stats->averages[dur]->pos++;
  if (p->stats->averages[dur]->pos >= p->stats->averages[dur]->length) {
    p->stats->averages[dur]->pos       = 0;
    p->stats->averages[dur]->is_filled = true;
  }
  printf("is_filled %d", p->stats->averages[dur]->is_filled);
  return(p->stats->averages[dur]->sum / (p->stats->averages[dur]->is_filled ? p->stats->averages[dur]->length : p->stats->averages[dur]->pos));
}

static int new_marker_period_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur, int len){
  p->stats->averages[dur]            = calloc(1, sizeof(struct avg_t));
  p->stats->averages[dur]->sum       = 0;
  p->stats->averages[dur]->pos       = 0;
  p->stats->averages[dur]->length    = 0;
  p->stats->averages[dur]->is_filled = false;
  p->stats->averages[dur]->buffer    = calloc(len, sizeof(float));
  return(EXIT_SUCCESS);
}

static int release_market_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur){
  if (p->stats->averages[dur]->buffer) {
    free(p->stats->averages[dur]->buffer);
  }
  p->stats->averages[dur]->buffer = NULL;
  if (p->stats->averages[dur]) {
    free(p->stats->averages[dur]);
  }
  return(EXIT_SUCCESS);
}

static struct args_t *args = &(struct args_t){
  .verbose_mode = false,
  .debug_mode   = false,
  .list         = &(struct list_args_t){
    .all_items     = false,
    .enabled_items = false,
  },
};
static struct market_stats_t *market_stats = &(struct market_stats_t){
  .max_volume = 0,
  .periods_v  = NULL,
};

static int print_market_period(struct market_period_t *p){
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

static int new_market_period(char **items, size_t items_qty){
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
  p->time = timelib_strtotime(p->ymdhm, strlen(p->ymdhm), NULL, timelib_builtin_db(), timelib_parse_tzfile);
  timelib_update_ts(p->time, NULL);
  p->timestamp = p->time->sse;
  p->age_ms    = timestamp() - p->timestamp * 1000;
  vector_push(market_stats->periods_v, (void *)p);
  return(EXIT_SUCCESS);
} /* new_market_period */

static float avg_float_pointers_v(struct Vector *v){
  float total = 0;

  for (size_t i = 0; i < vector_size(v); i++) {
    total += *((float *)vector_get(v, i));
  }
  return((float)total / vector_size(v));
}

static struct Vector *get_past_market_period_closes_v(size_t period_index, size_t past_periods_qty){
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

static void _command_parse_csv(){
  size_t                 items_qty = 0;
  struct StringFNStrings csv_lines = stringfn_split_lines_and_trim(gmarket_csvData);
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

  fprintf(stderr, "Loaded %lu CSV Items\n\t\tCSV File Size: %s\n\t\tCSV Data Lines: %d\n\t\tTime Spent: %s\n", vector_size(market_stats->periods_v), bytes_to_string(gmarket_csvSize), csv_lines.count, milliseconds_to_string(timestamp() - started));
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
  fprintf(stderr, "Performed EMA Calculations for Average Close Price\n\t\tCalculations Performed:%lu\n\t\tQuantity EMA Duration Types: %d\n%s\n\t\tMarket Period Close Values Analyzed: %lu\n\t\tMarket Periods: %lu\n\t\tStart Date: %s\n\t\tEnd Date: %s\n\t\tDuration: %s\n\t\tTime Spent: %s\n",
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
} /* _command_parse_csv */
//    struct Vector *past_weekly_close_values = get_past_market_period_closes_v(i, market_period_duration_type_hours[MARKET_PERIOD_DURATION_TYPE_LAST_WEEK]/MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS);
/*
 * new_marker_period_moving_average(p, MARKET_PERIOD_DURATION_TYPE_LAST_DAY, 5);
 * for(int i = 0; i < (market_period_duration_type_hours[MARKET_PERIOD_DURATION_TYPE_LAST_DAY]/MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS); i++){
 *  int newAvg = calculate_market_period_moving_average(p, i);
 *  printf("The new average is %d\n", newAvg);
 * }
 *
 *
 * int sample[] = {50, 10, 20, 18, 20, 100, 18, 10, 13, 500, 50, 40, 10};
 * int newAvg = 0;
 * int count = sizeof(sample) / sizeof(int);
 * struct moving_average_t *sensor_av = allocate_moving_average(5);
 *
 * for(int i = 0; i < count; i++){
 *  newAvg = movingAvg(sensor_av, sample[i]);
 *  printf("The new average is %d\n", newAvg);
 * }
 */

static void _command_list(){
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

static void _command_debug_args(){
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

int main(int argc, char **argv) {
  market_stats->periods_v = vector_new();
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
      { END_OF_OPTIONS },
    },
    .subcommands         = (struct optparse_cmd[]) {
      {
        .description = "Print a subcommand's help information and quit.",
        .name        = "help",
        .operands    = "COMMAND",
        .about       = "Command Help",
        .function    = optparse_print_help_subcmd,
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
        .name        = "list",
        .description = "List",
        .function    = _command_list,
        .about       = "List Items",
        .options     = (struct optparse_opt[]){
          {
            .short_name  = 'e',
            .long_name   = "enabled",
            .description = "Enabled Items",
            .flag_type   = FLAG_TYPE_SET_TRUE,
            .flag        = &(args->list->enabled_items),
          },
          {
            .short_name  = 'a',
            .long_name   = "all",
            .description = "All Items",
            .flag_type   = FLAG_TYPE_SET_TRUE,
            .flag        = &(args->list->all_items),
          },
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
