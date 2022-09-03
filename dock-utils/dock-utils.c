#include "core-utils/core-utils-extern.h"
#include "display-utils/display-utils.h"
#include "dock-utils/dock-utils.h"
#include "log.h/log.h"
static bool DOCK_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__dock_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_DOCK_UTILS") != NULL) {
    log_debug("Enabling Dock Utils Debug Mode");
    DOCK_UTILS_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
const char *dock_orientation_names[DOCK_ORIENTATIONS_QTY] = {
  [DOCK_ORIENTATION_TOP]    = "top",
  [DOCK_ORIENTATION_BOTTOM] = "bottom",
  [DOCK_ORIENTATION_LEFT]   = "left",
  [DOCK_ORIENTATION_RIGHT]  = "right",
};

bool dock_is_visible(void){
  return(!CoreDockGetAutoHideEnabled());
}

char *dock_orientation_name(void){
  return(dock_orientation_names[dock_orientation()]);
}

int dock_orientation(void){
  int pinning     = 0;
  int orientation = 0;

  CoreDockGetOrientationAndPinning(&orientation, &pinning);
  return(orientation);
}

CGRect dock_rect(void){
  int    reason = 0;
  CGRect bounds = {};

  SLSGetDockRectWithReason(g_connection, &bounds, &reason);
  return(bounds);
}

CGSize dock_offset(void){
  if (dock_is_visible() == false) {
    return(CGSizeMake(0, 0));
  }
  CGRect rect = dock_rect();
  CGSize size = CGSizeMake(
    rect.size.width,
    rect.size.height - get_display_height()
    );
  return(size);
}
