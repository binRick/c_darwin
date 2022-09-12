#pragma once
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_img/src/img.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "frameworks/frameworks.h"
#include "image-utils/image-utils.h"
#include "libfort/lib/fort.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process-utils/process-utils.h"
#include "process/process.h"
#include "space-utils/space-utils.h"
#include "submodules/log.h/log.h"
#include "system-utils/system-utils.h"
#include "timestamp/timestamp.h"
static bool IMAGE_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__image_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_IMAGE_UTILS") != NULL) {
    log_debug("Enabling Image Utils Debug Mode");
    IMAGE_UTILS_DEBUG_MODE = true;
  }
}

///////////////////////////////////////////////////////////////////////////////
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

void * CompressToPNG(int width, int height, const void *rgb, const void *mask, long *outsize){
  CFMutableDataRef      data     = CFDataCreateMutable(kCFAllocatorDefault, 0);
  CGImageDestinationRef dest     = CGImageDestinationCreateWithData(data, kUTTypePNG, 1, NULL);
  CGColorSpaceRef       space    = CGColorSpaceCreateDeviceRGB();
  CGDataProviderRef     provider = CGDataProviderCreateWithData(NULL, rgb, 4 * width * height, NULL);
  const CGFloat         decode[] = { 0, 1, 0, 1, 0, 1 };
  CGImageRef            image;

  if (mask) {
    for (int i = 0; i < width * height; i++) {
      ((char *)rgb)[i * 4] = ((char *)mask)[i];                 // ignoring const; should allocate new memory...
    }
    image = CGImageCreate(width, height, 8, 32, 4 * width, space, kCGImageAlphaFirst, provider, decode, true, kCGRenderingIntentDefault);
  }else {
    image = CGImageCreate(width, height, 8, 32, 4 * width, space, kCGImageAlphaNoneSkipFirst, provider, decode, true, kCGRenderingIntentDefault);
  }

  CGImageDestinationAddImage(dest, image, NULL);
  CGImageDestinationFinalize(dest);

  long size = CFDataGetLength(data);
  void *buf = malloc(size);

  CFDataGetBytes(data, CFRangeMake(0, size), buf);
  *outsize = size;

  CGDataProviderRelease(provider);
  CGColorSpaceRelease(space);
  CGImageRelease(image);
  CFRelease(dest);
  CFRelease(data);

  return(buf);
}
