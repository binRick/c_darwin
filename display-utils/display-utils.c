#include "core-utils/core-utils-extern.h"
#include "display-utils/display-utils.h"

////////////////////////////////////////////

uint32_t *display_active_display_list(uint32_t *count) {
  int      display_count = display_active_display_count();
  uint32_t *result       = malloc(sizeof(uint32_t) * display_count);

  CGGetActiveDisplayList(display_count, result, count);
  return(result);
}

void get_display_bounds(int *x, int *y, int *w, int *h){
  CGEventRef event          = CGEventCreate(NULL);
  CGPoint    cursorLocation = CGEventGetLocation(event);

  CFRelease(event);
  int numDisplays; CGDirectDisplayID displays[16];

  CGGetDisplaysWithPoint(cursorLocation, 16, displays, &numDisplays);
  HIRect bounds;

  HIWindowGetAvailablePositioningBounds(displays[0], kHICoordSpace72DPIGlobal, &bounds);
  *x = bounds.origin.x;
  *y = bounds.origin.y;
  *w = bounds.size.width;
  *h = bounds.size.height;
}

struct Vector *get_display_ids_v(){
  struct Vector     *ids                = vector_new();
  size_t            displays_qty        = 0;
  CGDirectDisplayID *display_ids        = calloc(MAX_DISPLAYS, sizeof(CGDirectDisplayID));
  CGError           get_displays_result = CGGetActiveDisplayList(UCHAR_MAX, display_ids, &displays_qty);

  if (get_displays_result == kCGErrorSuccess) {
    for (size_t i = 0; i < displays_qty && i < MAX_DISPLAYS; i++) {
      size_t display_id = (size_t)display_ids[i];
      if (display_id > 0) {
        vector_push(ids, (void *)display_id);
      }
    }
  }
  return(ids);
}

uint32_t display_active_display_count(void) {
  uint32_t count;

  CGGetActiveDisplayList(0, NULL, &count);
  return(count);
}

CGPoint display_center(uint32_t did){
  CGRect bounds = display_bounds(did);

  return((CGPoint){ bounds.origin.x + bounds.size.width / 2, bounds.origin.y + bounds.size.height / 2 });
}

CGRect display_bounds(uint32_t did){
  return(CGDisplayBounds(did));
}

CFStringRef display_uuid(uint32_t did){
  CFUUIDRef uuid_ref = CGDisplayCreateUUIDFromDisplayID(did);

  if (!uuid_ref) {
    return(NULL);
  }

  CFStringRef uuid_str = CFUUIDCreateString(NULL, uuid_ref);

  CFRelease(uuid_ref);

  return(uuid_str);
}

uint32_t display_id(CFStringRef uuid){
  CFUUIDRef uuid_ref = CFUUIDCreateFromString(NULL, uuid);

  if (!uuid_ref) {
    return(0);
  }

  uint32_t did = CGDisplayGetDisplayIDFromUUID(uuid_ref);

  CFRelease(uuid_ref);

  return(did);
}

int get_display_height(){
  int    h             = -1;
  CGRect displayBounds = CGDisplayBounds(CGMainDisplayID());

  h = displayBounds.size.height;
  return(h);
}

int get_display_width(){
  int    w             = -1;
  CGRect displayBounds = CGDisplayBounds(CGMainDisplayID());

  w = displayBounds.size.width;
  return(w);
}
