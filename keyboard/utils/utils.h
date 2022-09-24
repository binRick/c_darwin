#pragma once
#ifndef KEYBOARD_UTILS_H
#define KEYBOARD_UTILS_H
//////////////////////////////////////
#include "osx-keys/osx-keys.h"
#include <assert.h>
#include <ctype.h>
#include <Carbon/Carbon.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
void typeString(char *str);
void keyHit(CGKeyCode kc, CGEventFlags flags);
void keyPress(CGKeyCode kc, CGEventFlags flags);
void keyRelease(CGKeyCode kc, CGEventFlags flags);
#endif
