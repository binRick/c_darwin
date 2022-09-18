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
CGPoint *get_mouse_location(void);
void move_mouse(int posX, int posY);
void move_mouse_to_point(CGPoint loc);
void right_up_mouse();
void right_down_mouse();
void left_up_mouse();
void left_down_mouse();
void left_click_mouse();
void right_click_mouse();
//////////////////////////////////////

#endif
