#include "display-utils/display-utils.h"
#include "frameworks/frameworks.h"
#include "log.h/log.h"
#include "string-utils/string-utils.h"
static bool DISPLAY_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__display_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_DISPLAY_UTILS") != NULL) {
    log_debug("Enabling Table Utils Debug Mode");
    DISPLAY_UTILS_DEBUG_MODE = true;
  }
}

////////////////////////////////////////////
uint32_t get_main_display_id(void){
  return(CGMainDisplayID());
}

char *get_active_display_uuid(void){
  return(cfstring_copy(get_active_display_uuid_ref()));
}

CFStringRef get_active_display_uuid_ref(void){
  return(SLSCopyActiveMenuBarDisplayIdentifier(g_connection));
}

uint32_t get_active_display_id(void){
  CFStringRef uuid = get_active_display_uuid();

  assert(uuid);
  uint32_t result = get_display_id(uuid);

  CFRelease(uuid);
  return(result);
}

uint32_t *get_display_list(uint32_t *count) {
  int      display_count = get_active_display_count();
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

int get_active_display_count(void) {
  uint32_t count;

  CGGetActiveDisplayList(0, NULL, &count);
  return((int)count);
}

CGPoint get_display_center(uint32_t did){
  CGRect bounds = get_display_rect(did);

  return((CGPoint){ bounds.origin.x + bounds.size.width / 2, bounds.origin.y + bounds.size.height / 2 });
}

CGRect get_display_rect(uint32_t did){
  return(CGDisplayBounds(did));
}

char *get_display_uuid(uint32_t did){
  return(cfstring_copy(get_display_uuid_ref(did)));
}

CFStringRef get_display_uuid_ref(uint32_t did){
  CFUUIDRef uuid_ref = CGDisplayCreateUUIDFromDisplayID(did);

  if (!uuid_ref) {
    return(NULL);
  }

  CFStringRef uuid_str = CFUUIDCreateString(NULL, uuid_ref);

  CFRelease(uuid_ref);

  return(uuid_str);
}

int get_display_id(CFStringRef uuid){
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
