#pragma once
#ifndef WINDOW_INFO_C
#define WINDOW_INFO_C
////////////////////////////////////////////
#include "window/info/info.h"
////////////////////////////////////////////
#include "active-app/active-app.h"
#include "ansi-codes/ansi-codes.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "frameworks/frameworks.h"
#include "libfort/lib/fort.h"
#include "log/log.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "space/utils/utils.h"
#include "string-utils/string-utils.h"
#include "submodules/log.h/log.h"
#include "table/utils/utils.h"
#include "timestamp/timestamp.h"
static bool WINDOW_INFO_DEBUG_MODE = false;
////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__window_info(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_window_info") != NULL) {
    log_debug("Enabling window-info Debug Mode");
    WINDOW_INFO_DEBUG_MODE = true;
  }
}
#endif
