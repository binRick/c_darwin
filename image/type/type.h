#pragma once
#ifndef IMAGE_TYPE_H
#define IMAGE_TYPE_H
//////////////////////////////////////
#include "vips/vips.h"
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
enum image_type_id_t {
  IMAGE_TYPE_PNG = 1,
  IMAGE_TYPE_GIF,
  IMAGE_TYPE_TIFF,
  IMAGE_TYPE_JPEG,
  IMAGE_TYPE_BMP,
  IMAGE_TYPE_QOI,
  IMAGE_TYPE_QOIR,
  IMAGE_TYPE_RGB,
  IMAGE_TYPE_CGIMAGE,
  IMAGE_TYPE_WEBP,
  IMAGE_TYPE_HEIF,
  IMAGE_TYPE_PPM,
  IMAGE_TYPE_SVG,
  IMAGE_TYPES_QTY,
};
struct VipsImage;
typedef int (*image_target_file_saver_t)(struct VipsImage *in, char *filename, ...);
static const image_target_file_saver_t image_target_file_savers[] = {
  [IMAGE_TYPE_PNG]  = vips_pngsave,
  [IMAGE_TYPE_GIF]  = vips_gifsave,
  [IMAGE_TYPE_JPEG] = vips_jpegsave,
  [IMAGE_TYPE_TIFF] = vips_tiffsave,
  [IMAGE_TYPE_WEBP] = vips_webpsave,
//  [IMAGE_TYPE_SVG]  = vips_svgsave,
//  [IMAGE_TYPE_QOI] = vips_qoisave,
//  [IMAGE_TYPE_QOIR] = vips_qoirsave,
};
#endif
