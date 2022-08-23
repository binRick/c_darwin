#pragma once
////////////////////////////////////////////
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
////////////////////////////////////////////
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "libfort/lib/fort.h"
#include "string-utils/string-utils.h"
#include "submodules/tinydir/tinydir.h"
#include "tempdir.c/tempdir.h"
#include "wrec-common/wrec-common.h"
///////////////////////////////////////
struct loaded_png_file_t *load_png_file_pixels(char *png_file);
struct image_dimensions_t *get_png_file_dimensions(char *path);
struct qoi_encode_to_file_result_t *qoi_encode_to_file(struct qoi_encode_to_file_request_t *req);
int qoi_decode_file(char *qoi_file);
struct qoi_encode_result_t *qoi_encode_memory(const void *rgb_pixels, int width, int height);
void *qoi_decode_memory(const void *qoi_data, int qoi_data_size);
///////////////////////////////////////
