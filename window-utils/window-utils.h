#pragma once
#ifndef WINDOW_UTILS_H
#define WINDOW_UTILS_H
#ifndef DEFAULT_LOGLEVEL
#define DEFAULT_LOGLEVEL    1
#endif
#include <Carbon/Carbon.h>
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
#include "ansi-codes/ansi-codes.h"
#include "app-utils/app-utils.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
///////////////////////////////////////////////////
enum window_flag_t {
  WINDOW_FLAG_SHADOW     = 1 << 0,
  WINDOW_FLAG_FULLSCREEN = 1 << 1,
  WINDOW_FLAG_MINIMIZE   = 1 << 2,
  WINDOW_FLAG_FLOAT      = 1 << 3,
  WINDOW_FLAG_STICKY     = 1 << 4,
  WINDOW_FLAGS_QTY,
};
typedef struct {
  int longDisplay;
  int id;
  int numFound;
  int jsonMode;
} LsWinCtx;

typedef struct {
  int     id;
  int     fromRight;
  int     fromBottom;
  CGPoint position;
  CGSize  size;
  int     hasSize;
  int     movedWindow;
} MoveWinCtx;
struct window_info_t {
  const char *name, *title;
  size_t     window_id, memory_usage;
  pid_t      pid;
  int        layer, sharing_state, store_type;
  bool       is_onscreen;
  CGRect     rect;
};
struct window_t {
  size_t              window_id;
  pid_t               pid;
  CGPoint             position;
  CFTypeRef           app_window_list;
  size_t              app_window_list_qty;
  int                 pos_x, pos_y, width, height, space_id, connection_id, display_id, layer, display_index, level;
  CFNumberRef         layer_ref;
  CGSize              size;
  CFDictionaryRef     window;
  CGRect              rect;
  size_t              memory_usage;
  char                *app_name, *window_name, *window_title, *owner_name, *uuid, *display_uuid;
  char                pid_path[PATH_MAX];
  bool                is_focused, is_visible, is_minimized, can_move, can_minimize, can_resize, is_popover, is_onscreen;
  struct kinfo_proc   pid_info;
  struct Vector       *space_ids_v, *child_pids_v, *window_ids_above, *window_ids_below;
  AXUIElementRef      *app;
  ProcessSerialNumber psn;
  unsigned long       dur, started;
};
static const char *ax_error_str[] =
{
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
int get_window_id_space_id(size_t window_id);
bool get_pid_is_minimized(int pid);
bool get_window_id_is_minimized(size_t window_id);
bool get_window_is_minimized(struct window_t *w);
struct Vector *get_window_ids_above_window(struct window_t *w);
struct Vector *get_window_ids_below_window(struct window_t *w);
struct Vector *get_window_space_ids_v(struct Vector *windows_v);
int get_window_space_id(struct window_t *w);
size_t get_pid_window_id(int PID);
struct Vector *get_window_ids(void);
char *get_window_display_uuid(struct window_t *window);
struct window_t *get_focused_window();
int get_focused_window_id();
struct window_t *get_window_id(size_t WINDOW_ID);
struct Vector *get_windows();
int get_window_display_id(struct window_t *window);
AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window);
struct window_t *get_pid_window(const int PID);
int get_window_level(struct window_t *w);
char *get_window_title(struct window_t *w);
void set_window_tags(struct window_t *w);
CGImageRef window_capture(struct window_t *window);
bool save_window_cgref_to_png(const CGImageRef image, const char *filename);
char *get_window_title(struct window_t *w);
void set_window_tags(struct window_t *w);
void fade_window(struct window_t *w);
bool window_can_move(struct window_t *w);
bool window_can_resize(struct window_t *w);
bool window_can_minimize(struct window_t *w);
bool window_is_topmost(struct window_t *w);
size_t get_first_window_id_by_app_name(char *app_name);
uint32_t getWindowId(AXUIElementRef window);

char *windowTitle(char *appName, char *windowName);
CGPoint AXWindowGetPosition(AXUIElementRef window);
void AXWindowSetPosition(AXUIElementRef window, CGPoint position);
CGPoint CGWindowGetPosition(CFDictionaryRef window);
CGSize CGWindowGetSize(CFDictionaryRef window);

void set_window_id_flags(size_t window_id, enum window_flag_t flags);
void set_window_flags(struct window_t *w, enum window_flag_t flags);

void get_window_tags(struct window_t *w);
void focus_window_id(size_t WINDOW_ID);
void focus_window(struct window_t *w);
void minimize_window_id(size_t WINDOW_ID);
void minimize_window(struct window_t *w);
void make_key_window(struct window_t *w);

bool get_window_is_focused(struct window_t *w);
bool get_window_is_onscreen(struct window_t *w);
bool get_window_is_visible(struct window_t *w);

void set_window_active_on_all_spaces(struct window_t *w);
int window_layer(struct window_t *window);
void window_set_layer(struct window_t *window, uint32_t layer);
void window_id_send_to_space(size_t window_id, uint64_t dsid);
void window_send_to_space(struct window_t *window, uint64_t dsid);
bool get_window_is_minimized(struct window_t *w);
int get_window_layer(struct window_t *w);
CFStringRef get_window_role_ref(struct window_t *w);
bool get_window_is_popover(struct window_t *w);
int get_window_connection_id(struct window_t *w);
pid_t ax_window_pid(AXUIElementRef ref);
uint32_t ax_window_id(AXUIElementRef ref);
CFStringRef get_active_display_uuid(void);
bool window_is_excluded(struct window_t *w);
bool get_window_is_focused(struct window_t *w);
bool get_window_is_onscreen(struct window_t *w);
bool get_window_is_visible(struct window_t *w);
uint64_t *get_display_id_space_ids(uint32_t did, int *count);
char * get_focused_window_title();
char *windowTitle(char *appName, char *windowName);
void PrintWindow(CFDictionaryRef window, void *ctxPtr);
int EnumerateWindows(char *pattern, void (*callback)(CFDictionaryRef window, void *callback_data), void *callback_data);
CGPoint CGWindowGetPosition(CFDictionaryRef window);
CGSize CGWindowGetSize(CFDictionaryRef window);
AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window);
void AXWindowGetValue(AXUIElementRef window, CFStringRef attrName, void *valuePtr);
CGPoint AXWindowGetPosition(AXUIElementRef window);
void AXWindowSetPosition(AXUIElementRef window, CGPoint position);
CGSize AXWindowGetSize(AXUIElementRef window);
void AXWindowSetSize(AXUIElementRef window, CGSize size);
void MoveWindow(CFDictionaryRef window, void *ctxPtr);
int get_windows_qty(void);
struct Vector *get_window_ids(void);
struct Vector *get_windows();
void move_window_id(const int WINDOW_ID, const int X, const int Y);
char *get_window_id_title(const int WINDOW_ID);
CFDictionaryRef window_id_to_window(const int WINDOW_ID);
struct window_t *get_pid_window(const int PID);
bool resize_window(struct window_t *w, const int WIDTH, const int HEIGHT);
bool move_window(struct window_t *w, const int X, const int Y);
struct window_t *get_focused_window();
uint32_t display_active_display_id(void);
int get_focused_window_id();
ProcessSerialNumber get_window_ProcessSerialNumber(struct window_t *w);
int get_window_id_pid(int window_id);
int get_window_id_level(size_t window_id);
void print_all_window_items(FILE *rsp);
///////////////////////////////////////////////////
#endif
