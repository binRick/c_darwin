#pragma once
#ifndef CORE_UTILS_H
#define CORE_UTILS_H
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
#include "core/core.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "b64.c/b64.h"
#include "bytes/bytes.h"
#include "c_eventemitter/include/eventemitter.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "cbars/include/cbars/cbars.h"
#include "hashmap.h/hashmap.h"
#include "log/log.h"
#include "ms/ms.h"
#include "optparse99/optparse99.h"
#include "timestamp/timestamp.h"
#include "whereami/src/whereami.h"
#include "wildcardcmp/wildcardcmp.h"
#include "clamp/clamp.h"
//////////////////////////////////////
struct whereami_report_t {
  const char *executable, *executable_directory, *executable_basename;
};
struct whereami_report_t *core_utils_whereami_report(void);
#endif
