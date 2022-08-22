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
#include "capture/capture.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
///////////////////////////////////////////////////////
#include "core-utils/core-utils.h"
#include "wrec-utils/wrec-utils.h"
///////////////////////////////////////////////////////
static bool DEBUG_IMAGE_RESIZE         = false;
static bool VERBOSE_DEBUG_IMAGE_RESIZE = false;

extern int CGSMainConnectionID(void);
extern CGError CGSGetConnectionPSN(int cid, ProcessSerialNumber *psn);
extern CGError CGSSetWindowAlpha(int cid, uint32_t wid, float alpha);
extern CGError CGSSetWindowListAlpha(int cid, const uint32_t *window_list, int window_count, float alpha, float duration)
;
extern CGError CGSSetWindowLevelForGroup(int cid, uint32_t wid, int level);
extern OSStatus CGSMoveWindowWithGroup(const int cid, const uint32_t wid, CGPoint *point);
extern CGError CGSReassociateWindowsSpacesByGeometry(int cid, CFArrayRef window_list);
extern CGError CGSGetWindowOwner(int cid, uint32_t wid, int *window_cid);
extern CGError CGSSetWindowTags(int cid, uint32_t wid, const int tags[2], size_t maxTagSize);
extern CGError CGSClearWindowTags(int cid, uint32_t wid, const int tags[2], size_t maxTagSize);
extern CGError CGSGetWindowBounds(int cid, uint32_t wid, CGRect *frame);
extern CGError CGSGetWindowTransform(int cid, uint32_t wid, CGAffineTransform *t);
extern CGError CGSSetWindowTransform(int cid, uint32_t wid, CGAffineTransform t);
extern void CGSManagedDisplaySetCurrentSpace(int cid, CFStringRef display_ref, uint64_t spid);
extern uint64_t CGSManagedDisplayGetCurrentSpace(int cid, CFStringRef display_ref);
extern CFArrayRef CGSCopyManagedDisplaySpaces(const int cid);
extern CFStringRef CGSCopyManagedDisplayForSpace(const int cid, uint64_t spid);
extern void CGSShowSpaces(int cid, CFArrayRef spaces);
extern void CGSHideSpaces(int cid, CFArrayRef spaces);

typedef struct {
  const char   *text;
  unsigned int length;
} Token;

static float token_to_float(Token token){
  float result = 0.0f;
  char  buffer[token.length + 1];

  memcpy(buffer, token.text, token.length);
  buffer[token.length] = '\0';
  sscanf(buffer, "%f", &result);
  return(result);
}

static Token get_token(const char **message){
  Token token;

  token.text = *message;
  while (**message && !isspace(**message)) {
    ++(*message);
  }
  token.length = *message - token.text;

  if (isspace(**message)) {
    ++(*message);
  } else {
    // NOTE(koekeishiya): don't go past the null-terminator
  }

  return(token);
}

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

CGImageRef capture_window_id_cgimageref(const int WINDOW_ID){
  assert(WINDOW_ID > 0);
  CFDictionaryRef W = window_id_to_window(WINDOW_ID);
  assert(W != NULL);
  int             WID = (int)CFDictionaryGetInt(W, kCGWindowNumber);
  assert(WID > 0);
  assert(WID == WINDOW_ID);
  CGSize W_SIZE = CGWindowGetSize(W);

  CGRect frame       = {};
  int    _connection = CGSMainConnectionID();
  CGSGetWindowBounds(_connection, WID, &frame);
  if (DEBUG_IMAGE_RESIZE) {
    printf("size  : %dx%d\n", (int)W_SIZE.height, (int)W_SIZE.width);
    printf("frame : %dx%d\n", (int)frame.size.width, (int)frame.size.height);
    printf("origin: %dx%d\n", (int)frame.origin.x, (int)frame.origin.y);
  }

  CGImageRef img = CGWindowListCreateImage(
    CGRectMake(frame.origin.x, frame.origin.y, (int)W_SIZE.width - frame.origin.x, (int)W_SIZE.height - frame.origin.y),
    kCGWindowListOptionIncludingWindow,
    WID,
    kCGWindowImageBoundsIgnoreFraming | kCGWindowImageBestResolution
    );
  assert(img != NULL);
  CGContextRelease(W);
  return(img);
}

static void debug_resize(int WINDOW_ID,
                         char *FILE_NAME,
                         int RESIZE_TYPE, int RESIZE_VALUE,
                         int ORIGINAL_WIDTH, int ORIGINAL_HEIGHT,
                         long unsigned CAPTURE_DURATION_MS, long unsigned SAVE_DURATION_MS)                                                                  {
  char *msg;

  asprintf(&msg,
           "<%d> [debug_resize] "
           "\n\t|RESIZE_TYPE:%d"
           "|RESIZE_VALUE:%s"
           "\n\t|WINDOW_ID:%d"
           "|FILE_NAME:%s"
           "|FILE_SIZE:%ld"
           "\n\t|ORIGINAL_WIDTH:%d|ORIGINAL_HEIGHT:%d"
           "\n\t|CAPTURE_DUR_MS:%ld|SAVE_DURATION_MS:%ld",
           getpid(),
           RESIZE_TYPE,
           resize_type_name(RESIZE_VALUE),
           WINDOW_ID,
           FILE_NAME,
           fsio_file_size(FILE_NAME),
           ORIGINAL_WIDTH, ORIGINAL_HEIGHT,
           CAPTURE_DURATION_MS, SAVE_DURATION_MS
           );

  fprintf(stderr, "%s\n", msg);

  free(msg);
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

char *resize_type_name(const int RESIZE_TYPE){
  char *RESIZE_TYPE_NAME = "UNKNOWN";

  switch (RESIZE_TYPE) {
  case RESIZE_BY_WIDTH:  RESIZE_TYPE_NAME  = "WIDTH"; break;
  case RESIZE_BY_HEIGHT:  RESIZE_TYPE_NAME = "HEIGHT"; break;
  case RESIZE_BY_FACTOR:  RESIZE_TYPE_NAME = "FACTOR"; break;
  case RESIZE_BY_NONE:  RESIZE_TYPE_NAME   = "NONE"; break;
  }
  return(RESIZE_TYPE_NAME);
}
///////////////////////////////////////////////////////
