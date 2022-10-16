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
#include "clamp/clamp.h"
#include "hashmap.h/hashmap.h"
#include "log/log.h"
#include "ms/ms.h"
#include "optparse99/optparse99.h"
#include "timestamp/timestamp.h"
#include "whereami/src/whereami.h"
#include "wildcardcmp/wildcardcmp.h"
//////////////////////////////////////////////////////////////
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
//////////////////////////////////////////////////////////////
#include "app/utils/utils.h"
#include "display/utils/utils.h"
#include "dock/utils/utils.h"
//#include "image/utils/utils.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "table/utils/utils.h"
#include "window/utils/utils.h"
////#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/IOKitLib.h>
//////////////////////////////////////////////////////////////
struct offset_t {
  int left_offset, right_offset, top_offset, bottom_offset;
};
bool window_id_is_minimized(size_t window_id);
char *get_encoding_type_name(int encoding);

//////////////////////////////////////
struct whereami_report_t {
  const char *executable, *executable_directory, *executable_basename;
};
struct whereami_report_t *core_utils_whereami_report(void);
#endif
