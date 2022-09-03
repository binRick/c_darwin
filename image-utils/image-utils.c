#pragma once
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_img/src/img.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils-extern.h"
#include "core-utils/core-utils.h"
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
