#pragma once
#ifndef OUTPUT_UTILS_H
#define OUTPUT_UTILS_H
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
//////////////////////////////////////
const enum output_mode_type_t DEFAULT_OUTPUT_MODE;
enum output_mode_type_t {
  OUTPUT_MODE_TEXT = 1,
  OUTPUT_MODE_TABLE,
  OUTPUT_MODE_JSON,
  OUTPUT_MODES_QTY,
};
const char *output_modes[OUTPUT_MODES_QTY + 1];
void output_utils_fxn1(void);
#endif
