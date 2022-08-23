#pragma once
#include "wrec-common/wrec-common.h"
///////////////////////////////////////
struct loaded_png_file_t *load_png_file_pixels(char *png_file);
struct image_dimensions_t *get_png_file_dimensions(char *path);
struct qoi_encode_to_file_result_t *qoi_encode_to_file(struct qoi_encode_to_file_request_t *req);
int qoi_decode_file(char *qoi_file);
struct qoi_encode_result_t *qoi_encode_memory(const void *rgb_pixels, int width, int height);
void *qoi_decode_memory(const void *qoi_data, int qoi_data_size);
///////////////////////////////////////
