#pragma once
#include "frameworks/frameworks.h"
#include "iokit-utils/iokit-utils.h"
#include "log/log.h"
static bool IOKIT_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__iokit_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_IOKIT_UTILS") != NULL) {
    log_debug("Enabling IOKit Utils Debug Mode");
    IOKIT_UTILS_DEBUG_MODE = true;
  }
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
