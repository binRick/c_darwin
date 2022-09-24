#pragma once
#ifndef TABLE_C
#define TABLE_C
////////////////////////////////////////////
#include "table/table.h"
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

////////////////////////////////////////////
static bool TABLE_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__table(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_table") != NULL) {
    log_debug("Enabling table Debug Mode");
    TABLE_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
