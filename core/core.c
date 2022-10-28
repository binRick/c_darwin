#pragma once
#ifndef CORE_C
#define CORE_C
////////////////////////////////////////////
#include "core/core.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

#include "roscha/include/hmap.h"
#include "roscha/include/object.h"
#include "roscha/include/parser.h"
#include "roscha/include/roscha.h"
#include "roscha/include/sds/sds.h"
#include "roscha/include/token.h"
#include "roscha/include/vector.h"
#include <termios.h>
////////////////////////////////////////////
static bool     CORE_DEBUG_MODE = false;
pthread_mutex_t *core_stdout_mutex;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__core(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_core") != NULL) {
    log_debug("Enabling core Debug Mode");
    CORE_DEBUG_MODE = true;
  }
  core_stdout_mutex = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(core_stdout_mutex, (void *)0);
}
////////////////////////////////////////////
#endif
