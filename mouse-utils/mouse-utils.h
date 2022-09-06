#pragma once
#ifndef MOUSE_UTILS_H
#define MOUSE_UTILS_H
//////////////////////////////////////
#include <assert.h>
#include <ctype.h>
#include <Carbon/Carbon.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
void processCommand(const char *cmd);
void print_msg(const char *msg);
CGPoint mouseLoc();
CGEventType getMouseButton(int btn, int btnState);
void mouseEvent(int btn, int btnState, int clickType);
void mouseMove(int posX, int posY);
void mouseMoveTo(int posX, int posY, float speed);
void mousePress(int btn, int clickType);
void mouseRelease(int btn, int clickType);
void mouseClick(int btn, int clickType);
void mouseDrag(int btn, int posX, int posY);
void typeString(char *str);
void keyHit(CGKeyCode kc, CGEventFlags flags);
void keyPress(CGKeyCode kc, CGEventFlags flags);
void keyRelease(CGKeyCode kc, CGEventFlags flags);

#endif
