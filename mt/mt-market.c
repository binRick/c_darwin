#pragma once
#ifndef MT_MARKET_C
#define MT_MARKET_C
#include "mt/mt-include.h"

int calculate_market_period_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur){
    p->stats->averages[dur]->sum                                  = p->stats->averages[dur]->sum - p->stats->averages[dur]->buffer[p->stats->averages[dur]->pos] + p->close;
    p->stats->averages[dur]->buffer[p->stats->averages[dur]->pos] = p->close;
    p->stats->averages[dur]->pos++;
    if (p->stats->averages[dur]->pos >= p->stats->averages[dur]->length) {
      p->stats->averages[dur]->pos       = 0;
      p->stats->averages[dur]->is_filled = true;
    }
    printf("is_filled %d", p->stats->averages[dur]->is_filled);
    return(p->stats->averages[dur]->sum / (p->stats->averages[dur]->is_filled ? p->stats  ->averages[dur]->length : p->stats->averages[dur]->pos));
  }

int new_marker_period_moving_average(struct market_period_t *p, enum ema_period_duration_type_t dur, int len){
    p->stats->averages[dur]            = calloc(1, sizeof(struct avg_t));
    p->stats->averages[dur]->sum       = 0;
    p->stats->averages[dur]->pos       = 0;
    p->stats->averages[dur]->length    = 0;
    p->stats->averages[dur]->is_filled = false;
    p->stats->averages[dur]->buffer    = calloc(len, sizeof(float));
    return(EXIT_SUCCESS);
  }


#endif
