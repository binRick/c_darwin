#include "display/utils/utils.h"
#include "frameworks/frameworks.h"
#include "log.h/log.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "space/utils/utils.h"
static bool MENU_BAR_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__menu_bar_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_MENU_BAR_UTILS") != NULL) {
    log_debug("Enabling Menu Bar Utils Debug Mode");
    MENU_BAR_UTILS_DEBUG_MODE = true;
  }
}

////////////////////////////////////////////
int get_menu_bar_height(){
  if (get_menu_bar_visible() == false) {
    return(0);
  }
  int    _space_id   = get_current_space_id();
  int    _display_id = get_display_id_for_space(_space_id);
  CGRect rect        = get_menu_bar_rect(_display_id);
  return((int)rect.size.height);
}

CGRect get_menu_bar_rect(int display_id) {
  CGRect bounds    = {};
  int    _space_id = get_display_space_id(display_id);

  SLSGetRevealedMenuBarBounds(&bounds, g_connection, _space_id);
  return(bounds);
}

bool get_menu_bar_visible(void) {
  int status = 0;

  SLSGetMenuBarAutohideEnabled(g_connection, &status);
  return(!status);
}
