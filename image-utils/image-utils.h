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
typedef bool (^image_format_buffer_get_dimensions)(unsigned char *image_buf);
typedef bool (^image_format_get_dimensions_from_header)(unsigned char *header, int *width, int *height);
enum image_type_id_t {
    IMAGE_TYPE_PNG,
    IMAGE_TYPE_GIF,
    IMAGE_TYPE_JPEG,
    IMAGE_TYPE_BMP,
    IMAGE_TYPE_TIFF,
    IMAGE_TYPES_QTY,
};
struct image_type_t {
  const char *name, *extension;
  image_format_b format;
  image_format_validator_b validator;
  image_format_read_file_header_b read_file_header;
  image_format_read_buffer_header_b read_buffer_header;
  image_format_get_dimensions_from_header get_dimensions;
};
static int read_32bytes_big_endian_image_buffer(uint8_t *buf) { return(buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]); }
static int read_16bytes_little_endian_image_buffer(uint8_t *buf) { return(buf[1] << 8 | buf[0]); }
static void read_image_format_file(const char *path, unsigned char *buf, int len, int offset) {
  int fd = open(path, O_RDONLY); assert(fd>=0);   assert(pread(fd, buf, len, offset) >= 0); close(fd);
}
static struct image_type_t image_types[IMAGE_TYPES_QTY] = {
  [IMAGE_TYPE_PNG] =  { 
    .extension = "png", .name = "PNG",  
    .format = ^CFStringRef(void){ return kUTTypePNG; }, 
    .validator = ^bool(unsigned char *image_buf){ return(('P' == image_buf[1] && 'N' == image_buf[2] && 'G' == image_buf[3]) ? true : false); }, 
    .read_file_header = ^unsigned char *(char *image_path){ unsigned char *buf = calloc(8,sizeof(unsigned char)); read_image_format_file(image_path, buf, 8, 16); return(buf); },
    .get_dimensions = ^bool (unsigned char *header, int *width, int *height){ 
      *width = read_32bytes_big_endian_image_buffer(header); 
      *height = read_32bytes_big_endian_image_buffer(header + 4); 
      return((*width>0 && *height>0) ? true : false); 
    },
  },
  [IMAGE_TYPE_GIF] =  { 
    .extension = "gif",.name = "GIF",  .format = ^CFStringRef(void){ return kUTTypeGIF; }, 
    .validator = ^bool(unsigned char *image_buf){ return(('G' == image_buf[0] && 'I' == image_buf[1] && 'F' == image_buf[2]) ? true : false); }, 
    .read_file_header = ^unsigned char *(char *image_path){ unsigned char *buf = calloc(4,sizeof(unsigned char)); read_image_format_file(image_path, buf, 4, 6); return(buf); },
    .get_dimensions = ^bool (unsigned char *buf, int *width, int *height){ 
      *width = read_16bytes_little_endian_image_buffer(buf); 
      *height = read_16bytes_little_endian_image_buffer(buf + 2); 
      return((*width>0 && *height>0) ? true : false); 
    },
  },
  [IMAGE_TYPE_JPEG] = { 
    .extension = "jpeg",.name = "JPEG", .format = ^CFStringRef(void){ return kUTTypeJPEG; }, 
    .validator = ^bool(unsigned char *image_buf){ return((0xff == image_buf[0] && 0xd8 == image_buf[1]) ? true : false); }, 
    .get_dimensions = ^bool (__attribute__((unused))unsigned char *buf, __attribute__((unused))int *width, __attribute__((unused))int *height){ return false; },
  },
  [IMAGE_TYPE_BMP] =  { 
    .extension = "bmp",.name = "BMP",  .format = ^CFStringRef(void){ return kUTTypeBMP; }, 
    .validator = ^bool(unsigned char *image_buf){ return((image_buf != NULL) ? true : false); }, 
    .get_dimensions = ^bool (__attribute__((unused))unsigned char *buf, __attribute__((unused))int *width, __attribute__((unused))int *height){ return false; },
  },
  [IMAGE_TYPE_TIFF] = { 
    .extension = "tiff",.name = "TIFF", .format = ^CFStringRef(void){ return kUTTypeTIFF; }, 
    .validator = ^bool(unsigned char *image_buf){ return((image_buf != NULL) ? true : false); }, 
    .get_dimensions = ^bool (__attribute__((unused))unsigned char *buf, __attribute__((unused))int *width, __attribute__((unused))int *height){ return false; },
  },
};
CGImageRef resize_cgimage(CGImageRef imageRef, int width, int height);
void * CompressToPNG(int width, int height, const void *rgb, const void *mask, long *outsize);
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
bool save_cgref_to_png_file(CGImageRef image, char *image_file) ;
bool save_cgref_to_bmp_file(CGImageRef image, char *image_file) ;
bool save_cgref_to_gif_file(CGImageRef image, char *image_file) ;
bool save_cgref_to_jpeg_file(CGImageRef image, char *image_file) ;
bool save_cgref_to_tiff_file(CGImageRef image, char *image_file) ;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
