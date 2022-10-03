#pragma once
#ifndef MT_MARKET_H
#define MT_MARKET_H
#include "mt/mt-include.h"


extern int calculate_market_period_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur);
extern int new_marker_period_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur, int len);

#endif
