#pragma once
//////////////////////////////////////////////////////
#include <stdbool.h>
//////////////////////////////////////////////////////
#include "window-utils/window-utils.h"
//////////////////////////////////////////////////////
bool capture_to_file_image(const int WINDOW_ID, const char *FILE_NAME);
unsigned char *capture_to_file_image_data(const int WINDOW_ID);

//////////////////////////////////////////////////////
