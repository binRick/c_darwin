#pragma once
#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H
//////////////////////////////////////
#include "log/log.h"
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
#ifdef LOCAL_DEBUG_MODE
#define debug(M, ...)    {            \
    do {                              \
      if (LOCAL_DEBUG_MODE) {         \
        log_debug(M, ## __VA_ARGS__); \
      }                               \
    } while (0); }
#endif
#endif
