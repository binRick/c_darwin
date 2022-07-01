#include "core-utils/core-utils.h"
#include <CoreFoundation/CFBase.h>
#define ASCII_ENCODING    kCFStringEncodingASCII
#define UTF8_ENCODING     kCFStringEncodingUTF8


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

