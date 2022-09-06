#pragma once
#ifndef MOUSE_UTILS_C
#define MOUSE_UTILS_C
////////////////////////////////////////////
#include "mouse-utils/mouse-utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
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
static bool          mouse_utils_DEBUG_MODE = false;
static volatile bool bDragging              = false;

///////////////////////////////////////////////////////////////////////
void mouseMove(int posX, int posY) {
  CGPoint dest = { .x = posX, .y = posY };

  CGWarpMouseCursorPosition(dest);
  if (bDragging) {
    mouseEvent(LEFT_MOUSE, MOUSE_DRAGGED, 0);
  } else {
    mouseEvent(NO_MOUSE_BUTTON, MOUSE_MOVED, 0);
  }
}

void mouseMoveTo(int posX, int posY, float speed) {
  CGPoint currLoc = mouseLoc();
  CGPoint destLoc = { .x = posX, .y = posY };
  float   x = currLoc.x;
  float   y = currLoc.y;
  float   xrat, yrat;

  int     diffX = abs((int)currLoc.x - (int)destLoc.x);
  int     diffY = abs((int)currLoc.y - (int)destLoc.y);
  int     dirX  = currLoc.x > destLoc.x ? -1 : 1;
  int     dirY  = currLoc.y > destLoc.y ? -1 : 1;

  if (diffX == 0 && diffY == 0) {
    return;
  }

  if (diffX > diffY) {
    xrat = MOUSE_RESOLUTION * dirX;
    if (diffY == 0) {
      yrat = 0;
    } else {
      yrat = (((float)diffY / diffX) * dirY) * MOUSE_RESOLUTION;
    }
  } else {
    yrat = MOUSE_RESOLUTION * dirY;
    if (diffX == 0) {
      xrat = 0;
    } else {
      xrat = (((float)diffX / diffY) * dirX) * MOUSE_RESOLUTION;
    }
  }

  int   xArrived = 0, yArrived = 0, diff;
  float accelerant;

  while (!xArrived && !yArrived) {
    diffX      = abs((int)destLoc.x - (int)x);
    diffY      = abs((int)destLoc.y - (int)y);
    diff       = diffX > diffY ? diffX : diffY;
    accelerant = diff > 70 ? diff / 40 : (diff > 40 ? diff / 20 : 1);

    if (!xArrived && diffX < abs((int)xrat)) {
      xArrived = 1;
      x        = destLoc.x;
    } else {
      x += xrat * accelerant;
    }

    if (!yArrived && diffY < abs((int)yrat)) {
      yArrived = 1;
      y        = destLoc.y;
    } else {
      y += yrat * accelerant;
    }

    mouseMove((int)x, (int)y);
    usleep((int)(speed * (MOUSE_SPEED * MOUSE_RESOLUTION)));
  }
} /* mouseMoveTo */

void mousePress(int btn, int clickType) {
  mouseEvent(btn, MOUSE_DOWN, clickType);
}

void mouseRelease(int btn, int clickType) {
  mouseEvent(btn, MOUSE_UP, clickType);
}

void mouseClick(int btn, int clickType) {
  mousePress(btn, clickType);
  usleep(400000);
  mouseRelease(btn, clickType);
}

void mouseDrag(int btn, int posX, int posY) {
  bDragging = true;
  mouseEvent(btn, MOUSE_DOWN, SINGLE_CLICK);
  usleep(50000);
  mouseMoveTo(posX, posY, 5);
  usleep(50000);
  mouseEvent(btn, MOUSE_UP, SINGLE_CLICK);
}
////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__mouse_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_mouse_utils") != NULL) {
    log_debug("Enabling mouse-utils Debug Mode");
    mouse_utils_DEBUG_MODE = true;
  }
}

#endif
