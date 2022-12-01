#pragma once
#ifndef WINDOW_UTILS_H
#define WINDOW_UTILS_H
#include "core/core.h"
#include <fnmatch.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
///////////////////////////////////////////////////
struct cf_window_list {
  int           count;
  int           capacity;
  struct window **windows;
};
struct cf_window_list_options {
  bool all;
  bool on_screen_only;
  bool exclude_desktop_elements;
  bool including_window;
  bool above_window;
  bool below_window;
};
struct cf_app {
  AXUIElementRef ax_app;
  pid_t          pid;
  CFStringRef    name;
};
struct cf_window {
  struct cf_app  *app;
  AXUIElementRef ax_window;
  CGWindowID     id;
  CGRect         frame;
  CFStringRef    title;
};
static const char *ax_error_str[] = {
  [-kAXErrorSuccess]                           = "kAXErrorSuccess",
  [-kAXErrorFailure]                           = "kAXErrorFailure",
  [-kAXErrorIllegalArgument]                   = "kAXErrorIllegalArgument",
  [-kAXErrorInvalidUIElement]                  = "kAXErrorInvalidUIElement",
  [-kAXErrorInvalidUIElementObserver]          = "kAXErrorInvalidUIElementObserver",
  [-kAXErrorCannotComplete]                    = "kAXErrorCannotComplete",
  [-kAXErrorAttributeUnsupported]              = "kAXErrorAttributeUnsupported",
  [-kAXErrorActionUnsupported]                 = "kAXErrorActionUnsupported",
  [-kAXErrorNotificationUnsupported]           = "kAXErrorNotificationUnsupported",
  [-kAXErrorNotImplemented]                    = "kAXErrorNotImplemented",
  [-kAXErrorNotificationAlreadyRegistered]     = "kAXErrorNotificationAlreadyRegistered",
  [-kAXErrorNotificationNotRegistered]         = "kAXErrorNotificationNotRegistered",
  [-kAXErrorAPIDisabled]                       = "kAXErrorAPIDisabled",
  [-kAXErrorNoValue]                           = "kAXErrorNoValue",
  [-kAXErrorParameterizedAttributeUnsupported] = "kAXErrorParameterizedAttributeUnsupported",
  [-kAXErrorNotEnoughPrecision]                = "kAXErrorNotEnoughPrecision"
};
static const char *window_levels[] = { "Base", "Minimum", "Desktop", "Backstop", "Normal", "Floating", "TornOffMenu", "Dock", "MainMenu", "Status", "ModalPanel", "PopUpMenu", "Dragging", "ScreenSaver", "Maximum", "Overlay", "Help", "Utility", "DesktopIcon", "Cursor", "AssistiveTechHigh" };
///////////////////////////////////////////////////
struct window_iteration_process_t {
  struct rusage     r_usage;
  struct kinfo_proc info;
};
struct window_iteration_t {
  char                              *name, *role, *subrole;
  CGWindowID                        id;
  pid_t                             pid;
  bool                              can_minimize;
  CFDictionaryRef                   window;
  AXUIElementRef                    app;
  hash_t                            *map;
  ProcessSerialNumber               psn;
  struct window_iteration_process_t process;
  hash_t                            *spaces, *displays, *apps, *copied;
};
struct sqldbal_db;
int get_window_id_space_id(size_t window_id);
bool get_pid_is_minimized(int pid);
bool unminimize_window_id(size_t window_id);
bool window_db_load(struct sqldbal_db *db);
bool get_window_id_is_minimized(size_t window_id);
struct Vector *get_window_ids_above_window(struct window_info_t *w);
struct Vector *get_window_ids_below_window(struct window_info_t *w);
struct Vector *get_window_space_ids_v(struct Vector *windows_v);
int get_window_space_id(struct window_info_t *w);
size_t get_pid_window_id(int PID);
struct Vector *get_window_ids(void);
struct Vector *get_window_names(void);
hash_t *get_first_window_id_from_names(hash_t *names);
hash_t *get_window_ids_from_names(hash_t *names);
uint32_t *front_process_window_list_for_active_space(int *count);
void wu_run(void);
AXUIElementRef GetUIElement(CGWindowID win);
void bringWindoToForeground(CGWindowID wid);
bool isInside(int rad, CGPoint cirlce, CGPoint point);
CGImageRef __CaptureWindow(CGWindowID wid);
typedef void (^wu_stream_cb)(CGDisplayStreamFrameStatus status, uint64_t time, IOSurfaceRef frame, CGDisplayStreamUpdateRef ref);
struct stop_loop_t;
int wu_stream_display_cb(int width, int height, int display_id, wu_stream_cb cb);
int wu_stream_active_window(int width, int height);
int wu_stream_display(void *L);
char *get_window_display_uuid(struct window_info_t *window);
struct hash_t *get_window_properties_map();
struct hash_t *get_window_properties_map_for_window_ids(hash_t *ids);
struct window_info_t *get_focused_window();
int get_focused_window_id();
int get_window_display_id(size_t window_id);
struct window_info_t *get_random_window_info(void);
size_t get_random_window_info_id(void);
AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window);
CGImageRef capture_window_id_width(size_t window_id, size_t width);
CGImageRef capture_window_id_height(size_t window_id, size_t height);
struct window_info_t *get_pid_window(const int PID);
int get_window_level(struct window_info_t *w);
char *get_window_title(struct window_info_t *w);
void set_window_tags(struct window_info_t *w);
char *get_window_title(struct window_info_t *w);
void set_window_tags(struct window_info_t *w);
void fade_window(struct window_info_t *w);
bool window_can_move(struct window_info_t *w);
bool window_can_resize(struct window_info_t *w);
bool window_can_minimize(struct window_info_t *w);
struct Vector *get_window_props(void);
bool window_is_topmost(struct window_info_t *w);
size_t get_first_window_id_by_name(char *name);
uint32_t getWindowId(AXUIElementRef window);
int get_width_offset_from_display_index(int display_index);
int get_width_offset_from_display_id(int display_id);

char *windowTitle(char *appName, char *windowName);
CGPoint AXWindowGetPosition(AXUIElementRef window);
void AXWindowSetPosition(AXUIElementRef window, CGPoint position);
CGPoint CGWindowGetPosition(CFDictionaryRef window);
CGSize CGWindowGetSize(CFDictionaryRef window);

void set_window_id_flags(size_t window_id, enum window_info_flag_t flags);
void set_window_flags(struct window_info_t *w, enum window_info_flag_t flags);

void get_window_tags(struct window_info_t *w);
void focus_window_id(size_t WINDOW_ID);
void focus_window(struct window_info_t *w);
void make_key_window(struct window_info_t *w);

bool get_window_is_focused(struct window_info_t *w);
bool get_window_is_onscreen(struct window_info_t *w);
bool get_window_is_visible(struct window_info_t *w);

void set_window_active_on_all_spaces(struct window_info_t *w);
int window_layer(struct window_info_t *window);
void window_set_layer(struct window_info_t *window, uint32_t layer);
void window_id_send_to_space(size_t window_id, uint64_t dsid);
void window_send_to_space(struct window_info_t *window, uint64_t dsid);
int get_window_layer(struct window_info_t *w);
CFStringRef get_window_role_ref(struct window_info_t *w);
bool get_window_is_popover(struct window_info_t *w);
int get_window_connection_id(struct window_info_t *w);
bool window_is_excluded(struct window_info_t *w);
bool get_window_is_focused(struct window_info_t *w);
bool get_window_is_onscreen(struct window_info_t *w);
bool get_window_is_visible(struct window_info_t *w);
uint64_t *get_display_id_space_ids(uint32_t did, int *count);
char * get_focused_window_title();
char *windowTitle(char *appName, char *windowName);
int EnumerateWindows(char *pattern, void (*callback)(CFDictionaryRef window, void *callback_data), void *callback_data);
CGPoint CGWindowGetPosition(CFDictionaryRef window);
CGSize CGWindowGetSize(CFDictionaryRef window);
AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window);
void AXWindowGetValue(AXUIElementRef window, CFStringRef attrName, void *valuePtr);
CGPoint AXWindowGetPosition(AXUIElementRef window);
void AXWindowSetPosition(AXUIElementRef window, CGPoint position);
CGSize AXWindowGetSize(AXUIElementRef window);
void AXWindowSetSize(AXUIElementRef window, CGSize size);
int get_windows_qty(void);
bool move_window_id(size_t window_id, const int X, const int Y);
bool minimize_window_id(size_t window_id);
bool set_window_id_to_space(size_t window_id, int space_id);
char *get_window_id_title(const int WINDOW_ID);
CFDictionaryRef window_id_to_window(const int WINDOW_ID);
struct window_info_t *get_pid_window(const int PID);
bool resize_window(struct window_info_t *w, const int WIDTH, const int HEIGHT);
bool move_window(struct window_info_t *w, const int X, const int Y);
struct window_info_t *get_focused_window();
ProcessSerialNumber get_window_ProcessSerialNumber(struct window_info_t *w);
pid_t get_window_id_pid(size_t window_id);
int get_window_id_level(size_t window_id);
void print_all_window_items(FILE *rsp);
struct window_info_t *get_window_id_info(size_t window_id);
struct window_info_t *get_focused_window_info();
bool resize_window_info(struct window_info_t *w, const int WIDTH, const int HEIGHT);
bool move_window_info(struct window_info_t *w, const int X, const int Y);
CGRect get_resized_window_info_rect_by_factor(struct window_info_t *w, float width_factor, float height_factor);
CGRect get_resized_window_info_rect_by_factor_left_side(struct window_info_t *w, float width_factor, float height_factor);
CGRect get_resized_window_info_rect_by_factor_right_side(struct window_info_t *w, float width_factor, float height_factor);
CGRect get_resized_window_info_rect_by_factor_bottom_side(struct window_info_t *w, float width_factor, float height_factor);
CGRect get_resized_window_info_rect_by_factor_top_side(struct window_info_t *w, float width_factor, float height_factor);
CGImageRef capture_window_id(size_t window_id);
CGImageRef capture_window(struct window_info_t *window);
///////////////////////////////////////////////////
unsigned char *save_cgref_to_png_memory(CGImageRef image, size_t *len);
bool save_cgref_to_png_file(CGImageRef image, char *filename);
CGImageRef preview_window_id(size_t window_id);
CGImageRef capture_window_id_rect(size_t window_id, CGRect rect);
CGImageRef capture_window_id_ptr(size_t window_id);
///////////////////////////////////////////////////
struct Vector *get_captured_window_infos_v(void);
int __window_utils_show_alert(const char *title, const char *msg, const char *defaultButton, const char *cancelButton);
bool window_name_compare(char *, char *);
#endif
