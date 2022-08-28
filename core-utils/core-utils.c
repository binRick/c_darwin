#pragma once
#include "core-utils/core-utils.h"

int get_focused_pid(){
  ProcessSerialNumber psn;
  GetFrontProcess(&psn);
  pid_t pid = PSN2PID(psn);
  if(pid>1)
      return (int)pid;

  CFArrayRef window_list = CGWindowListCopyWindowInfo(
    kCGWindowListExcludeDesktopElements | kCGWindowListOptionOnScreenOnly,
    kCGNullWindowID);

  int num_windows = CFArrayGetCount(window_list);

  for (int i = 0; i < num_windows; i++) {
    CFDictionaryRef dict              = CFArrayGetValueAtIndex(window_list, i);
    CFNumberRef     objc_window_layer = CFDictionaryGetValue(dict, kCGWindowLayer);

    int             window_layer;
    CFNumberGetValue(objc_window_layer, kCFNumberIntType, &window_layer);

    if (window_layer == 0) {
      CFNumberRef objc_window_pid = CFDictionaryGetValue(dict, kCGWindowOwnerPID);

      int         window_pid = 0;
      CFNumberGetValue(objc_window_pid, kCFNumberIntType, &window_pid);

      return(window_pid);
    }
  }

  return(-1);
}

ProcessSerialNumber PID2PSN(pid_t pid) {
  ProcessSerialNumber tempPSN;

  GetProcessForPID(pid, &tempPSN);
  return(tempPSN);
}

pid_t PSN2PID(ProcessSerialNumber psn) {
  pid_t tempPID;

  GetProcessPID(&psn, &tempPID);
  return(tempPID);
}


int get_focused_space_id(){
    int focused_pid = get_focused_pid();

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

CFStringRef cfstring_from_cstring(char *cstring){
  CFStringRef cfstring = CFStringCreateWithCString(NULL, cstring, kCFStringEncodingUTF8);

  return(cfstring);
}

char * CFStringCopyUTF8String(CFStringRef aString){
  if (aString == NULL) {
    return(NULL);
  }

  CFIndex length  = CFStringGetLength(aString);
  CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);

  char    *buffer = (char *)malloc(maxSize);

  if (CFStringGetCString(aString, buffer, maxSize, kCFStringEncodingUTF8)) {
    return(buffer);
  }

  return(buffer);
}

char *cstring_from_CFString(CFStringRef cf_string) {
  CFIndex length = CFStringGetLength(cf_string);
  CFIndex size   = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
  CFIndex used_size;
  CFRange range   = CFRangeMake(0, length);
  char    *string = malloc(size);

  CFStringGetBytes(
    cf_string, range, kCFStringEncodingUTF8, '?', false, (unsigned char *)string,
    size - 1, &used_size);
  string[used_size] = '\0';
  return(string);
}

char * cstring_get_ascii_string(CFStringRef data) {
  const char *bytes = NULL;
  char       *chars = NULL;
  size_t     len    = 0;

  bytes = CFStringGetCStringPtr(data, ASCII_ENCODING);
  if (bytes == NULL) {
    len   = (size_t)CFStringGetLength(data) + 1;
    chars = (char *)calloc(len, sizeof(char));
    if (chars != NULL) {
      if (!CFStringGetCString(data, chars, (CFIndex)len, ASCII_ENCODING)) {
        free(chars);
        chars = NULL;
      }
    }
  } else {
    len   = strlen(bytes) + 1;
    chars = (char *)calloc(len, sizeof(char));
    if (chars != NULL) {
      strcpy(chars, bytes);
    }
  }
  return(chars);
}

char *get_chars_from_CFString(CFStringRef cf_string) {
  CFIndex length = CFStringGetLength(cf_string);
  CFIndex size   = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
  CFIndex used_size;
  CFRange range   = CFRangeMake(0, length);
  char    *string = malloc(size);

  CFStringGetBytes(
    cf_string, range, kCFStringEncodingUTF8, '?', false, (unsigned char *)string,
    size - 1, &used_size);
  string[used_size] = '\0';
  return(string);
}

int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key){
  CFTypeRef ref;
  int32_t   value;

  ref = IOHIDDeviceGetProperty(device, key);
  if (ref) {
    if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
      CFNumberGetValue((CFNumberRef)ref, kCFNumberSInt32Type, &value);
      return(value);
    }
  }
  return(0);
}

int get_string_property(IOHIDDeviceRef device, CFStringRef prop, wchar_t *buf, size_t len){
  CFStringRef str;

  if (!len) {
    return(0);
  }

  str = (CFStringRef)IOHIDDeviceGetProperty(device, prop);

  buf[0] = 0;

  if (str) {
    CFIndex str_len = CFStringGetLength(str);
    CFRange range;
    CFIndex used_buf_len;
    CFIndex chars_copied;

    len--;

    range.location = 0;
    range.length   = ((size_t)str_len > len)? len: (size_t)str_len;
    chars_copied   = CFStringGetBytes(str,
                                      range,
                                      kCFStringEncodingUTF32LE,
                                      (char)'?',
                                      FALSE,
                                      (UInt8 *)buf,
                                      len * sizeof(wchar_t),
                                      &used_buf_len);

    if (chars_copied <= 0) {
      buf[0] = 0;
    }else{
      buf[chars_copied] = 0;
    }

    return(0);
  }else {
    return(-1);
  }
} /* get_string_property */

unsigned short get_vendor_id(IOHIDDeviceRef device){
  return(get_int_property(device, CFSTR(kIOHIDVendorIDKey)));
}

unsigned short get_product_id(IOHIDDeviceRef device){
  return(get_int_property(device, CFSTR(kIOHIDProductIDKey)));
}

int get_serial_number(IOHIDDeviceRef device, wchar_t *buf, size_t len){
  return(get_string_property(device, CFSTR(kIOHIDSerialNumberKey), buf, len));
}

int get_manufacturer_string(IOHIDDeviceRef device, wchar_t *buf, size_t len){
  return(get_string_property(device, CFSTR(kIOHIDManufacturerKey), buf, len));
}

int get_product_string(IOHIDDeviceRef device, wchar_t *buf, size_t len){
  return(get_string_property(device, CFSTR(kIOHIDProductKey), buf, len));
}

CFArrayRef cfarray_of_cfnumbers(void *values, size_t size, int count, CFNumberType type){
  CFNumberRef temp[count];

  for (int i = 0; i < count; ++i) {
    temp[i] = CFNumberCreate(NULL, type, ((char *)values) + (size * i));
  }

  CFArrayRef result = CFArrayCreate(NULL, (const void **)temp, count, &kCFTypeArrayCallBacks);

  for (int i = 0; i < count; ++i) {
    CFRelease(temp[i]);
  }

  return(result);
}

bool ts_init(uint64_t size){
  int      page_size = getpagesize();

  uint64_t num_pages = size / page_size;
  uint64_t remainder = size % page_size;

  if (remainder) {
    num_pages++;
  }

  g_temp_storage.used   = 0;
  g_temp_storage.size   = num_pages * page_size;
  g_temp_storage.memory = mmap(0, g_temp_storage.size + page_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

  bool result = g_temp_storage.memory != MAP_FAILED;

  if (result) {
    mprotect(g_temp_storage.memory + g_temp_storage.size, page_size, PROT_NONE);
  }

  return(result);
}

uint64_t ts_align(uint64_t used, uint64_t align){
  assert((align & (align - 1)) == 0);

  uintptr_t ptr   = (uintptr_t)(g_temp_storage.memory + used);
  uintptr_t a_ptr = (uintptr_t)align;
  uintptr_t mod   = ptr & (a_ptr - 1);

  if (mod != 0) {
    ptr += a_ptr - mod;
  }

  return(ptr - (uintptr_t)g_temp_storage.memory);
}

void ts_assert_within_bounds(void){
  if (g_temp_storage.used > g_temp_storage.size) {
    fprintf(stderr, "fatal error: temporary_storage exceeded amount of allocated memory. requested %lld, but allocated size is %lld\n", g_temp_storage.used, g_temp_storage.size);
    exit(EXIT_FAILURE);
  }
}

void *ts_alloc_aligned(uint64_t elem_size, uint64_t elem_count){
  for ( ;;) {
    uint64_t used     = g_temp_storage.used;
    uint64_t aligned  = ts_align(used, elem_size);
    uint64_t new_used = aligned + (elem_size * elem_count);

    if (__sync_bool_compare_and_swap(&g_temp_storage.used, used, new_used)) {
      ts_assert_within_bounds();
      return(g_temp_storage.memory + aligned);
    }
  }
}

void *ts_alloc_unaligned(uint64_t size){
  uint64_t used = __sync_fetch_and_add(&g_temp_storage.used, size);

  ts_assert_within_bounds();
  return(g_temp_storage.memory + used);
}

void *ts_expand(void *ptr, uint64_t old_size, uint64_t increment){
  if (ptr) {
    assert(ptr == g_temp_storage.memory + g_temp_storage.used - old_size);
    __sync_fetch_and_add(&g_temp_storage.used, increment);
    ts_assert_within_bounds();
  } else {
    ptr = ts_alloc_unaligned(increment);
  }

  return(ptr);
}

void *ts_resize(void *ptr, uint64_t old_size, uint64_t new_size){
  assert(ptr == g_temp_storage.memory + g_temp_storage.used - old_size);
  if (new_size > old_size) {
    __sync_fetch_and_add(&g_temp_storage.used, new_size - old_size);
  } else if (new_size < old_size) {
    __sync_fetch_and_sub(&g_temp_storage.used, old_size - new_size);
  }
  return(ptr);
}

void ts_reset(void){
  g_temp_storage.used = 0;
}

uint32_t ax_window_id(AXUIElementRef ref){
  uint32_t wid = 0;

  _AXUIElementGetWindow(ref, &wid);
  return(wid);
}

pid_t ax_window_pid(AXUIElementRef ref){
  return(*(pid_t *)((void *)ref + 0x10));
}

int total_spaces(void){
  int rows, cols;

  CoreDockGetWorkspacesCount(&rows, &cols);
  return(cols);
}

uint32_t *space_window_list(uint64_t sid, int *count, bool include_minimized){
  return(space_window_list_for_connection(&sid, 1, 0, count, include_minimized));
}

uint32_t *space_window_list_for_connection(uint64_t *space_list, int space_count, int cid, int *count, bool include_minimized){
  int        window_count = 0;
  uint64_t   set_tags     = 1;
  uint64_t   clear_tags   = 0;
  uint32_t   options      = include_minimized ? 0x7 : 0x2;
  uint32_t   *window_list = calloc(1, sizeof(uint32_t));

  CFArrayRef space_list_ref  = cfarray_of_cfnumbers(space_list, sizeof(uint64_t), space_count, kCFNumberSInt64Type);
  CFArrayRef window_list_ref = SLSCopyWindowsWithOptionsAndTags(g_connection, cid, space_list_ref, options, &set_tags, &clear_tags);

  if (!window_list_ref) {
    goto err;
  }

  *count = CFArrayGetCount(window_list_ref);
  if ((*count) < 1) {
    goto out;
  }

  CFTypeRef query    = SLSWindowQueryWindows(g_connection, window_list_ref, *count);
  CFTypeRef iterator = SLSWindowQueryResultCopyWindows(query);

  if (window_list) {
    free(window_list);
  }
  window_list = calloc((*count) * 2, sizeof(uint32_t));
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
}   /* space_window_list_for_connection */

bool window_id_is_minimized(int window_id){
  bool is_min = false;
  int  space_minimized_window_qty;
  int  spaces_qty = total_spaces();

  for (int s = 1; s < spaces_qty && is_min == false; s++) {
    uint32_t *minimized_window_list = space_minimized_window_list(s, &space_minimized_window_qty);
    for (int i = 0; i < space_minimized_window_qty && is_min == false; i++) {
      if (minimized_window_list[i] == (uint32_t)window_id) {
        is_min = true;
      }
    }
    if (minimized_window_list) {
      free(minimized_window_list);
    }
  }
  return(is_min);
}

bool space_is_user(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 0);
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

int get_display_width(){
  int    w             = -1;
  CGRect displayBounds = CGDisplayBounds(CGMainDisplayID());

  w = displayBounds.size.width;
  return(w);
}

int get_window_connection_id(struct window_t *w){
  int conn;

  SLSGetConnectionIDForPSN(g_connection, &w->psn, &conn);
  return(conn);
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


