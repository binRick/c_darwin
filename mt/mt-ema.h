#pragma once
#ifndef MT_EMA_H
#define MT_EMA_H
#include "mt/mt-include.h"


float avg_float_pointers_v(struct Vector *v);
struct Vector *get_past_market_period_closes_v(size_t period_index,size_t past_periods_qty);
int print_market_period(struct market_period_t *p);
int new_market_period(char **items,size_t items_qty);
char *market_period_durations_csv(void);
int release_market_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur);

#endif
