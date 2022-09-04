#pragma once
#ifndef IOKIT_UTILS_H
#define IOKIT_UTILS_H
#include "string-utils/string-utils.h"
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/IOKitLib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

unsigned short get_vendor_id(IOHIDDeviceRef device);
unsigned short get_product_id(IOHIDDeviceRef device);
int get_serial_number(IOHIDDeviceRef device, wchar_t *buf, size_t len);
int get_manufacturer_string(IOHIDDeviceRef device, wchar_t *buf, size_t len);
int get_product_string(IOHIDDeviceRef device, wchar_t *buf, size_t len);
int get_string_property(IOHIDDeviceRef device, CFStringRef prop, wchar_t *buf, size_t len);
int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key);

#endif
