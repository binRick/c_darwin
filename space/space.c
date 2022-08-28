#include "space/space.h"
/*
 * void set_space(long space) {
 *      CGSConnection conn = _CGSDefaultConnection();
 *      CGSSetWorkspace(conn, space);
 * }*/

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
