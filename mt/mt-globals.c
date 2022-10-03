#pragma once
#ifndef MT_GLOBALS_C
#define MT_GLOBALS_C
#include "mt-include.h"

struct market_stats_t *market_stats = &(struct market_stats_t){
    .max_volume = 0,
    .periods_v  = NULL,
};

struct args_t *args = &(struct args_t){
    .verbose_mode = false,
    .debug_mode   = false,
    .list         = &(struct list_args_t){
      .all_items     = false,
      .enabled_items = false,
    },
};

const int market_period_duration_type_hours[] = {
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
const char *market_period_duration_type_names[] = {
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

#endif
