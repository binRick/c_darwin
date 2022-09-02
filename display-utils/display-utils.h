#pragma once
#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H
//////////////////////////////////////
#include "c_vector/vector/vector.h"
#include <ApplicationServices/ApplicationServices.h>
#include <ctype.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreServices/CoreServices.h>
#include <CoreVideo/CVPixelBuffer.h>
#include <errno.h>
#include <inttypes.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/IOKitLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//////////////////////////////////////
int get_display_width();
int get_display_height();
CFStringRef display_uuid(uint32_t did);
uint32_t display_id(CFStringRef uuid);
CGPoint display_center(uint32_t did);
CGRect display_bounds(uint32_t did);
uint32_t display_active_display_count(void);
struct Vector *get_display_ids_v();
void get_display_bounds(int *x, int *y, int *w, int *h);
uint32_t *display_active_display_list(uint32_t *count);

#endif
