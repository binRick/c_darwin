#pragma once
#ifndef MENU_BAR_UTILS_H
#define MENU_BAR_UTILS_H
//////////////////////////////////////
//#include <ApplicationServices/ApplicationServices.h>
#include <ctype.h>

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

int get_menu_bar_height();
bool get_menu_bar_visible(void);
CGRect get_menu_bar_rect(int display_id);

#endif
