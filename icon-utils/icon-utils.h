#pragma once
#ifndef ICON_UTILS_H
#define ICON_UTILS_H
//////////////////////////////////////
#include "icns/src/icns.h"
#include <assert.h>
#include <ApplicationServices/ApplicationServices.h>
#include <ctype.h>
#include <Carbon/Carbon.h>
#include <CoreVideo/CVPixelBuffer.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
bool write_app_icon_from_png(char *app_path, char *png_file_path);
bool write_app_icon_to_icns(char *app_path, char *icns_file_path);
bool write_app_icon_to_png(char *app_path, char *png_file_path, size_t icon_size);
char *get_icon_size_name(size_t icon_size);
bool app_icon_size_is_valid(size_t icon_size);
#endif
