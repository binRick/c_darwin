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
#include "core-utils/core-utils-extern.h"
#include "display-utils/display-utils.h"
#include "dock-utils/dock-utils.h"
#include "process/process.h"
#include "system-utils/system-utils.h"
#include "window-utils/window-utils.h"
//////////////////////////////////////////////////////////////
struct list_window_table_t {
  struct Vector *windows_v;
  bool          current_space_only;
};
struct window_t {
  size_t              window_id;
  pid_t               pid;
  CGPoint             position;
  CFTypeRef           app_window_list;
  size_t              app_window_list_qty;
  int                 pos_x, pos_y, width, height, space_id, connection_id, display_id, layer, display_index, level;
  CFNumberRef         layer_ref;
  CGSize              size;
  CFDictionaryRef     window;
  CGRect              rect;
  size_t              memory_usage;
  char                *app_name, *window_name, *window_title, *owner_name, *uuid, *display_uuid;
  char                pid_path[PATH_MAX];
  bool                is_focused, is_visible, is_minimized, can_move, can_minimize, can_resize, is_popover, is_onscreen;
  struct kinfo_proc   pid_info;
  struct Vector       *space_ids_v, *child_pids_v, *window_ids_above, *window_ids_below;
  AXUIElementRef      *app;
  ProcessSerialNumber psn;
  unsigned long       dur, started;
};
