#pragma once
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "core-utils/core-utils.h"
#include "djbhash/src/djbhash.h"
#include "frameworks/frameworks.h"
#include "hash/hash.h"
#include "iokit-utils/iokit-utils.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "osx-keys/osx-keys.h"
#include "parson/parson.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"
#include <ApplicationServices/ApplicationServices.h>
#include <bsm/libbsm.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CoreFoundation.h>
#include <errno.h>
#include <libgen.h>
#include <libproc.h>
#include <pthread.h>
#include <pwd.h>
#include <security/audit/audit_ioctl.h>
#include <signal.h>
#include <signal.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <sys/syslimits.h>
#include <time.h>
#include <unistd.h>
static bool CORE_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__core_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_CORE_UTILS") != NULL) {
    log_debug("Enabling Core Utils Debug Mode");
    CORE_UTILS_DEBUG_MODE = true;
  }
}

bool window_id_is_minimized(size_t window_id){
  return(false);

  struct window_info_t *W = get_window_id_info(window_id);
  AXUIElementRef       app = AXWindowFromCGWindow(W->window);    CFBooleanRef boolRef;
  if (AXUIElementCopyAttributeValue(app, kAXMinimizedAttribute, &boolRef) != kAXErrorSuccess) {
    log_error("Failed to copy attrs");
    return(false);
  }
  return(CFBooleanGetValue(boolRef));
}
