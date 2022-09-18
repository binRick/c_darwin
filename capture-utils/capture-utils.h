#pragma once
#ifndef CAPTURE_UTILS_H
#define CAPTURE_UTILS_H
//////////////////////////////////////
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
typedef size_t (^capture_type_get_default_capture_id)(void);
typedef bool (^capture_type_validate_capture_id)(size_t id);
typedef CGImageRef (^capture_type_capture)(size_t id);
enum capture_type_id_t {
  CAPTURE_TYPE_WINDOW,
  CAPTURE_TYPE_DISPLAY,
  CAPTURE_TYPE_SPACE,
  CAPTURE_TYPES_QTY,
};
struct capture_type_t {
  const char                          *name;
  capture_type_get_default_capture_id get_default_capture_id;
  capture_type_validate_capture_id    validate_capture_id;
  capture_type_capture                capture;
};
struct capture_type_t capture_types[CAPTURE_TYPES_QTY + 1];
CGImageRef capture_type_id_or_default_capture_type_id(enum capture_type_id_t capture_type_id, size_t capture_id);
bool validate_capture_type_id(enum capture_type_id_t capture_type_id, size_t capture_id);
size_t get_default_capture_type_id(enum capture_type_id_t capture_type_id);
size_t validate_id_or_get_default_capture_id(enum capture_type_id_t capture_type_id, size_t capture_id);
CGImageRef capture(enum capture_type_id_t capture_type_id, size_t capture_id);
#endif
