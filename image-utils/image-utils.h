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
///////////////////////////////////////////////////
typedef CFStringRef (^image_format_b)(void);
typedef bool (^image_format_validator_b)(unsigned char *image_buf);
typedef unsigned char *(^image_format_read_buffer_header_b)(unsigned char *image_buf);
typedef unsigned char *(^image_format_read_file_header_b)(char *image_path);
typedef bool (^image_format_file_get_dimensions)(char *image_path);
typedef bool (^image_format_buffer_get_dimensions)(unsigned char *image_buf, size_t len, int *width, int *height);
typedef bool (^image_format_get_dimensions_from_header)(unsigned char *header, int *width, int *height);
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
  const char                              *name, *extension;
  image_format_b                          format;
  image_format_validator_b                validator;
  image_format_read_file_header_b         read_file_header;
  image_format_read_buffer_header_b       read_buffer_header;
  image_format_get_dimensions_from_header get_dimensions_from_header;
  image_format_buffer_get_dimensions      get_dimensions;
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
#endif
