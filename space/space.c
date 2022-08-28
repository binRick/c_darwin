#include "core-utils/core-utils.h"
#include "space.h"
#include <ApplicationServices/ApplicationServices.h>
#include <CoreServices/CoreServices.h>
#include <unistd.h>
#include "window-utils/window-utils.h"
#define g_connection    CGSMainConnectionID()

uint64_t *display_space_list(uint32_t did, int *count){
  uint64_t    *space_list = NULL;

  CFStringRef uuid = display_uuid(did);

  if (!uuid) {
    goto out;
  }

  CFArrayRef display_spaces_ref = SLSCopyManagedDisplaySpaces(g_connection);

  if (!display_spaces_ref) {
    goto err;
  }

  int display_spaces_count = CFArrayGetCount(display_spaces_ref);

  for (int i = 0; i < display_spaces_count; ++i) {
    CFDictionaryRef display_ref = CFArrayGetValueAtIndex(display_spaces_ref, i);
    CFStringRef     identifier  = CFDictionaryGetValue(display_ref, CFSTR("Display Identifier"));
    if (!CFEqual(uuid, identifier)) {
      continue;
    }

    CFArrayRef spaces_ref   = CFDictionaryGetValue(display_ref, CFSTR("Spaces"));
    int        spaces_count = CFArrayGetCount(spaces_ref);

    space_list = calloc(spaces_count, sizeof(uint64_t));
    *count     = spaces_count;

    for (int j = 0; j < spaces_count; ++j) {
      CFDictionaryRef space_ref = CFArrayGetValueAtIndex(spaces_ref, j);
      CFNumberRef     sid_ref   = CFDictionaryGetValue(space_ref, CFSTR("id64"));
      CFNumberGetValue(sid_ref, CFNumberGetType(sid_ref), &space_list[j]);
    }
  }

  CFRelease(display_spaces_ref);
err:
  CFRelease(uuid);
out:
  return(space_list);
} /* display_space_list */



bool window_id_is_minimized(int window_id){
  bool is_min = true;
    
  int space_minimized_window_qty;
 // uint32_t *minimized_window_list = space_minimized_window_list(i,&space_minimized_window_qty);
  return(is_min);
}

bool space_is_user(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 0);
}

bool space_is_fullscreen(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 4);
}

bool space_is_system(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 2);
}

bool space_is_visible(uint64_t sid){
  return(sid == display_space_id(space_display_id(sid)));
}

int get_space_via_keywin(void) {
  CFArrayRef winList =
    CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);

  int len = CFArrayGetCount(winList);
  int i, num = 0;

  for (i = 0; i < len; i++) {
    CFDictionaryRef winDict = CFArrayGetValueAtIndex(winList, i);
    if (CFDictionaryContainsKey(winDict, kCGWindowWorkspace)) {
      const void  *thing = CFDictionaryGetValue(winDict, kCGWindowWorkspace);
      CFNumberRef numRef = (CFNumberRef)thing;
      CFNumberGetValue(numRef, kCFNumberIntType, &num);
      return(num);
    }
  }
  return(-1);
}

int get_space_id(void){
  int        space;
  CFArrayRef windows = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
  CFIndex    i, n;

  for (i = 0, n = CFArrayGetCount(windows); i < n; i++) {
    CFDictionaryRef windict  = CFArrayGetValueAtIndex(windows, i);
    CFNumberRef     spacenum = CFDictionaryGetValue(windict, kCGWindowWorkspace);
    if (spacenum) {
      CFNumberGetValue(spacenum, kCFNumberIntType, &space);
      return(space);
    }
  }
  return(-1);
}


void set_space_by_index(int space){
  CFNotificationCenterRef nc     = CFNotificationCenterGetDistributedCenter();
  CFStringRef             numstr = CFStringCreateWithFormat(NULL, nil, CFSTR("%d"), space);

  CFNotificationCenterPostNotification(nc, CFSTR("com.apple.switchSpaces"), numstr, NULL, TRUE);
}


CGPoint display_center(uint32_t did){
  CGRect bounds = display_bounds(did);

  return((CGPoint) { bounds.origin.x + bounds.size.width / 2, bounds.origin.y + bounds.size.height / 2 });
}

