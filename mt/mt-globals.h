#pragma once
#ifndef MT_GLOBALS_H
#define MT_GLOBALS_H
#include "mt/mt-include.h"

extern const char            *market_period_duration_type_names[];
extern const size_t          market_period_duration_type_hours[];
extern const size_t          market_period_duration_type_periods_qty[];
extern struct args_t         *args;
extern struct market_stats_t *market_stats;

#endif
