#pragma once
//////////////////////////////////////////////////////
#include <stdbool.h>
//////////////////////////////////////////////////////
#include "window-utils/window-utils.h"
#include "wrec-cli/wrec-cli.h"
#include "wrec-common/wrec-common.h"
//////////////////////////////////////////////////////
bool capture_to_file_image(const int WINDOW_ID, const char *FILE_NAME);
unsigned char *capture_to_file_image_data(const int WINDOW_ID);
bool capture_to_file_image_resize_factor(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_FACTOR);
bool capture_to_file_image_resize_width(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_WIDTH);
bool capture_to_file_image_resize_height(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_HEIGHT);
//////////////////////////////////////////////////////
