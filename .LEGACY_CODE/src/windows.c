/**********************************/
#include "../include/includes.h"


int get_windows_qty(){
  int             qty = 0;
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
    qty++;
  }
  return(qty);
}


/**********************************/
void iterate_windows(){
  tq_start("iterate_windows duration");
  CFDictionaryRef window;

  CFArrayRef      windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );
  int windows_qty = 0;
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
    log_info(
      AC_RESETALL "| " AC_RESETALL
      AC_RESETALL AC_BOLD AC_BRIGHT_GREEN_BLACK AC_UNDERLINE "%s" AC_RESETALL
      AC_RESETALL " | " AC_RESETALL
      AC_RESETALL AC_REVERSED AC_BRIGHT_CYAN_BLACK "%s" AC_RESETALL
      AC_RESETALL " |" AC_RESETALL
      "",
      appName,
      str_truncate(windowName, 30)
      );
    windows_qty++;
  }
  log_info("%d windows | %s", windows_qty, tq_stop("iterate_windows duration"));
} /* iterate_windows */

