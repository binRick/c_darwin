#include "core-utils/core-utils-extern.h"
#include "display-utils/display-utils.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "space-utils/space-utils.h"

////////////////////////////////////////////
int get_menu_bar_height(){
  if (menu_bar_visible() == false) {
    return(0);
  }
  CGRect rect = menu_bar_rect(display_id_for_space(get_space_id()));
  return((int)rect.size.height);
}

CGRect menu_bar_rect(uint32_t did) {
  CGRect bounds = {};

  SLSGetRevealedMenuBarBounds(&bounds, g_connection, display_space_id(did));
  return(bounds);
}

bool menu_bar_visible(void) {
  int status = 0;

  SLSGetMenuBarAutohideEnabled(g_connection, &status);
  return(!status);
}
