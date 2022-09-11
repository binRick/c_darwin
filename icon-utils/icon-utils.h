#pragma once
#ifndef ICON_UTILS_H
#define ICON_UTILS_H
//////////////////////////////////////
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
void get_icon_data_from_path(char *path);
bool write_app_icon_to_png(char *app_path, char *png_file_path);
bool save_app_icon_to_icns_file(char *app_path, char *icns_file_path);

#endif
