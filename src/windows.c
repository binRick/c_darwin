/**********************************/
#include "../include/includes.h"


/**********************************/


void iterate_windows(){
  CFArrayRef      windowList;
  unsigned int    windows_qty;
  CFDictionaryRef window;

  windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );
  windows_qty = 0;
  int layer;

  for (int i = 0; i < CFArrayGetCount(windowList); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);
    layer  = CFDictionaryGetInt(window, kCGWindowLayer);
    if (layer > 0) {
      continue;
    }
    char *appName = CFDictionaryCopyCString(window, kCGWindowOwnerName);
    if (!appName || !*appName) {
      continue;
    }
    char *windowName = CFDictionaryCopyCString(window, kCGWindowName);
    if (!windowName) {
      continue;
    }
    char *title = windowTitle(appName, windowName);
    if (!title) {
      continue;
    }
    log_info("title:%s|name:%s|app:%s", title, windowName, appName);
    windows_qty++;
  }
  log_info("%d windows", windows_qty);
}
