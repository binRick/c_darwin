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
#include "app-utils/app-utils.h"
#include "display-utils/display-utils.h"
#include "dock-utils/dock-utils.h"
#include "image-utils/image-utils.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "mission-control-utils/mission-control-utils.h"
#include "process-utils/process-utils.h"
#include "process/process.h"
#include "system-utils/system-utils.h"
#include "table-utils/table-utils.h"
#include "window-utils/window-utils.h"
//////////////////////////////////////////////////////////////
struct offset_t {
  int left_offset, right_offset, top_offset, bottom_offset;
};
