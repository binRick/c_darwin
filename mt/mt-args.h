#pragma once
#ifndef MT_ARGS_INCLUDE_H
#define MT_ARGS_INCLUDE_H
#include "mt-include.h"
struct args_t;

struct args_t {
  bool verbose_mode, debug_mode;
  struct args_csv_t {
    const char             *name, *data, *start_date, *end_date;
    struct StringFNStrings lines;
    timelib_time           *start, *end;
  } *csv;
};
#endif
