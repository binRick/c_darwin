#pragma once
#ifndef CAPTURE_SAVE_H
#define CAPTURE_SAVE_H
//////////////////////////////////////
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "c_vector/vector/vector.h"
#include "capture/type/type.h"
#include "image/type/type.h"

struct save_capture_result_t {
  size_t qty, bytes;
  unsigned long dur, started;
};

struct save_capture_result_t *save_capture_type_results(enum capture_type_id_t type, enum image_type_id_t format, struct Vector *results_v, size_t concurrency, char *save_directory, bool progress_bar_mode);

#endif
