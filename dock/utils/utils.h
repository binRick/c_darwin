#pragma once
#ifndef DOCK_UTILS_H
#define DOCK_UTILS_H
//////////////////////////////////////
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
CGSize dock_offset(void);
bool dock_is_visible(void);
int dock_orientation(void);
char *dock_orientation_name(void);
CGRect dock_rect(void);
#endif
