#pragma once
#include "core-utils/core-utils-extern.h"
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOKitLib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define g_connection    CGSMainConnectionID()

char *get_chars_from_CFString(CFStringRef cf_string);
int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key);
int get_string_property(IOHIDDeviceRef device, CFStringRef prop, wchar_t *buf, size_t len);
unsigned short get_vendor_id(IOHIDDeviceRef device);
unsigned short get_product_id(IOHIDDeviceRef device);
int get_serial_number(IOHIDDeviceRef device, wchar_t *buf, size_t len);
int get_manufacturer_string(IOHIDDeviceRef device, wchar_t *buf, size_t len);
int get_product_string(IOHIDDeviceRef device, wchar_t *buf, size_t len);
char * CFStringCopyUTF8String(CFStringRef aString);
char *int_to_string(const int);
char *cstring_from_CFString(CFStringRef cf_string);
CFStringRef cfstring_from_cstring(char *cstring);
CFArrayRef cfarray_of_cfnumbers(void *values, size_t size, int count, CFNumberType type);

static struct {
  void              *memory;
  uint64_t          size;
  volatile uint64_t used;
} g_temp_storage;

bool ts_init(uint64_t size);
uint64_t ts_align(uint64_t used, uint64_t align);
void ts_assert_within_bounds(void);
void *ts_alloc_aligned(uint64_t elem_size, uint64_t elem_count);
void *ts_alloc_unaligned(uint64_t size);
void *ts_expand(void *ptr, uint64_t old_size, uint64_t increment);
void *ts_resize(void *ptr, uint64_t old_size, uint64_t new_size);
void ts_reset(void);
pid_t ax_window_pid(AXUIElementRef ref);
uint32_t ax_window_id(AXUIElementRef ref);
int total_spaces(void);
uint32_t *space_window_list_for_connection(uint64_t *space_list, int space_count, int cid, int *count, bool include_minimized);
uint32_t *space_window_list(uint64_t sid, int *count, bool include_minimized);
uint32_t *space_minimized_window_list(uint64_t sid, int *count);
int space_display_id(int sid);
