#include "../include/includes.h"


int CFDictionaryGetInt(CFDictionaryRef dict, const void *key) {
  int isSuccess, value;

  isSuccess = CFNumberGetValue(
    CFDictionaryGetValue(dict, key), kCFNumberIntType, &value
    );

  return(isSuccess ? value : 0);
}


/* Copy a string value from a CFDictionary into a newly allocated string */
char *CFDictionaryCopyCString(CFDictionaryRef dict, const void *key) {
  const void *dictValue;
  CFIndex    length;
  int        maxSize, isSuccess;
  char       *value;

  dictValue = CFDictionaryGetValue(dict, key);
  if (dictValue == NULL) {
    return(NULL);
  }

  /* If empty value, allocate and return empty string */
  length  = CFStringGetLength(dictValue);
  maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
  if (length == 0 || maxSize == 0) {
    value  = (char *)malloc(1);
    *value = '\0';
    return(value);
  }

  /* Otherwise, allocate string and copy value into it */
  value     = (char *)malloc(maxSize);
  isSuccess = CFStringGetCString(
    dictValue, value, maxSize, kCFStringEncodingUTF8
    );

  return(isSuccess ? value : NULL);
}


void AXWindowGetValue(AXUIElementRef window,
                      CFStringRef    attrName,
                      void           *valuePtr) {
  AXValueRef attrValue;

  AXUIElementCopyAttributeValue(window, attrName, (CFTypeRef *)&attrValue);
  AXValueGetValue(attrValue, AXValueGetType(attrValue), valuePtr);
  CFRelease(attrValue);
}


CGPoint CGWindowGetPosition(CFDictionaryRef window) {
  CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
  int             x      = CFDictionaryGetInt(bounds, CFSTR("X"));
  int             y      = CFDictionaryGetInt(bounds, CFSTR("Y"));

  return(CGPointMake(x, y));
}


char *windowTitle(char *appName, char *windowName) {
  size_t titleSize;
  char   *title;

  if (!appName || !*appName) {
    title  = (char *)malloc(1);
    *title = '\0';
  } else if (!windowName || !*windowName) {
    titleSize = strlen(appName) + 1;
    title     = (char *)malloc(titleSize);
    strncpy(title, appName, titleSize);
  } else {
    titleSize = strlen(appName) + strlen(" - ") + strlen(windowName) + 1;
    title     = (char *)malloc(titleSize);
    snprintf(title, titleSize, "%s - %s", appName, windowName);
  }

  return(title);
}


/* Given window dictionary from CGWindowList, return size */
CGSize CGWindowGetSize(CFDictionaryRef window) {
  CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
  int             width  = CFDictionaryGetInt(bounds, CFSTR("Width"));
  int             height = CFDictionaryGetInt(bounds, CFSTR("Height"));

  return(CGSizeMake(width, height));
}

