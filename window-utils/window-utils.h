#pragma once
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
uint64_t *get_display_id_space_ids(uint32_t did, int *count);
bool display_manager_dock_hidden(void);
int display_manager_dock_orientation(void);
CGRect display_manager_dock_rect(void);
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
int get_display_width();
bool resize_window(struct window_t *w, const int WIDTH, const int HEIGHT);
bool move_window(struct window_t *w, const int X, const int Y);
struct window_t *get_focused_window();
uint32_t display_active_display_id(void);
int get_focused_window_id();
bool get_window_is_visible(struct window_t *window);
ProcessSerialNumber get_window_ProcessSerialNumber(struct window_t *w);
int get_window_id_pid(int window_id);
///////////////////////////////////////////////////
