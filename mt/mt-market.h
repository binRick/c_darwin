#pragma once
#ifndef MT_MARKET_H
#define MT_MARKET_H
#include "mt/mt-include.h"

int print_market_period(struct market_period_t *p);
int release_market_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur);
struct Vector *get_past_market_period_closes_v(size_t period_index, size_t past_periods_qty);
int new_market_period(char **items, size_t items_qty);
char *market_period_durations_csv();
struct ma_t *get_ma_type_from_current_and_previous_market_period(enum ema_moving_average_type_t, struct market_period_t *p, struct market_period_t *prev_p);

#endif
