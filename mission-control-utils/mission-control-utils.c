#pragma once
//#include <ApplicationServices/ApplicationServices.h>

#include <CoreServices/CoreServices.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>

#include "active-app/active-app.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "core/utils/utils.h"
#include "frameworks/frameworks.h"
#include "frameworks/frameworks.h"
#include "libfort/lib/fort.h"
#include "mission-control-utils/mission-control-utils.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process/process.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "space/utils/utils.h"
#include "submodules/log/log.h"
#include "timestamp/timestamp.h"
//#include <ApplicationServices/ApplicationServices.h>

#include <CoreServices/CoreServices.h>
static bool MISSION_CONTROL_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__mission_control_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_MISSION_CONTROL") != NULL) {
    log_debug("Enabling Mission Control Utils Debug Mode");
    MISSION_CONTROL_UTILS_DEBUG_MODE = true;
  }
}

///////////////////////////////////////////////////////////////////////////////
int mission_control_index(uint64_t sid) {
  uint64_t   result      = 0;
  int        desktop_cnt = 1;

  CFArrayRef display_spaces_ref   = SLSCopyManagedDisplaySpaces(g_connection);
  int        display_spaces_count = CFArrayGetCount(display_spaces_ref);

  for (int i = 0; i < display_spaces_count; ++i) {
    CFDictionaryRef display_ref  = CFArrayGetValueAtIndex(display_spaces_ref, i);
    CFArrayRef      spaces_ref   = CFDictionaryGetValue(display_ref, CFSTR("Spaces"));
    int             spaces_count = CFArrayGetCount(spaces_ref);

    for (int j = 0; j < spaces_count; ++j) {
      CFDictionaryRef space_ref = CFArrayGetValueAtIndex(spaces_ref, j);
      CFNumberRef     sid_ref   = CFDictionaryGetValue(space_ref, CFSTR("id64"));
      CFNumberGetValue(sid_ref, CFNumberGetType(sid_ref), &result);
      if (sid == result) {
        goto out;
      }

      ++desktop_cnt;
    }
  }

  desktop_cnt = 0;

out:
  CFRelease(display_spaces_ref);
  return(desktop_cnt);
}
