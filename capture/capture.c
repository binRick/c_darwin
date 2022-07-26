#pragma once
#define DEBUG_STDOUT               true
#define STDOUT_READ_BUFFER_SIZE    1024 * 2
#define BYTES_PER_COMPONENT        1
#define BITS_PER_COMPONENT         8
#define COMPONENTS_PER_PIXEL       4
#define DEBUG_IMAGE_RESIZE         false
#define DEBUG_MEASUREMENTS         true
///////////////////////////////////////////////////////
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
///////////////////////////////////////////////////////
#include "bench/bench.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "capture/capture.h"
#include "core-utils/core-utils.h"
#include "ms/ms.h"
#include "tempdir.c/tempdir.h"
static const char *tempdir_path = NULL;
struct img_data {
  size_t  width;
  size_t  height;
  uint8_t *rgba;
};

static void debug_resize(const char *FILE_NAME, char *RESIZE_BY, int RESIZE_VALUE, int ORIGINAL_WIDTH, int ORIGINAL_HEIGHT, int RESIZE_FACTOR, int RESIZE_WIDTH, int RESIZE_HEIGHT);


static bool cgimage_save_png(const CGImageRef image, const char *filename) {
  //MEASURE(measurement_cgimage_save_png)
  bool                  success = false;
  CFStringRef           path;
  CFURLRef              url;
  CGImageDestinationRef destination;
  {
    path = CFStringCreateWithCString(NULL, filename, kCFStringEncodingUTF8);
    url  = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, 0);
    CFRelease(path);
    destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
    CGImageDestinationAddImage(destination, image, nil);
    success = CGImageDestinationFinalize(destination);
  }

//   END_MEASURE(measurement_cgimage_save_png)
//   MEASURE_SUMMARY(measurement_cgimage_save_png);

  if (!success) {
    fprintf(stderr, "Failed to write image to %s", filename);
  }
  CFRelease(url);
  return(fsio_file_exists(filename));
}


void CGImageDumpInfo(CGImageRef image, bool shouldPrintPixelData) {
  size_t          width              = CGImageGetWidth(image);
  size_t          height             = CGImageGetHeight(image);
  CGColorSpaceRef colorSpace         = CGImageGetColorSpace(image);
  size_t          bytesPerRow        = CGImageGetBytesPerRow(image);
  size_t          bitsPerPixel       = CGImageGetBitsPerPixel(image);
  size_t          bitsPerComponent   = CGImageGetBitsPerComponent(image);
  size_t          componentsPerPixel = bitsPerPixel / bitsPerComponent;
  CGBitmapInfo    bitmapInfo         = CGImageGetBitmapInfo(image);

  printf("\n");
  printf("CGImageGetWidth: %lu\n", width);
  printf("CGImageGetHeight: %lu\n", height);
  printf("CGColorSpaceModel: %d\n", CGColorSpaceGetModel(colorSpace));
  printf("CGImageGetBytesPerRow: %lu\n", bytesPerRow);
  printf("CGImageGetBitsPerPixel: %lu\n", bitsPerPixel);
  printf("CGImageGetBitsPerComponent: %lu\n", bitsPerComponent);
  printf("components per pixel: %lu\n", componentsPerPixel);
  printf("CGImageGetBitmapInfo: 0x%.8X\n", (unsigned)bitmapInfo);
  printf("->kCGBitmapAlphaInfoMask = %s\n", (bitmapInfo & kCGBitmapAlphaInfoMask) ? "YES" : "NO");
  printf("->kCGBitmapFloatComponents = %s\n", (bitmapInfo & kCGBitmapFloatComponents) ? "YES" : "NO");
  printf("->kCGBitmapByteOrderMask = 0x%.8X\n", (bitmapInfo & kCGBitmapByteOrderMask));
  printf("->->kCGBitmapByteOrderDefault = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrderDefault) ? "YES" : "NO");
  printf("->->kCGBitmapByteOrder16Little = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder16Little) ? "YES" : "NO");
  printf("->->kCGBitmapByteOrder32Little = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder32Little) ? "YES" : "NO");
  printf("->->kCGBitmapByteOrder16Big = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder16Big) ? "YES" : "NO");
  printf("->->kCGBitmapByteOrder32Big = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder32Big) ? "YES" : "NO");

  if (!shouldPrintPixelData) {
    return;
  }

  //   PixelBytesArrayDump( CGImageCopyRawData(image, 0), width, height, bytesPerRow, componentsPerPixel);

  printf("\n");
} /* CGImageDumpInfo */


static CFStringRef image_file_name(const char *file_extension) {
  if (tempdir_path == NULL) {
    tempdir_path = gettempdir();
  }
  char      *c_file_name;
  time_t    t  = time(NULL);
  struct tm tm = *localtime(&t);

  asprintf(c_file_name, "%s/Image_%d-%02d-%02d_%02d.%02d.%02d.%s",
           tempdir_path,
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
           tm.tm_hour, tm.tm_min, tm.tm_sec, file_extension);
  return(CFStringCreateWithCString(kCFAllocatorDefault, c_file_name, kCFStringEncodingUTF8));
}


CGImageRef resize_cgimage(CGImageRef imageRef, int width, int height) {
  CGRect       newRect = CGRectIntegral(CGRectMake(0, 0, width, height));
  CGContextRef context = CGBitmapContextCreate(NULL, width, height,
                                               CGImageGetBitsPerComponent(imageRef),
                                               CGImageGetBytesPerRow(imageRef),
                                               CGImageGetColorSpace(imageRef),
                                               CGImageGetBitmapInfo(imageRef));

  CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
  CGContextDrawImage(context, newRect, imageRef);
  CGImageRef newImageRef = CGBitmapContextCreateImage(context);

  CGContextRelease(context);
  return(newImageRef);
}


CGImageRef capture_window_id_cgimageref(const int WINDOW_ID){
  CFDictionaryRef W   = window_id_to_window(WINDOW_ID);
  int             WID = (int)CFDictionaryGetInt(W, kCGWindowNumber);

  assert(WID == WINDOW_ID);
  CGSize     W_SIZE = CGWindowGetSize(W);
  CGImageRef img    = CGWindowListCreateImage(
    CGRectMake(0, 0, (int)W_SIZE.height, (int)W_SIZE.width),
    kCGWindowListOptionIncludingWindow,
    WID,
    kCGWindowImageBoundsIgnoreFraming | kCGWindowImageBestResolution
    );

  assert(img != NULL);
  return(img);
}


static void debug_resize(const char *FILE_NAME, char *RESIZE_BY, int RESIZE_VALUE, int ORIGINAL_WIDTH, int ORIGINAL_HEIGHT, int RESIZE_FACTOR, int RESIZE_WIDTH, int RESIZE_HEIGHT){
  char *msg;

  asprintf(&msg, "[debug_resize] FILE_NAME:%s|RESIZE_BY:%s|RESIZE_VALUE:%d|RESIZE_FACTOR:%d|ORIGINAL_WIDTH:%d|ORIGINAL_HEIGHT:%d|ESIZE_WIDTH:%d|RESIZE_HEIGHT:%d",
           FILE_NAME, RESIZE_BY, RESIZE_VALUE, RESIZE_FACTOR, ORIGINAL_WIDTH, ORIGINAL_HEIGHT, RESIZE_WIDTH, RESIZE_HEIGHT
           );
  fprintf(stderr, "%s\n", msg);
  free(msg);
}


bool capture_to_file_image_resize_factor(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_FACTOR){
  CGImageRef img           = capture_window_id_cgimageref(WINDOW_ID);
  size_t     width         = CGImageGetWidth(img);
  size_t     height        = CGImageGetHeight(img);
  int        RESIZE_WIDTH  = width / RESIZE_FACTOR;
  int        RESIZE_HEIGHT = height / RESIZE_FACTOR;

  if (DEBUG_IMAGE_RESIZE) {
    debug_resize(FILE_NAME, "factor", RESIZE_FACTOR, RESIZE_FACTOR, width, height, RESIZE_WIDTH, RESIZE_HEIGHT);
  }
  CGImageRef scaled_img = resize_cgimage(img, RESIZE_WIDTH, RESIZE_HEIGHT);

  assert(cgimage_save_png(scaled_img, FILE_NAME) == true);
  return(fsio_file_exists(FILE_NAME));
}


bool capture_to_file_image_resize_height(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_HEIGHT){
  CGImageRef img           = capture_window_id_cgimageref(WINDOW_ID);
  size_t     width         = CGImageGetWidth(img);
  size_t     height        = CGImageGetHeight(img);
  int        RESIZE_FACTOR = height / RESIZE_HEIGHT;
  int        RESIZE_WIDTH  = width / RESIZE_FACTOR;

  if (DEBUG_IMAGE_RESIZE) {
    debug_resize(FILE_NAME, "height", RESIZE_HEIGHT, RESIZE_FACTOR, width, height, RESIZE_WIDTH, RESIZE_HEIGHT);
  }
  CGImageRef scaled_img = resize_cgimage(img, RESIZE_WIDTH, RESIZE_HEIGHT);

  assert(cgimage_save_png(scaled_img, FILE_NAME) == true);
  return(fsio_file_exists(FILE_NAME));
}


bool capture_to_file_image_resize_width(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_WIDTH){
  CGImageRef img           = capture_window_id_cgimageref(WINDOW_ID);
  size_t     width         = CGImageGetWidth(img);
  size_t     height        = CGImageGetHeight(img);
  int        RESIZE_FACTOR = width / RESIZE_WIDTH;
  int        RESIZE_HEIGHT = height / RESIZE_FACTOR;

  if (DEBUG_IMAGE_RESIZE) {
    debug_resize(FILE_NAME, "width", RESIZE_WIDTH, RESIZE_FACTOR, width, height, RESIZE_WIDTH, RESIZE_HEIGHT);
  }
  CGImageRef scaled_img = resize_cgimage(img, RESIZE_WIDTH, RESIZE_HEIGHT);

  assert(cgimage_save_png(scaled_img, FILE_NAME) == true);
  return(fsio_file_exists(FILE_NAME));
}


bool capture_to_file_image(const int WINDOW_ID, const char *FILE_NAME){
  //MEASURE(measurement_capture_window_id_cgimageref)

  CGImageRef img = capture_window_id_cgimageref(WINDOW_ID);

  //END_MEASURE(measurement_capture_window_id_cgimageref)
  //MEASURE_SUMMARY(measurement_capture_window_id_cgimageref);

  size_t width  = CGImageGetWidth(img);
  size_t height = CGImageGetHeight(img);

  if (DEBUG_IMAGE_RESIZE) {
    debug_resize(FILE_NAME, "none", 0, 1, width, height, width, height);
  }

  assert(cgimage_save_png(img, FILE_NAME) == true);
  return(fsio_file_exists(FILE_NAME));
} /* capture_to_file_image */
///////////////////////////////////////////////////////
