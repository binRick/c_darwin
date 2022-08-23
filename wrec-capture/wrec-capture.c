#pragma once
///////////////////////////////////////////////////////
#include <assert.h>
#include <objc/message.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
///////////////////////////////////////////////////////
#include "bench/bench.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "wrec-capture/wrec-capture.h"
///////////////////////////////////////////////////////
#include "core-utils/core-utils.h"
#include "wrec-utils/wrec-utils.h"
///////////////////////////////////////////////////////
static bool DEBUG_IMAGE_RESIZE         = false;
static bool VERBOSE_DEBUG_IMAGE_RESIZE = false;

///////////////////////////////////////////////////////
static bool cgimage_save_png(const CGImageRef image, const char *filename) {
  bool                  success = false;
  CFStringRef           path;
  CFURLRef              url;
  CGImageDestinationRef destination;
  {
    path        = CFStringCreateWithCString(NULL, filename, kCFStringEncodingUTF8);
    url         = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, 0);
    destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
    CGImageDestinationAddImage(destination, image, nil);
    success = CGImageDestinationFinalize(destination);
  }

  assert(success == true);
  CFRelease(url);
  CFRelease(path);
  CFRelease(destination);
  return(fsio_file_exists(filename));
}

static void CGImageDumpInfo(CGImageRef image) {
  size_t          width              = CGImageGetWidth(image);
  size_t          height             = CGImageGetHeight(image);
  CGColorSpaceRef colorSpace         = CGImageGetColorSpace(image);
  size_t          bytesPerRow        = CGImageGetBytesPerRow(image);
  size_t          bitsPerPixel       = CGImageGetBitsPerPixel(image);
  size_t          bitsPerComponent   = CGImageGetBitsPerComponent(image);
  size_t          componentsPerPixel = bitsPerPixel / bitsPerComponent;
  CGBitmapInfo    bitmapInfo         = CGImageGetBitmapInfo(image);

  printf("==================================================================================================\n");
  printf("CGImageGetWidth: %lu\n", width);
  printf("CGImageGetHeight: %lu\n", height);
  printf("CGColorSpaceModel: %d\n", CGColorSpaceGetModel(colorSpace));
  printf("CGImageGetBytesPerRow: %lu\n", bytesPerRow);
  printf("CGImageGetBitsPerPixel: %lu\n", bitsPerPixel);
  printf("CGImageGetBitsPerComponent: %lu\n", bitsPerComponent);
  printf("components per pixel: %lu\n", componentsPerPixel);
  printf("CGImageGetBitmapInfo: 0x%.8X\n", (unsigned)bitmapInfo);
  printf("\t->kCGBitmapAlphaInfoMask = %s\n", (bitmapInfo & kCGBitmapAlphaInfoMask) ? "YES" : "NO");
  printf("\t->kCGBitmapFloatComponents = %s\n", (bitmapInfo & kCGBitmapFloatComponents) ? "YES" : "NO");
  printf("\t->kCGBitmapByteOrderMask = 0x%.8X\n", (bitmapInfo & kCGBitmapByteOrderMask));
  printf("\t->kCGBitmapByteOrderDefault = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrderDefault) ? "YES" : "NO");
  printf("\t->kCGBitmapByteOrder16Little = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder16Little) ? "YES" : "NO");
  printf("\t->kCGBitmapByteOrder32Little = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder32Little) ? "YES" : "NO");
  printf("\t->kCGBitmapByteOrder16Big = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder16Big) ? "YES" : "NO");
  printf("\t->kCGBitmapByteOrder32Big = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder32Big) ? "YES" : "NO");
  printf("==================================================================================================\n");
} /* CGImageDumpInfo */

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

///////////////////////////////////////////////////////
#define COMMON_RESIZE_AND_SAVE(RESIZE_TYPE, RESIZE_VALUE, RESIZE_HEIGHT) \
  resized_img  = resize_cgimage(img, RESIZE_WIDTH, RESIZE_HEIGHT);       \
  save_started = timestamp();                                            \
  bool ok = cgimage_save_png(resized_img, FILE_NAME);                    \
  save_dur = timestamp() - save_started;                                 \
  assert(ok == true);                                                    \
  if (true == DEBUG_IMAGE_RESIZE) {                                      \
    debug_resize(                                                        \
      WINDOW_ID,                                                         \
      FILE_NAME,                                                         \
      RESIZE_TYPE, RESIZE_VALUE,                                         \
      width, height,                                                     \
      capture_dur, save_dur                                              \
      );                                                                 \
  }                                                                      \
  if (true == VERBOSE_DEBUG_IMAGE_RESIZE) {                              \
    CGImageDumpInfo(resized_img);                                        \
  }

#define COMMON_CAPTURE()                                                                  \
  size_t width = 0, height = 0;                                                           \
  long unsigned capture_dur = -1, save_dur = -1, capture_started = -1, save_started = -1; \
  DEBUG_IMAGE_RESIZE         = (getenv("DEBUG") != NULL) ? true : false;                  \
  VERBOSE_DEBUG_IMAGE_RESIZE = (getenv("VERBOSE_DEBUG") != NULL) ? true : false;          \
  CGImageRef resized_img;                                                                 \
  capture_started = timestamp();                                                          \
  CGImageRef img = capture_window_id_cgimageref(WINDOW_ID);                               \
  capture_dur = timestamp() - capture_started;                                            \
  assert(img != NULL);                                                                    \
  width  = CGImageGetWidth(img);                                                          \
  height = CGImageGetHeight(img);                                                         \
  assert(width > 0);                                                                      \
  assert(height > 0);
#define COMMON_RESIZE_RETURN() \
  return((fsio_file_exists(FILE_NAME) && fsio_file_size(FILE_NAME) > 64));

///////////////////////////////////////////////////////

bool capture_to_file_image_resize_factor(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_FACTOR){
  COMMON_CAPTURE()
  int RESIZE_WIDTH = width / RESIZE_FACTOR;
  int RESIZE_HEIGHT = height / RESIZE_FACTOR;
  COMMON_RESIZE_AND_SAVE(RESIZE_FACTOR, RESIZE_WIDTH, RESIZE_HEIGHT)
  COMMON_RESIZE_RETURN()
}

bool capture_to_file_image_resize_height(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_HEIGHT){
  COMMON_CAPTURE()
  int RESIZE_FACTOR = height / RESIZE_HEIGHT;
  int RESIZE_WIDTH = width / RESIZE_FACTOR;
  COMMON_RESIZE_AND_SAVE(RESIZE_HEIGHT, RESIZE_WIDTH, RESIZE_HEIGHT)
  COMMON_RESIZE_RETURN()
}

bool capture_to_file_image_resize_width(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_WIDTH){
  COMMON_CAPTURE()
  int RESIZE_FACTOR = width / RESIZE_WIDTH;
  int RESIZE_HEIGHT = height / RESIZE_FACTOR;
  COMMON_RESIZE_AND_SAVE(RESIZE_BY_WIDTH, RESIZE_WIDTH, RESIZE_HEIGHT)
  COMMON_RESIZE_RETURN()
}

bool capture_to_file_image(const int WINDOW_ID, const char *FILE_NAME){
  COMMON_CAPTURE()
  int RESIZE_WIDTH = width;
  int RESIZE_HEIGHT = height;
  COMMON_RESIZE_AND_SAVE(RESIZE_BY_NONE, RESIZE_WIDTH, RESIZE_HEIGHT)
  COMMON_RESIZE_RETURN()
}

struct capture_result_t *request_window_capture(struct capture_request_t *capture_request){
  struct capture_result_t *res = calloc(1, sizeof(struct capture_result_t));
  {
    res->data                = calloc(1, sizeof(struct capture_result_data_t));
    res->data->file_path     = NULL;
    res->request             = capture_request;
    res->success             = false;
    res->save_file_success   = false;
    res->capture_dur         = 0;
    res->resize_dur          = 0;
    res->write_file_dur      = 0;
    res->total_dur           = 0;
    res->request_received    = timestamp();
    res->request->debug_mode = (res->request->debug_mode == true)
                      ? true
                      : (getenv("DEBUG") != NULL)
                        ? true
                        : false;
  }

  {
    res->capture_started          = timestamp();
    res->data->captured_image_ref = capture_window_id_cgimageref(res->request->window_id);
    res->data->captured_width     = CGImageGetWidth(res->data->captured_image_ref);
    res->data->captured_height    = CGImageGetHeight(res->data->captured_image_ref);
    res->capture_dur              = timestamp() - res->capture_started;
  }
  {
    if (false) {
      res->data->resized_image_ref = resize_cgimage(
        res->data->captured_image_ref, res->data->resized_width, res->data->resized_height
        );
    }
    res->data->resized_width  = CGImageGetWidth(res->data->resized_image_ref);
    res->data->resized_height = CGImageGetWidth(res->data->resized_image_ref);
    res->resize_dur           = timestamp() - res->resize_started;
  }
  {
    switch (res->request->resize_type) {
    case RESIZE_BY_FACTOR:
      res->resize_started          = timestamp();
      res->request->width          = res->data->captured_width / res->request->resize_factor;
      res->request->height         = res->data->captured_height / res->request->resize_factor;
      res->data->resized_image_ref = resize_cgimage(res->data->captured_image_ref,
                                                    res->request->width, res->request->height
                                                    );
      res->data->resized_width  = CGImageGetWidth(res->data->resized_image_ref);
      res->data->resized_height = CGImageGetWidth(res->data->resized_image_ref);
      break;

    case RESIZE_BY_NONE:
      res->data->resized_image_ref = res->data->captured_image_ref;
    case RESIZE_BY_WIDTH:
    case RESIZE_BY_HEIGHT:
    default:
      goto RETURN_FAILURE;
      break;
    }
  }
  {
    switch (res->request->mode) {
    case CAPTURE_REQUEST_MODE_WRITE_FILE_RETURN_RESULT:
    case CAPTURE_REQUEST_MODE_WRITE_FILE_CALLBACK_RESULT:
      res->write_file_started = timestamp();
      res->save_file_success  = cgimage_save_png(res->data->resized_image_ref, res->request->file_path);
      res->write_file_dur     = timestamp() - res->write_file_started;
      if (res->save_file_success != true || fsio_file_exists(res->request->file_path) != true) {
        goto RETURN_FAILURE;
      }
      res->data->file_path = res->request->file_path;
      res->data->file_size = fsio_file_size(res->data->file_path);
      res->success         = true;
      if (res->request->mode == CAPTURE_REQUEST_MODE_CALLBACK_RESULT) {
        goto CALLBACK_RESULT;
      }
      break;

    case CAPTURE_REQUEST_MODE_RETURN_RESULT:
      if (1 != 1) {
        goto RETURN_FAILURE;
      }

      res->success = true;
      break;

    case CAPTURE_REQUEST_MODE_CALLBACK_RESULT:
      if (1 != 1) {
        goto RETURN_FAILURE;
      }
      res->success = true;
      break;

    default:
      goto RETURN_FAILURE;
      break;
    }
  }

  res->total_dur = timestamp() - res->request_received;
  return(res);

CALLBACK_RESULT:
  res->total_dur = res->request_received - timestamp();
  res->request->callback_function(res);
  return(res);

RETURN_FAILURE:
  res->success = false;
  return(res);
} /* request_capture */

///////////////////////////////////////////////////////
