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
struct Vector *get_display_ids_v();
//////////////////////////////////////
int get_active_display_count(void);
//////////////////////////////////////
CFStringRef get_active_display_uuid_ref(void);
CFStringRef get_display_uuid_ref(uint32_t did);
char *get_display_uuid(uint32_t did);
//////////////////////////////////////
int get_display_id(CFStringRef uuid);
uint32_t get_main_display_id(void);
uint32_t get_active_display_id(void);
//////////////////////////////////////
uint32_t *display_active_display_list(uint32_t *count);
//////////////////////////////////////
int get_display_width();
int get_display_height();
void get_display_bounds(int *x, int *y, int *w, int *h);
//////////////////////////////////////
CGPoint get_display_center(uint32_t did);
CGRect get_display_rect(uint32_t did);
//////////////////////////////////////

#endif