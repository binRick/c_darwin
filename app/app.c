#pragma once
#ifndef APP_C
#define APP_C
#include "ansi-codes/ansi-codes.h"
#include "app/app.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
////////////////////////////////////////////
const char       *app_type_names[APP_TYPES_QTY + 1] = {
  [APP_TYPE_APPLE]          = "Apple",
  [APP_TYPE_ABSENT]         = "Absent",
  [APP_TYPE_EXISTANT]       = "Existant",
  [APP_TYPE_SYSTEM]         = "System",
  [APP_TYPE_SYSTEM_LIBRARY] = "System Library",
  [APP_TYPE_USER_LIBRARY]   = "User Library",
  [APP_TYPE_APPLICATIONS]   = "Applications",
  [APP_TYPE_IDENTIFIED]     = "Identified",
  [APP_TYPE_UNVERSIONED]    = "Unversioned",
  [APP_TYPE_VERSIONED]      = "Versioned",
  [APP_TYPES_QTY]           = NULL,
};
static const int app_event_types[] = {
  kEventAppActivated,
  kEventAppDeactivated,
};
static bool      APP_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__app(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_APP") != NULL) {
    log_debug("Enabling app Debug Mode");
    APP_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
