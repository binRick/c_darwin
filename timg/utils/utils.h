#pragma once
#ifndef TIMG_UTILS_H
#define TIMG_UTILS_H
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
int timg_utils_run_help(void);
int timg_utils_image(char *file);
int timg_utils_titled_image(char *file);
int timg_utils_images(struct Vector *v);
int timg_utils_test_images(void);
bool write_binary_path();
#endif
