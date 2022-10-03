#pragma once
#ifndef MT_EMBEDDED_C
#define MT_EMBEDDED_C
#include "mt/mt-define.h"
#define INCBIN_PREFIX inc_
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#include "incbin/incbin.h"
INCTXT(market_csv, "assets/XRPUSD240.csv");

#undef INCBIN_PREFIX
#endif
