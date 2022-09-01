#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/IOKitLib.h>
//////////////////////////////////////////////////////////////
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
//////////////////////////////////////////////////////////////
#include "core-utils/core-utils-extern.h"
#include "process/process.h"
#include "system-utils/system-utils.h"
//////////////////////////////////////////////////////////////
#define g_connection                              CGSMainConnectionID()
#define ASCII_ENCODING                            kCFStringEncodingASCII
#define UTF8_ENCODING                             kCFStringEncodingUTF8
#define MAX_DISPLAYS                              8
#define DEBUG_MODE                                false
#define LAYER_BELOW                               kCGBackstopMenuLevelKey
#define LAYER_NORMAL                              kCGNormalWindowLevelKey
#define LAYER_ABOVE                               kCGFloatingWindowLevelKey
#define kCGSFloatingWindowTagBit                  (1 << 1)
#define kCGSFollowsUserTagBit                     (1 << 2)
#define kCGSHiddenTagBit                          (1 << 8)
#define kCGSStickyTagBit                          (1 << 13)
#define kCGSWindowOwnerFollowsForegroundTagBit    (1 << 27)
#define kCGSOnAllWorkspacesTagBit                 (1 << 11)
//////////////////////////////////////////////////////////////
struct window_t {
  size_t              window_id;
  pid_t               pid;
  CGPoint             position;
  CFTypeRef           app_window_list;
  size_t              app_window_list_qty;
  int                 pos_x, pos_y, width, height, space_id, connection_id, display_id, layer, display_index;
  CFNumberRef         layer_ref;
  CGSize              size;
  CFDictionaryRef     window;
  CGRect              rect;
  size_t              memory_usage;
  char                *app_name, *window_name, *window_title, *owner_name, *uuid, *display_uuid;
  char                pid_path[PATH_MAX];
  bool                is_focused, is_visible, is_minimized, can_move, can_minimize, can_resize;
  struct kinfo_proc   pid_info;
  struct Vector       *space_ids_v, *child_pids_v;
  AXUIElementRef      *app;
  ProcessSerialNumber psn;
  unsigned long       dur, started;
};
static struct {
  void              *memory;
  uint64_t          size;
  volatile uint64_t used;
}                 g_temp_storage;

static const char *bool_str[]  = { "off", "on" };
static const char *layer_str[] =
{
  [0]            = "",
  [LAYER_BELOW]  = "below",
  [LAYER_NORMAL] = "normal",
  [LAYER_ABOVE]  = "above"
};

struct Vector *get_space_ids_v();
struct Vector *get_window_space_ids_v(struct Vector *windows_v);
int get_display_id_index(int display_id);
int window_layer(struct window_t *window);
void window_send_to_space(struct window_t *window, uint64_t dsid);
void window_set_layer(struct window_t *window, uint32_t layer);
CFDictionaryRef window_id_to_window(const int WINDOW_ID);
AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window);
bool get_window_is_visible(struct window_t *window);
char *windowTitle(char *appName, char *windowName);
CGPoint AXWindowGetPosition(AXUIElementRef window);
int get_display_id_index(int display_id);
CGSize CGWindowGetSize(CFDictionaryRef window);
void set_space_by_index(int space);
int is_full_screen(void);
CGPoint CGWindowGetPosition(CFDictionaryRef window);
struct window_t *get_focused_window();
void CoreDockGetWorkspacesCount(int *rows, int *cols);
int get_pid_window_id(const int PID);
struct window_t *get_window_id(const int WINDOW_ID);
int space_display_id(int sid);
uint32_t *space_window_list_for_connection(uint64_t *space_list, int space_count, int cid, int *count, bool include_minimized);
uint32_t *space_window_list(uint64_t sid, int *count, bool include_minimized);
bool space_is_fullscreen(uint64_t sid);
bool space_is_system(uint64_t sid);
bool space_is_visible(uint64_t sid);
CGPoint display_center(uint32_t did);
CGRect display_bounds(uint32_t did);
CFStringRef display_uuid(uint32_t did);
uint32_t display_id(CFStringRef uuid);
void *ts_alloc_aligned(uint64_t elem_size, uint64_t elem_count);
uint64_t *display_space_list(uint32_t did, int *count);
uint32_t *space_minimized_window_list(uint64_t sid, int *count);
int get_window_id_space_id(int window_id);
int get_window_display_id(struct window_t *window);
char *get_window_display_uuid(struct window_t *window);
int get_window_space_id(struct window_t *window);
int total_spaces(void);
int space_type(uint64_t sid);
int get_space_id(void);
void set_space_by_index(int space);
CGPoint display_center(uint32_t did);
bool window_is_topmost(struct window_t *w);
bool window_can_minimize(struct window_t *w);
bool window_can_move(struct window_t *w);
bool window_can_resize(struct window_t *w);
char *get_space_uuid(uint64_t sid);
bool window_is_excluded(struct window_t *w);
CGImageRef resize_cgimage(CGImageRef imageRef, int width, int height);
int CFDictionaryGetInt(CFDictionaryRef dict, const void *key);
char *CFDictionaryCopyCString(CFDictionaryRef dict, const void *key);
char *get_chars_from_CFString(CFStringRef cf_string);
int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key);
struct Vector *get_windows();
int get_string_property(IOHIDDeviceRef device, CFStringRef prop, wchar_t *buf, size_t len);
unsigned short get_vendor_id(IOHIDDeviceRef device);
unsigned short get_product_id(IOHIDDeviceRef device);
int get_serial_number(IOHIDDeviceRef device, wchar_t *buf, size_t len);
int get_manufacturer_string(IOHIDDeviceRef device, wchar_t *buf, size_t len);
int get_product_string(IOHIDDeviceRef device, wchar_t *buf, size_t len);
char * CFStringCopyUTF8String(CFStringRef aString);
char *int_to_string(const int);
char *cstring_from_CFString(CFStringRef cf_string);
CFStringRef cfstring_from_cstring(char *cstring);
CFArrayRef cfarray_of_cfnumbers(void *values, size_t size, int count, CFNumberType type);
bool ts_init(uint64_t size);
uint64_t ts_align(uint64_t used, uint64_t align);
void ts_assert_within_bounds(void);
void *ts_alloc_aligned(uint64_t elem_size, uint64_t elem_count);
void *ts_alloc_unaligned(uint64_t size);
void *ts_expand(void *ptr, uint64_t old_size, uint64_t increment);
void *ts_resize(void *ptr, uint64_t old_size, uint64_t new_size);
void ts_reset(void);
pid_t ax_window_pid(AXUIElementRef ref);
uint32_t ax_window_id(AXUIElementRef ref);
int total_spaces(void);
uint32_t *space_window_list_for_connection(uint64_t *space_list, int space_count, int cid, int *count, bool include_minimized);
uint32_t *space_window_list(uint64_t sid, int *count, bool include_minimized);
uint32_t *space_minimized_window_list(uint64_t sid, int *count);
int space_display_id(int sid);
bool window_id_is_minimized(int window_id);
uint64_t display_space_id(uint32_t did);
int get_display_width();
CFStringRef display_uuid(uint32_t did);
uint32_t display_id(CFStringRef uuid);
int get_window_connection_id(struct window_t *w);
int get_focused_pid();
pid_t PSN2PID(ProcessSerialNumber psn);
ProcessSerialNumber PID2PSN(pid_t pid);
struct Vector *get_display_id_space_ids_v(uint32_t did);
struct Vector *get_display_ids_v();
bool display_menu_bar_visible(void);
char *string_copy(char *s);
AXUIElementRef get_frontmost_app();
void get_display_bounds(int *x, int *y, int *w, int *h);
char *cfstring_copy(CFStringRef string);
uint32_t *display_active_display_list(uint32_t *count);
CGRect display_menu_bar_rect(uint32_t did);
uint32_t display_active_display_count(void);
int mission_control_index(uint64_t sid);
uint32_t display_id_for_space(uint32_t sid);
void command_list_displays();
int window_layer(struct window_t *window);
