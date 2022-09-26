#pragma once
#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H
#include <Carbon/Carbon.h>
#include <fnmatch.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
///////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "libspng/spng/spng.h"
#include "tesseract/capi.h"
///////////////////////////////////////////////////
#define PREVIEW_FACTOR      2
#define THUMBNAIL_FACTOR    5
#define CAPTURE_FACTOR      1
struct spng_info_t {
  int           filter_method, interlace_method, width, height, bit_depth, color_type, compression_method;
  char          *color_name;
  unsigned long started, dur;
};
enum image_type_id_t {
  IMAGE_TYPE_PNG,
  IMAGE_TYPE_GIF,
  IMAGE_TYPE_TIFF,
  IMAGE_TYPE_JPEG,
  IMAGE_TYPE_BMP,
  IMAGE_TYPE_QOI,
  IMAGE_TYPES_QTY,
};
struct image_type_t {
  const char    *name, *file_extension;
  CFStringRef   (^get_format)(void);
  bool          (^load_buffer_to_tesseract_api)(unsigned char *image_buf, size_t len, TessBaseAPI *api);
  bool          (^validate_header)(unsigned char *header_buf);
  bool          (^validate_buffer)(unsigned char *image_buf, size_t len);
  bool          (^validate_file)(char *image_path);
  unsigned char *(^decode_buffer_to_rgb_buffer)(char *image_buf, size_t image_buf_len, size_t *len);
  unsigned char *(^decode_file_to_rgb_buffer)(char *image_path, size_t *len);
  unsigned char *(^encode_rgb_buffer_to_buffer)(unsigned char *image_buf, size_t image_buf_len, size_t *len);
  bool          *(^encode_rgb_buffer_to_file)(unsigned char *image_buf, size_t image_buf_len, char *image_path);
  bool          *(^encode_buffer_to_image_type_file)(unsigned char *image_buf, size_t image_buf_len, enum image_type_id_t image_type_id, char *image_path);
  unsigned char *(^encode_buffer_to_image_type_buffer)(unsigned char *image_buf, size_t image_buf_len, enum image_type_id_t image_type_id, size_t *len);
  unsigned char *(^read_file_header)(char *image_buf);
  unsigned char *(^read_buffer_header)(unsigned char *image_buf);
  bool          (^get_dimensions_from_header)(unsigned char *header, int *width, int *height);
  bool          (^get_dimensions_from_buffer)(unsigned char *header, size_t len, int *width, int *height);
  bool          (^get_dimensions_from_file)(char *image_path, int *with, int *height);
};
char * convert_png_to_grayscale(char *png_file, size_t resize_factor);
struct image_type_t image_types[IMAGE_TYPES_QTY + 1];
CGImageRef resize_cgimage(CGImageRef imageRef, int width, int height);
unsigned char *rgb_pixels_to_png_pixels(int width, int height, const void *rgb, const void *mask, long *outsize);
bool write_cgimage_ref_to_tif_file_path(CGImageRef im, char *tif_file_path);
CGImageRef cgimageref_to_grayscale(CGImageRef im);
CGImageRef resize_cgimage_factor(CGImageRef imageRef, double resize_factor);
CGImageRef resize_png_file_factor(FILE *fp, double resize_factor);
CGImageRef rgb_pixels_to_png_cgimage_ref(unsigned char *rgb_pixels, int width, int height);
CGImageRef png_file_to_grayscale_cgimage_ref_resized(FILE *fp, double resize_factor);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool save_cgref_to_image_type_file(enum image_type_id_t image_type, CGImageRef image, char *image_file);
unsigned char *save_cgref_to_image_type_memory(enum image_type_id_t image_type, CGImageRef image, size_t *len);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char *save_cgref_to_tiff_memory(CGImageRef image, size_t *len);
unsigned char *save_cgref_to_bmp_memory(CGImageRef image, size_t *len);
unsigned char *save_cgref_to_gif_memory(CGImageRef image, size_t *len);
unsigned char *save_cgref_to_jpeg_memory(CGImageRef image, size_t *len);
unsigned char *save_cgref_to_png_memory(CGImageRef image, size_t *len);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool save_cgref_to_png_file(CGImageRef image, char *image_file);
bool save_cgref_to_bmp_file(CGImageRef image, char *image_file);
bool save_cgref_to_gif_file(CGImageRef image, char *image_file);
bool save_cgref_to_jpeg_file(CGImageRef image, char *image_file);
bool save_cgref_to_tiff_file(CGImageRef image, char *image_file);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char *cgimage_ref_to_rgb_pixels(CGImageRef image_ref, size_t *len);
bool save_cgref_to_qoi_file(CGImageRef image_ref, char *image_file);
unsigned char *cgref_to_qoi_memory(CGImageRef image_ref, size_t *qoi_len);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int imagequant_encode_rgb_pixels_to_png_file(unsigned char *raw_rgba_pixels, int width, int height, char *png_file, int min_quality, int max_quality);
CGImageRef png_fp_to_cgimage(FILE *fp);
unsigned char *imagequant_encode_rgb_pixels_to_png_buffer(unsigned char *raw_rgba_pixels, int width, int height, int min_quality, int max_quality, size_t *len);
bool image_conversions();
const char *color_type_str(enum spng_color_type color_type);

struct spng_info_t *spng_test(FILE *fp);
bool compress_png_file(char *file);
#endif
