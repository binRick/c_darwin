#include "core-utils/core-utils.h"
#include "display-utils/display-utils.h"
#include "log.h/log.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"

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

struct Vector *get_space_window_ids_v(size_t space_id){
  struct Vector *ids          = vector_new();
  int           window_qty    = 0;
  uint64_t      sid           = (uint64_t)space_id;
  uint32_t      *windows_list = space_window_list_for_connection(&sid, 1, 0, &window_qty, true);

  for (int i = 0; i < window_qty; i++) {
    vector_push(ids, (void *)(size_t)windows_list[i]);
  }
  return(ids);
}

struct Vector *get_display_id_space_ids_v(uint32_t did){
  struct Vector *display_space_ids = vector_new();
  CFStringRef   uuid               = display_uuid(did);

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

    CFArrayRef spaces_ref = CFDictionaryGetValue(display_ref, CFSTR("Spaces"));
    int        qty        = CFArrayGetCount(spaces_ref);
    for (int j = 0; j < qty; ++j) {
      CFDictionaryRef space_ref = CFArrayGetValueAtIndex(spaces_ref, j);
      CFNumberRef     sid_ref   = CFDictionaryGetValue(space_ref, CFSTR("id64"));
      int             ij        = 0;
      CFNumberGetValue(sid_ref, CFNumberGetType(CFDictionaryGetValue(space_ref, CFSTR("id64"))), &ij);
      vector_push(display_space_ids, (void *)(uint64_t)ij);
    }
  }

  CFRelease(display_spaces_ref);
err:
  CFRelease(uuid);
out:
  return(display_space_ids);
}

uint32_t *space_minimized_window_list(uint64_t sid, int *count){
  int      window_count, non_min_window_count;
  uint32_t *windows_list         = space_window_list_for_connection(&sid, 1, 0, &window_count, true);
  uint32_t *non_min_windows_list = space_window_list_for_connection(&sid, 1, 0, &non_min_window_count, false);

  *count = (window_count - non_min_window_count);
  uint32_t *minimized_window_list = calloc((*count) + 1, sizeof(uint32_t));
  int      on                     = 0;

  for (int i = 0; i < window_count; i++) {
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

  return(minimized_window_list);
}

CGImageRef space_capture(uint32_t sid) {
  uint64_t   dsid  = dsid_from_sid(sid);
  CGImageRef image = NULL;

  if (dsid) {
    CFArrayRef result = SLSHWCaptureSpace(g_connection, dsid, 0);
    uint32_t   count  = CFArrayGetCount(result);
    if (count > 0) {
      image = (CGImageRef)CFArrayGetValueAtIndex(result, 0);
    }
  }
  return(image);
}

int space_type(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid));
}

char *get_space_uuid(uint64_t sid){
  int type = space_type(sid);

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

uint32_t *space_window_list_for_connection(uint64_t *space_list, int space_count, int cid, int *count, bool include_minimized){
  int        window_count = 0;
  uint64_t   set_tags     = 1;
  uint64_t   clear_tags   = 0;
  uint32_t   options      = include_minimized ? 0x7 : 0x2;
  uint32_t   *window_list = calloc(1, sizeof(uint32_t));

  CFArrayRef space_list_ref = cfarray_of_cfnumbers(space_list, sizeof(uint64_t), 1, kCFNumberSInt64Type);
  //log_info("space window list> %d||min?%d|cid:%d|space_count:%d|",space_count,include_minimized,cid,space_count);
  //  space_list_ref = calloc(100,sizeof(uint64_t));
  CFArrayRef window_list_ref = SLSCopyWindowsWithOptionsAndTags(g_connection, cid, space_list_ref, options, &set_tags, &clear_tags);

  if (!window_list_ref) {
    // log_error("window list not found");
    goto err;
  }

  *count = CFArrayGetCount(window_list_ref);
  if ((*count) < 1) {
    //   log_error("window count less then one:%d",*count);
    goto out;
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
out:
  CFRelease(window_list_ref);
err:
  CFRelease(space_list_ref);
  return(window_list);
} /* space_window_list_for_connection */

uint32_t *space_window_list(uint64_t sid, int *count, bool include_minimized){
  return(space_window_list_for_connection(&sid, 1, 0, count, include_minimized));
}

int total_spaces(void){
  int rows, cols;

  CoreDockGetWorkspacesCount(&rows, &cols);
  return(cols);
}

int space_display_id(int sid){
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

bool space_is_fullscreen(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 4);
}

bool space_is_system(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 2);
}

bool space_is_visible(uint64_t sid){
  return(sid == display_space_id(space_display_id(sid)));
}

uint64_t display_space_id(uint32_t did){
  CFStringRef uuid = display_uuid(did);

  if (!uuid) {
    return(0);
  }

  uint64_t sid = SLSManagedDisplayGetCurrentSpace(g_connection, uuid);

  CFRelease(uuid);

  return(sid);
}

uint64_t dsid_from_sid(uint32_t sid) {
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

uint32_t display_id_for_space(uint32_t sid) {
  uint64_t dsid = dsid_from_sid(sid);

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

bool space_is_user(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 0);
}
