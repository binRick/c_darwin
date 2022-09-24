#pragma once
#ifndef MOUSE_UTILS_C
#define MOUSE_UTILS_C
////////////////////////////////////////////
#include "mouse/utils/utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "frameworks/frameworks.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include <Carbon/Carbon.h>
#include <Carbon/Carbon.h>
#define MOUSE_SPEED         4000
#define MOUSE_RESOLUTION    2.5
#define NO_MOUSE_BUTTON     0
#define LEFT_MOUSE          1
#define RIGHT_MOUSE         2
#define MOUSE_DOWN          0
#define MOUSE_UP            1
#define MOUSE_DRAGGED       2
#define MOUSE_MOVED         2
#define SINGLE_CLICK        1
#define DOUBLE_CLICK        2
#define TRIPLE_CLICK        3
////////////////////////////////////////////
static bool               mouse_utils_DEBUG_MODE = false;
//static CGEventTapLocation tapA = kCGAnnotatedSessionEventTap;
static CGEventTapLocation tapH = kCGHIDEventTap;
///////////////////////////////////////////////////////////////////////

CGPoint *get_mouse_location(){
  CGEventRef Event = CGEventCreate(nil);
  CGPoint    *loc  = calloc(1, sizeof(CGPoint));
  CGPoint    L     = CGEventGetLocation(Event);

  loc->x = L.x;
  loc->y = L.y;
  if (mouse_utils_DEBUG_MODE) {
    log_info("%dx%d", (int)loc->x, (int)loc->y);
  }
  CFRelease(Event);
  return(loc);
}

void move_mouse(int posX, int posY) {
  CGPoint loc = { .x = posX, .y = posY };

  return(move_mouse_to_point(loc));
}

void move_mouse_to_point(CGPoint loc){
  CGWarpMouseCursorPosition(loc);
}

void right_click_mouse(){
  CGPoint    *loc           = get_mouse_location();
  CGEventRef mouseClickDown = CGEventCreateMouseEvent(
    NULL, kCGEventRightMouseDown, *loc, kCGMouseButtonRight);
  CGEventRef mouseClickUp = CGEventCreateMouseEvent(
    NULL, kCGEventRightMouseUp, *loc, kCGMouseButtonRight);

  CGEventPost(tapH, mouseClickDown);
  CGEventPost(tapH, mouseClickUp);
  CFRelease(mouseClickDown);
  CFRelease(mouseClickUp);
}

void right_up_mouse(){
  CGPoint    *loc         = get_mouse_location();
  CGEventRef mouseClickUp = CGEventCreateMouseEvent(
    NULL, kCGEventRightMouseUp, *loc, kCGMouseButtonRight);

  CGEventPost(tapH, mouseClickUp);
  CFRelease(mouseClickUp);
}

void right_down_mouse(){
  CGPoint    *loc           = get_mouse_location();
  CGEventRef mouseClickDown = CGEventCreateMouseEvent(
    NULL, kCGEventRightMouseDown, *loc, kCGMouseButtonRight);

  CGEventPost(tapH, mouseClickDown);
  CFRelease(mouseClickDown);
}

void left_up_mouse(){
  CGPoint    *loc         = get_mouse_location();
  CGEventRef mouseClickUp = CGEventCreateMouseEvent(
    NULL, kCGEventLeftMouseUp, *loc, kCGMouseButtonLeft);

  CGEventPost(tapH, mouseClickUp);
  CFRelease(mouseClickUp);
}

void left_down_mouse(){
  CGPoint    *loc           = get_mouse_location();
  CGEventRef mouseClickDown = CGEventCreateMouseEvent(
    NULL, kCGEventLeftMouseDown, *loc, kCGMouseButtonLeft);

  CGEventPost(tapH, mouseClickDown);
  CFRelease(mouseClickDown);
}

void left_click_mouse(){
  CGPoint    *loc           = get_mouse_location();
  CGEventRef mouseClickDown = CGEventCreateMouseEvent(
    NULL, kCGEventLeftMouseDown, *loc, kCGMouseButtonLeft);
  CGEventRef mouseClickUp = CGEventCreateMouseEvent(
    NULL, kCGEventLeftMouseUp, *loc, kCGMouseButtonLeft);

  CGEventPost(tapH, mouseClickDown);
  CGEventPost(tapH, mouseClickUp);
  CFRelease(mouseClickDown);
  CFRelease(mouseClickUp);
}

////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__mouse_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_mouse_utils") != NULL) {
    log_debug("Enabling mouse-utils Debug Mode");
    mouse_utils_DEBUG_MODE = true;
  }
}

#endif
