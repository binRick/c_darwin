#include "mt/mt.h"
#include "timelib/timelib.h"
#include "csv_parser/csv.h"
#ifdef INCBIN_STYLE 
#undef INCBIN_STYLE
#endif
#ifdef INCBIN_PREFIX
#undef INCBIN_PREFIX
#endif
#define MAX_CSV_LINE_LENGTH 1024
#define MIN_CSV_LINE_LENGTH CSV_COLUMNS_QTY*2
#include "incbin/incbin.h"
#define MARKET_CSV_FILE "XRPUSD240.csv"
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
static int market_period_duration_type_hours[] = {
  [MARKET_PERIOD_DURATION_TYPE_LAST_EIGHT_HOURS] = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 2,
  [MARKET_PERIOD_DURATION_TYPE_LAST_TWELVE_HOURS] = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 3,
  [MARKET_PERIOD_DURATION_TYPE_LAST_DAY] = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 6,
  [MARKET_PERIOD_DURATION_TYPE_LAST_TWO_DAYS] = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 12,
  [MARKET_PERIOD_DURATION_TYPE_LAST_WEEK] = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 42,
  [MARKET_PERIOD_DURATION_TYPE_LAST_MONTH] = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 180,
  [MARKET_PERIOD_DURATION_TYPE_LAST_THREE_MONTHS] = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 540,
  [MARKET_PERIOD_DURATION_TYPE_LAST_SIX_MONTHS] = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 1080,
  [MARKET_PERIOD_DURATION_TYPE_LAST_YEAR] = MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS * 2160,
};
struct list_args_t {
  bool all_items, enabled_items;
};
struct market_stats_t {
  unsigned long max_volume, min_volume, max_open, min_open, max_close, min_close;
  struct Vector *periods_v;
  struct market_period_t *newest_period, *oldest_period;
};
struct market_period_t {
  unsigned int year, month, day, hour, minute;
  float open, high, low, close;
  unsigned long timestamp, age_ms, volume;
  timelib_time *time;
  struct stat_t {
    enum ema_applied_price_type_t applied_price_type;
    enum ema_shift_type_t shift_type;
    struct avg_t {
        int pos, length;
        float *buffer;
        long sum;
        bool is_filled;
    } *averages[MARKET_PERIOD_DURATION_TYPES_QTY];
  } *stats;
};
struct args_t {
  bool               verbose_mode, debug_mode;
  char *file;
  struct list_args_t *list;
};
static int calculate_market_period_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur){
  p->stats->averages[dur]->sum = p->stats->averages[dur]->sum - p->stats->averages[dur]->buffer[p->stats->averages[dur]->pos] + p->close;
  p->stats->averages[dur]->buffer[p->stats->averages[dur]->pos] = p->close;
  p->stats->averages[dur]->pos++;
  if (p->stats->averages[dur]->pos >= p->stats->averages[dur]->length){
    p->stats->averages[dur]->pos = 0;
    p->stats->averages[dur]->is_filled = true;
  }
  printf("is_filled %d", p->stats->averages[dur]->is_filled);
  return p->stats->averages[dur]->sum / (p->stats->averages[dur]->is_filled ? p->stats->averages[dur]->length : p->stats->averages[dur]->pos);
}

static int new_marker_period_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur, int len){
    p->stats->averages[dur] = calloc(1,sizeof(struct avg_t));
    p->stats->averages[dur]->sum       = 0;
    p->stats->averages[dur]->pos = 0;
    p->stats->averages[dur]->length = 0;
    p->stats->averages[dur]->is_filled = false;
    p->stats->averages[dur]->buffer = calloc(len, sizeof(float));
    return(EXIT_SUCCESS);
}

static int release_market_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur){
  if(p->stats->averages[dur]->buffer)
    free(p->stats->averages[dur]->buffer);
  p->stats->averages[dur]->buffer = NULL;
  if(p->stats->averages[dur])
    free(p->stats->averages[dur]);
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
  .periods_v = NULL,
};
static int print_market_period(struct market_period_t *p){
    fprintf(stderr,
              " [%.4d-%.2d-%.2d %.2d:%.2d] (" AC_GREEN AC_BOLD "%3s ago" AC_RESETALL ")"
                            "|Open:"AC_CYAN"%.3f"AC_RESETALL"|Close:"AC_MAGENTA"%.3f"AC_RESETALL 
                            "|High:"AC_GREEN"%.3f"AC_RESETALL"|Low:"AC_RED"%.3f"AC_RESETALL
                            "|Volume:"AC_YELLOW"%.*ld"AC_RESETALL
                            "|\n", 
              p->year, p->month, p->day, p->hour, p->minute,milliseconds_to_string(p->age_ms),
              p->open,p->close,
              p->high,p->low,
              (int)(market_stats->max_volume%10)+1, p->volume
    );
}
static int new_market_period(char **items, size_t items_qty){
  char *item;
  struct market_period_t *p = calloc(1,sizeof(struct market_period_t));
  char *year_month_day_hour_minute;
  if(items_qty!= CSV_COLUMNS_QTY){
    log_error("Invalid CSV Line (%lu items)", items_qty);
    return(EXIT_FAILURE);
  }

  struct StringFNStrings s;
  for(size_t i = 0; i < CSV_COLUMNS_QTY; i++){
    item = stringfn_trim(items[i]);
    switch(i){
      case CSV_COLUMN_YEAR_MONTH_DAY: 
        s = stringfn_split(item, '.');
        p->year = atoi(s.strings[0]);
        p->month = atoi(s.strings[1]);
        p->day = atoi(s.strings[2]);
        stringfn_release_strings_struct(s);
      break;
      case CSV_COLUMN_HOUR_MINUTE: 
        s = stringfn_split(item, ':');
        p->hour = atoi(s.strings[0]);
        p->minute = atoi(s.strings[1]);
        stringfn_release_strings_struct(s);
      break;
      case CSV_COLUMN_OPEN: p->open = atof(item);break;
      case CSV_COLUMN_HIGH: p->high = atof(item);break;
      case CSV_COLUMN_LOW: p->low = atof(item);break;
      case CSV_COLUMN_CLOSE: p->close = atof(item);break;
      case CSV_COLUMN_VOLUME: p->volume = atoi(item);break;
    }
  }
  market_stats->max_volume = (p->volume > market_stats->max_volume) ? p->volume : market_stats->max_volume;
  asprintf(&year_month_day_hour_minute,"%.4d-%.2d-%.2d %.2d:%.2d", p->year,p->month,p->day,p->hour,p->minute);
  p->time = timelib_strtotime(year_month_day_hour_minute, strlen(year_month_day_hour_minute), NULL, timelib_builtin_db(), timelib_parse_tzfile);
  timelib_update_ts(p->time, NULL);
  p->timestamp = p->time->sse;
  p->age_ms = timestamp() - p->timestamp * 1000;
  vector_push(market_stats->periods_v,(void*)p);
  return(EXIT_SUCCESS);
}

static void _command_parse_csv(){
  size_t items_qty = 0;
  struct StringFNStrings csv_lines = stringfn_split_lines_and_trim(gmarket_csvData);
  char **items = NULL, **buf;
  unsigned long started = timestamp();
  for(int i = 1; i < csv_lines.count; i++){
    if(strlen(csv_lines.strings[i])<MIN_CSV_LINE_LENGTH||strlen(csv_lines.strings[i])>MAX_CSV_LINE_LENGTH)continue;
    items = parse_csv(csv_lines.strings[i]);
    items_qty = 0;
    buf = items;
    while(*buf++)items_qty++;
    assert(new_market_period(items,items_qty) == EXIT_SUCCESS);
  }

  log_info("Loaded %lu csv items in %s|Max Volume:%ld|",vector_size(market_stats->periods_v), milliseconds_to_string(timestamp() - started), market_stats->max_volume);
  for(size_t i=0;i<vector_size(market_stats->periods_v);i++){
    struct market_period_t *p = (struct market_period_t*)vector_get(market_stats->periods_v,i);
    /*
    new_marker_period_moving_average(p, MARKET_PERIOD_DURATION_TYPE_LAST_DAY, 5);
    for(int i = 0; i < (market_period_duration_type_hours[MARKET_PERIOD_DURATION_TYPE_LAST_DAY]/MARKET_PERIOD_LENGTH_TYPE_FOUR_HOURS); i++){
        int newAvg = calculate_market_period_moving_average(p, i);
        printf("The new average is %d\n", newAvg);
    }
    */
    if(args->debug_mode)
      print_market_period(p);
  }


   int sample[] = {50, 10, 20, 18, 20, 100, 18, 10, 13, 500, 50, 40, 10};
   int newAvg = 0;
   int count = sizeof(sample) / sizeof(int);
/*   struct moving_average_t *sensor_av = allocate_moving_average(5);

    for(int i = 0; i < count; i++){
        newAvg = movingAvg(sensor_av, sample[i]);
        printf("The new average is %d\n", newAvg);
    }
*/

  exit(EXIT_SUCCESS);
}
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
