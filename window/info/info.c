#pragma once
#ifndef WINDOW_INFO_C
#define WINDOW_INFO_C
////////////////////////////////////////////
#include "core/core.h"
#include "window/info/info.h"
////////////////////////////////////////////
#include "active-app/active-app.h"
#include "ansi-codes/ansi-codes.h"
#include "app/utils/utils.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "space/utils/utils.h"
#include "submodules/log/log.h"
#include "table/utils/utils.h"
static bool WINDOW_INFO_DEBUG_MODE = false;
////////////////////////////////////////////
static int  window_event_types[] = {
  kEventWindowHidden,
  kEventWindowActivated,
  kEventWindowBoundsChanged,
  kEventWindowResizeStarted,
  kEventWindowResizeCompleted,
  kEventWindowClose,
  kEventWindowCollapsed,
  kEventWindowDeactivated,
};
static void __attribute__((constructor)) __constructor__window_info(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_WINDOW_INFO") != NULL) {
    log_debug("Enabling window-info Debug Mode");
    WINDOW_INFO_DEBUG_MODE = true;
  }
}
#endif
