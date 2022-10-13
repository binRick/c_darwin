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
struct display_t {
  size_t        display_id, index;
  int           width, height;
  int offset_x, offset_y;
  int center_x, center_y;
  char          *uuid;
  bool          is_main;
  struct Vector *space_ids_v, *window_ids_v;
};
int get_display_id_index(size_t display_id);
struct Vector *get_display_id_window_ids_v(uint32_t display_id);
struct Vector *get_displays_v();
int get_display_id_width(int display_id);
int get_display_id_height(int display_id);
void print_displays();
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
struct Vector *get_display_id_space_ids_v(uint32_t did);
int get_current_display_id(void);
int get_display_id_for_space(uint32_t sid);
uint64_t get_dsid_from_sid(uint32_t sid);
int get_space_display_id(int sid);
int get_current_display_index(void);
#endif
