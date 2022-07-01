#pragma once
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOKitLib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

char *get_chars_from_CFString(CFStringRef cf_string);
int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key);
int get_string_property(IOHIDDeviceRef device, CFStringRef prop, wchar_t *buf, size_t len);
unsigned short get_vendor_id(IOHIDDeviceRef device);
unsigned short get_product_id(IOHIDDeviceRef device);
int get_serial_number(IOHIDDeviceRef device, wchar_t *buf, size_t len);
int get_manufacturer_string(IOHIDDeviceRef device, wchar_t *buf, size_t len);
int get_product_string(IOHIDDeviceRef device, wchar_t *buf, size_t len);
char * CFStringCopyUTF8String(CFStringRef aString);
