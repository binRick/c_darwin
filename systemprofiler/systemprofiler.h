#pragma once
#ifndef SYSTEMPROFILER_H
#define SYSTEMPROFILER_H
//////////////////////////////////////
#include "iowow/src/fs/iwfile.h"
#include "iowow/src/iowow.h"
#include "iowow/src/kv/iwkv.h"
#include "iowow/src/log/iwlog.h"
#include "iowow/src/platform/iwp.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
char *run_system_profiler_item_subprocess(char *ITEM_NAME, size_t CACHE_TTL);
struct db_cache_item_t {
  IWDB      iwdb;
  IWKV      iwkv;
  IWKV_OPTS opts;
};
#endif
