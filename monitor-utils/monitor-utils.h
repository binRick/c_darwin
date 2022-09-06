#pragma once
#ifndef MONITOR_UTILS_H
#define MONITOR_UTILS_H
//////////////////////////////////////
#include "c_vector/vector/vector.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
struct monitor_t {
  char   *name, *uuid;
  bool   primary;
  size_t width_pixels, height_pixels;
  size_t width_mm, height_mm, modes_qty;
  int    id, refresh_hz;
};
struct Vector *get_monitors_v(void);
void print_monitors();
#endif
