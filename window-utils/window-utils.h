#pragma once
#ifndef DEFAULT_LOGLEVEL
#define DEFAULT_LOGLEVEL    1
#endif
#include "ansi-codes/ansi-codes.h"
#include "app-utils.h"
#include "c_stringfn/include/stringfn.h"
#include <assert.h>
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
int get_front_window_pid(void);

///////////////////////////////////////////////////
