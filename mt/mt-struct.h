#pragma once
#ifndef MT_STRUCT_H
#define MT_STRUCT_H
#include "mt/mt-enum.h"
#include "mt/mt-utils.h"
struct market_period_t;
struct args_t;
struct list_args_t {
  bool all_items, enabled_items;
};
struct market_stats_t {
  unsigned long          max_volume, min_volume, max_open, min_open, max_close, min_close;
  float                  max_ma, min_ma;
  struct Vector          *periods_v;
  struct market_period_t *newest_period, *oldest_period;
  struct {
    unsigned long close_periods, average, moving_average, total;
  }                      time_spent;
};
struct market_period_t {
  char          *ymdhm;
  unsigned int  year, month, day, hour, minute;
  float         open, high, low, close;
  unsigned long timestamp, age_ms, volume;
  timelib_time  *time;
  struct Vector *close_values[MARKET_PERIOD_DURATION_TYPES_QTY];
  float         averages[MARKET_PERIOD_DURATION_TYPES_QTY];
  struct moving_average_t {
    float  sum, average;
    float  *values;
    size_t qty;
  } *moving_averages[MARKET_PERIOD_DURATION_TYPES_QTY];
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

#endif
