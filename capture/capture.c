#pragma once
#define DEBUG_STDOUT               true
#define STDOUT_READ_BUFFER_SIZE    1024 * 2
#define BYTES_PER_COMPONENT        1
#define BITS_PER_COMPONENT         8
#define COMPONENTS_PER_PIXEL       4
///////////////////////////////////////////////////////
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
///////////////////////////////////////////////////////
#include "c_fsio/include/fsio.h"
#include "c_fsio/include/fsio.h"
#include "capture/capture.h"
struct img_data {
  size_t  width;
  size_t  height;
  uint8_t *rgba;
};


static void __cgimage_save_png(const CGImageRef image, const char *filename) {
  CFStringRef path;
  CFURLRef    url;

  path = CFStringCreateWithCString(NULL, filename, kCFStringEncodingUTF8);
  url  = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, 0);
  CFRelease(path);

  CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);

  CGImageDestinationAddImage(destination, image, nil);
  bool success = CGImageDestinationFinalize(destination);

  if (!success) {
    fprintf(stderr, "Failed to write image to %s", filename);
  }

  CFRelease(url);
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

//    PixelBytesArrayDump( CGImageCopyRawData(image, 0), width, height, bytesPerRow, componentsPerPixel);

  printf("\n");
} /* CGImageDumpInfo */


///////////////////////////////////////////////////////
unsigned char *capture_to_file_image_data(const int WINDOW_ID){
  unsigned char   *pixelData = 0;
  CFDictionaryRef W          = window_id_to_window(WINDOW_ID);
  int             WID        = (int)CFDictionaryGetInt(W, kCGWindowNumber);

  assert(WID == WINDOW_ID);
  CGSize     W_SIZE = CGWindowGetSize(W);

  CGImageRef img = CGWindowListCreateImage(
    CGRectMake(0, 0, (int)W_SIZE.height, (int)W_SIZE.width),
    kCGWindowListOptionIncludingWindow,
    WID,
    kCGWindowImageBestResolution
    );


  CGImageDumpInfo(img, true);

  size_t            width  = CGImageGetWidth(img);
  size_t            height = CGImageGetHeight(img);

  CGDataProviderRef dataProvider = CGImageGetDataProvider(img);

  CGImageRef        png = CGImageCreateWithPNGDataProvider(dataProvider,
                                                           NULL,
                                                           false,
                                                           kCGRenderingIntentDefault);

  CFDataRef data          = CGDataProviderCopyData(dataProvider);
  size_t    rawDataLength = (size_t)CFDataGetLength(data);

  pixelData = CFDataGetBytePtr(data);


//  PixelBytesArrayDump( CGImageCopyRawData(image, 0), width, height, bytesPerRow, componentsPerPixel);

  size_t l = CFDataGetLength(data);

  fprintf(stdout, "l:%lu\n", l);

  fprintf(stdout, "captured image of %lux%lu, %lu bytes\n", width, height, rawDataLength);
  char *bf = "/tmp/bf.png";
  bool ok  = fsio_write_binary_file(bf, pixelData, rawDataLength);

  fprintf(stdout, "wrote ok:%d\n", ok);

  CFRelease(dataProvider);
  CFRelease(data);
  return(pixelData);
} /* capture_to_file_image_data */


bool capture_to_file_image(const int WINDOW_ID, const char *FILE_NAME){
  CFDictionaryRef W   = window_id_to_window(WINDOW_ID);
  int             WID = (int)CFDictionaryGetInt(W, kCGWindowNumber);

  assert(WID == WINDOW_ID);
  CGSize     W_SIZE = CGWindowGetSize(W);
  CGImageRef img    = CGWindowListCreateImage(
    CGRectMake(0, 0, (int)W_SIZE.height, (int)W_SIZE.width),
    kCGWindowListOptionIncludingWindow,
    WID,
    kCGWindowImageBestResolution
    );
  CFStringRef           type   = CFSTR("public.png");
  CFStringRef           file   = CFStringCreateWithCString(NULL, FILE_NAME, kCFStringEncodingMacRoman);
  CFURLRef              urlRef = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, file, kCFURLPOSIXPathStyle, false);
  CGImageDestinationRef idst   = CGImageDestinationCreateWithURL(urlRef, type, 1, NULL);

  CGImageDestinationAddImage(idst, img, NULL);
  CGImageDestinationFinalize(idst);
  return(fsio_file_exists(FILE_NAME));
}
///////////////////////////////////////////////////////
