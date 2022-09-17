#include "core-utils/core-utils.h"
#include "display-utils/display-utils.h"
#include "log.h/log.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
static bool SPACE_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__space_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_SPACE_UTILS") != NULL) {
    log_debug("Enabling Space Utils Debug Mode");
    SPACE_UTILS_DEBUG_MODE = true;
  }
}

struct Vector *get_spaces_v(){
  struct Vector *spaces_v    = vector_new();
  struct Vector *space_ids_v = get_space_ids_v();
  int           cur_space_id = get_space_id();

  for (size_t i = 0; i < vector_size(space_ids_v); i++) {
    size_t         space_id = (size_t)vector_get(space_ids_v, i);
    struct space_t *space   = calloc(1, sizeof(struct space_t));
    space->id           = space_id;
    space->is_current   = (cur_space_id == space->id);
    space->window_ids_v = get_space_window_ids_v(space->id);
    vector_push(spaces_v, (void *)space);
  }
  return(spaces_v);
}
struct Vector *get_space_ids_v(){
  struct Vector *ids  = vector_new();
  struct Vector *dids = get_display_ids_v();

  for (size_t i = 0; i < vector_size(dids); i++) {
    struct Vector *dspace_ids_v = get_display_id_space_ids_v((uint32_t)(size_t)vector_get(dids, i));
    for (size_t j = 0; j < vector_size(dspace_ids_v); j++) {
      size_t dspace_id = (size_t)vector_get(dspace_ids_v, j);
      vector_push(ids, (void *)dspace_id);
    }
  }

  return(ids);
}

struct Vector *get_space_non_minimized_window_ids_v(size_t space_id){
  struct Vector *ids = vector_new();

  int           window_qty = 0;
  uint64_t      sid        = (uint64_t)space_id;

  log_info("getting non min space %lld window list....", sid);
  uint32_t *windows_list = get_space_window_list_for_connection(&sid, 1, 0, &window_qty, false);

  for (int i = 0; i < window_qty; i++) {
    vector_push(ids, (void *)(size_t)windows_list[i]);
  }
  return(ids);
}
struct Vector *get_space_minimized_window_ids_v(size_t space_id){
  struct Vector *ids = vector_new();

  log_debug("space id %lu", space_id);

  int      window_qty    = 0;
  uint64_t sid           = (uint64_t)space_id;
  uint32_t *windows_list = get_space_minimized_window_list(sid, &window_qty);

  for (int i = 0; i < window_qty; i++) {
    vector_push(ids, (void *)(size_t)windows_list[i]);
  }
  return(ids);
}
struct Vector *get_space_window_ids_v(size_t space_id){
  struct Vector *ids          = vector_new();
  int           window_qty    = 0;
  uint64_t      sid           = (uint64_t)space_id;
  uint32_t      *windows_list = get_space_window_list_for_connection(&sid, 1, 0, &window_qty, true);

  for (int i = 0; i < window_qty; i++) {
    vector_push(ids, (void *)(size_t)windows_list[i]);
  }
  return(ids);
}

struct Vector *get_display_id_space_ids_v(uint32_t did){
  struct Vector *display_space_ids_v = vector_new();
  CFStringRef   uuid                 = get_display_uuid_ref(did);

  if (!uuid) {
    log_error("Unable to determine uuid of display id %d", did);
    return(display_space_ids_v);
  }
  CFArrayRef display_spaces_ref = SLSCopyManagedDisplaySpaces(g_connection);

  if (!display_spaces_ref) {
    log_error("Unable to determine spaces ref of display id %d", did);
    return(display_space_ids_v);
  }

  int display_spaces_count = CFArrayGetCount(display_spaces_ref);

  for (int i = 0; i < display_spaces_count; i++) {
    CFDictionaryRef display_ref = CFArrayGetValueAtIndex(display_spaces_ref, i);
    CFStringRef     identifier  = CFDictionaryGetValue(display_ref, CFSTR("Display Identifier"));
    if (!CFEqual(uuid, identifier)) {
      continue;
    }

    CFArrayRef spaces_ref = CFDictionaryGetValue(display_ref, CFSTR("Spaces"));
    int        qty        = CFArrayGetCount(spaces_ref);
    for (int j = 0; j < qty; ++j) {
      CFDictionaryRef space_ref = CFArrayGetValueAtIndex(spaces_ref, j);
      CFNumberRef     sid_ref   = CFDictionaryGetValue(space_ref, CFSTR("id64"));
      size_t          space_id  = 0;
      CFNumberGetValue(sid_ref, CFNumberGetType(CFDictionaryGetValue(space_ref, CFSTR("id64"))), &space_id);
      vector_push(display_space_ids_v, (void *)(size_t)space_id);
    }
  }
  CFRelease(display_spaces_ref);
  CFRelease(uuid);
  return(display_space_ids_v);
}

struct Vector *get_space_owners(int space_id){
  struct Vector *v           = vector_new();
  CFArrayRef    space_owners = CGSSpaceCopyOwners(g_connection, (CGSSpaceID)space_id);
  uint32_t      count        = CFArrayGetCount(space_owners);

  for (uint32_t i = 0; i < count; i++) {
    CFNumberRef nref = CFArrayGetValueAtIndex(space_owners, i);
    int         p    = 0;
    CFNumberGetValue(nref, CFNumberGetType(nref), &p);
    vector_push(v, (void *)(size_t)p);
  }
  return(v);
}

CGRect get_space_rect(int space_id){
  CGRect       rect    = { 0 };
  CGSRegionRef reg_ref = CGSSpaceCopyManagedShape(g_connection, (CGSSpaceID)space_id);

  CGSGetRegionBounds(reg_ref, &rect);
  return(rect);
}

bool get_space_can_create_tile(int space_id){
  return(CGSSpaceCanCreateTile(g_connection, (CGSSpaceID)space_id));
}

bool get_space_is_active(int space_id){
  return((CGSGetActiveSpace(g_connection) == (CGSSpaceID)space_id) ? true : false);
}

int get_space_management_mode(int space_id){
  return(SLSGetSpaceManagementMode(space_id));
}

uint32_t *get_space_minimized_window_list(uint64_t sid, int *count){
  int      window_count, non_min_window_count;
  uint32_t *windows_list         = get_space_window_list_for_connection(&sid, 1, 0, &window_count, true);
  uint32_t *non_min_windows_list = get_space_window_list_for_connection(&sid, 1, 0, &non_min_window_count, false);

  *count = (window_count - non_min_window_count);
  uint32_t *minimized_window_list = calloc((*count) + 1, sizeof(uint32_t));
  int      on                     = 0;

  if (windows_list != 0) {
    for (int i = 0; i < window_count; i++) {
      if (windows_list[i] == 0) {
        continue;
      }
      bool is_minimized = true;
      for (int ii = 0; ii < non_min_window_count && is_minimized == true; ii++) {
        if (non_min_windows_list[ii] == windows_list[i]) {
          is_minimized = false;
        }
      }
      if (is_minimized == true) {
        minimized_window_list[on++] = windows_list[i];
      }
    }
  }

  return(minimized_window_list);
}

CGImageRef space_capture(uint32_t sid) {
  CGImageRef image  = NULL;
  CFArrayRef result = SLSHWCaptureSpace(g_connection, sid, 0);
  uint32_t   count  = CFArrayGetCount(result);

  if (count > 0) {
    image = (CGImageRef)CFArrayGetValueAtIndex(result, 0);
  }
  return(image);
}

int get_space_type(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid));
}

char *get_space_uuid(uint64_t sid){
  int type = get_space_type(sid);

  if (type != 0 && type != 4) {
    log_error("space not user or fullscreen");
    return("");
  }
  CFStringRef uuid_ref = SLSSpaceCopyName(g_connection, sid);
  char        *uuid    = cstring_from_CFString(uuid_ref);

  if (!uuid_ref) {
    log_error("uuid ref err");
  }
  CFRelease(uuid_ref);
  return(uuid);
}

void set_space_by_index(int space){
  CFNotificationCenterRef nc     = CFNotificationCenterGetDistributedCenter();
  CFStringRef             numstr = CFStringCreateWithFormat(NULL, nil, CFSTR("%d"), space);

  CFNotificationCenterPostNotification(nc, CFSTR("com.apple.switchSpaces"), numstr, NULL, TRUE);
}

uint32_t *get_space_window_list_for_connection(uint64_t *space_list, int space_count, int cid, int *count, bool include_minimized){
  int        window_count = 0;
  uint64_t   set_tags     = 1;
  uint64_t   clear_tags   = 0;
  uint32_t   options      = include_minimized ? 0x7 : 0x2;
  uint32_t   *window_list = calloc(1, sizeof(uint32_t));

  CFArrayRef space_list_ref = cfarray_of_cfnumbers(space_list, sizeof(uint64_t), 1, kCFNumberSInt64Type);

  if (SPACE_UTILS_DEBUG_MODE == true) {
    log_info("space window list> %d||min?%d|cid:%d|space_count:%d|display id:%d|",
             space_count, include_minimized, cid, space_count,
             get_space_display_id(space_list[0])
             );
  }
  errno = 0;
  CFArrayRef window_list_ref = SLSCopyWindowsWithOptionsAndTags(g_connection, cid, space_list_ref, options, &set_tags, &clear_tags);

  if (!window_list_ref) {
    if (SPACE_UTILS_DEBUG_MODE == true) {
      log_error("connection %d Failed", cid);
    }
    goto err;
  }

  *count = CFArrayGetCount(window_list_ref);
  if ((*count) < 1) {
    if (SPACE_UTILS_DEBUG_MODE == true) {
      log_error("window count less then one:%d", *count);
    }
    return(0);
  }

  CFTypeRef query    = SLSWindowQueryWindows(g_connection, window_list_ref, *count);
  CFTypeRef iterator = SLSWindowQueryResultCopyWindows(query);

  if (window_list) {
    free(window_list);
  }
  window_list = calloc((*count) * 10, sizeof(uint32_t));
  while (SLSWindowIteratorAdvance(iterator)) {
    window_list[window_count++] = SLSWindowIteratorGetWindowID(iterator);
  }
  CFRelease(query);
  CFRelease(iterator);
  CFRelease(window_list_ref);
  return(window_list);

err:
  if (SPACE_UTILS_DEBUG_MODE == true) {
    log_error("returning null window list");
  }
  return(0);
} /* space_window_list_for_connection */

uint32_t *get_space_window_list(uint64_t sid, int *count, bool include_minimized){
  return(get_space_window_list_for_connection(&sid, 1, 0, count, include_minimized));
}

int get_total_spaces(void){
  int rows, cols;

  CoreDockGetWorkspacesCount(&rows, &cols);
  return(cols);
}

int get_space_display_id(int sid){
  CFStringRef uuid_string = SLSCopyManagedDisplayForSpace(g_connection, (uint32_t)sid);

  if (!uuid_string) {
    return(0);
  }

  CFUUIDRef uuid = CFUUIDCreateFromString(NULL, uuid_string);
  uint32_t  id   = CGDisplayGetDisplayIDFromUUID(uuid);

  CFRelease(uuid);
  CFRelease(uuid_string);

  return((int)id);
}

bool get_space_is_fullscreen(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 4);
}

bool get_space_is_system(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 2);
}

bool get_space_is_visible(uint64_t sid){
  return(sid == get_display_space_id(get_space_display_id(sid)));
}

uint64_t get_display_space_id(uint32_t did){
  CFStringRef uuid = get_display_uuid_ref(did);

  if (!uuid) {
    return(0);
  }
  uint64_t sid = SLSManagedDisplayGetCurrentSpace(g_connection, uuid);

  CFRelease(uuid);

  return(sid);
}

uint64_t get_dsid_from_sid(uint32_t sid) {
  uint64_t   result      = 0;
  int        desktop_cnt = 1;

  CFArrayRef display_spaces_ref   = SLSCopyManagedDisplaySpaces(g_connection);
  int        display_spaces_count = CFArrayGetCount(display_spaces_ref);

  for (int i = 0; i < display_spaces_count; ++i) {
    CFDictionaryRef display_ref  = CFArrayGetValueAtIndex(display_spaces_ref, i);
    CFArrayRef      spaces_ref   = CFDictionaryGetValue(display_ref, CFSTR("Spaces"));
    int             spaces_count = CFArrayGetCount(spaces_ref);

    for (int j = 0; j < spaces_count; ++j) {
      CFDictionaryRef space_ref = CFArrayGetValueAtIndex(spaces_ref, j);
      CFNumberRef     sid_ref   = CFDictionaryGetValue(space_ref, CFSTR("id64"));
      CFNumberGetValue(sid_ref, CFNumberGetType(sid_ref), &result);
      if (sid == (uint32_t)desktop_cnt) {
        goto out;
      }

      ++desktop_cnt;
    }
  }

  result = 0;
out:
  CFRelease(display_spaces_ref);
  return(result);
}

uint32_t get_display_id_for_space(uint32_t sid) {
  uint64_t dsid = get_dsid_from_sid(sid);

  if (!dsid) {
    return(0);
  }
  CFStringRef uuid_string = SLSCopyManagedDisplayForSpace(g_connection, dsid);

  if (!uuid_string) {
    return(0);
  }

  CFUUIDRef uuid = CFUUIDCreateFromString(NULL, uuid_string);
  uint32_t  id   = CGDisplayGetDisplayIDFromUUID(uuid);

  CFRelease(uuid);
  CFRelease(uuid_string);

  return(id);
}

int get_space_id(void){
  return(SLSGetActiveSpace(g_connection));
}

bool get_space_is_user(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 0);
}
