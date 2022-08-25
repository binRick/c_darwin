#pragma once
#ifndef DEFAULT_LOGLEVEL
#define DEFAULT_LOGLEVEL    1
#endif
#include <ApplicationServices/ApplicationServices.h>
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

typedef struct window_t {
  pid_t               pid;
  CGPoint             position;
  int                 pos_x, pos_y, width, height, space_id, connection_id, display_id, window_id, layer;
  CGSize              size;
  CFDictionaryRef     window;
  CGRect              rect;
  char                *app_name, *window_name, *window_title, *owner_name, *uuid, *display_uuid;
  bool                is_focused, is_visible, is_minimized;
  struct kinfo_proc   pid_info;
  struct Vector       *space_ids_v;
  AXUIElementRef      *app;
  ProcessSerialNumber psn;
} window_t;

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
///////////////////////////////////////////////////
int get_window_display_id(struct window_t *window);
int get_window_space_id(struct window_t *window);
char *get_window_display_uuid(struct window_t *window);
bool get_window_is_minimized(struct window_t *window);
CFStringRef display_manager_active_display_uuid(void);
CGRect display_manager_dock_rect(void);
uint64_t display_space_id(uint32_t did);
bool display_manager_dock_hidden(void);
int display_manager_dock_orientation(void);
uint32_t display_manager_active_display_count(void);
uint32_t display_manager_active_display_id(void);
uint32_t display_manager_main_display_id(void);
uint32_t display_manager_active_display_count(void);
uint32_t *display_manager_active_display_list(uint32_t *count);
uint64_t *get_display_id_space_ids(uint32_t did, int *count);
bool display_manager_dock_hidden(void);
int display_manager_dock_orientation(void);
CGRect display_manager_dock_rect(void);
char * get_focused_window_title();
char *windowTitle(char *appName, char *windowName);
void PrintWindow(CFDictionaryRef window, void *ctxPtr);
int EnumerateWindows(char *pattern, void (*callback)(CFDictionaryRef window, void *callback_data), void *callback_data);
int get_window_connection_id(struct window_t *window);
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
int get_front_window_pid(void);
int get_pid_window_id(const int PID);
void move_window_id(const int WINDOW_ID, const int X, const int Y);
char *get_window_id_title(const int WINDOW_ID);
window_t *get_window_id(const int WINDOW_ID);
CFDictionaryRef window_id_to_window(const int WINDOW_ID);
window_t *get_pid_window(const int PID);
int get_display_width();
bool resize_window(const CFDictionaryRef WINDOW, const int WIDTH, const int HEIGHT);
bool move_window(CFDictionaryRef w, const int X, const int Y);
///////////////////////////////////////////////////
