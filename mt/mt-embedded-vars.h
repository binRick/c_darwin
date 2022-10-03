#pragma once
#ifndef MT_EMBEDDED_VARS
#define MT_EMBEDDED_VARS
#include "mt/mt-embedded.h"
static const size_t EMBEDDED_CSV_VARS_QTY, EMBEDDED_CSV_NAMES_QTY = 4;
static const char   *EMBEDDED_CSV_VARS[4] = {
  inc_xrpusd_columns_csv_data,
  inc_xrpusd240_csv_data,
  inc_xrpusd_2020_csv_data,
  inc_xrpusd_2021_csv_data,
};
static const char   *EMBEDDED_CSV_NAMES[4] = {
  "xrpusd_columns_csv",
  "xrpusd240_csv",
  "xrpusd_2020_csv",
  "xrpusd_2021_csv",
};

#endif
