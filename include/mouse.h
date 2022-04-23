#include <ApplicationServices/ApplicationServices.h>
//#include "../include/includes.h"

struct MouseEvent {
  CGFloat     x;
  CGFloat     y;
  CGEventType type;
};


const char *LEFT_DOWN  = "left-down";
const char *LEFT_UP    = "left-up";
const char *RIGHT_DOWN = "right-down";
const char *RIGHT_UP   = "right-up";
const char *MOVE       = "move";
const char *LEFT_DRAG  = "left-drag";
const char *RIGHT_DRAG = "right-drag";


bool IsMouseEvent(CGEventType type) {
  return(  type == kCGEventLeftMouseDown
        || type == kCGEventLeftMouseUp
        || type == kCGEventRightMouseDown
        || type == kCGEventRightMouseUp
        || type == kCGEventMouseMoved
        || type == kCGEventLeftMouseDragged
        || type == kCGEventRightMouseDragged);
}

void move_mouse(const CGPoint);

void test_move_mouse();
