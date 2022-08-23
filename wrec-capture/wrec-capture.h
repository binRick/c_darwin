#pragma once
//////////////////////////////////////////////////////
#include <stdbool.h>
//////////////////////////////////////////////////////
bool capture_to_file_image(const int WINDOW_ID, const char *FILE_NAME);
bool capture_to_file_image_resize_factor(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_FACTOR);
bool capture_to_file_image_resize_width(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_WIDTH);
bool capture_to_file_image_resize_height(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_HEIGHT);
unsigned char *capture_to_file_image_data(const int WINDOW_ID);
void wait_for_control_d();
//////////////////////////////////////////////////////
