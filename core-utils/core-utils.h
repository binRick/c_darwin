#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/IOKitLib.h>
//////////////////////////////////////////////////////////////
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
//////////////////////////////////////////////////////////////
#include "app/utils/utils.h"
#include "display/utils/utils.h"
#include "dock/utils/utils.h"
#include "image/utils/utils.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "mission-control-utils/mission-control-utils.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "process/utils/utils.h"
#include "table/utils/utils.h"
#include "window/utils/utils.h"
//////////////////////////////////////////////////////////////
struct offset_t {
  int left_offset, right_offset, top_offset, bottom_offset;
};
bool window_id_is_minimized(size_t window_id);
