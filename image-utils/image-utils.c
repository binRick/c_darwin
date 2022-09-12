#pragma once
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "stb/stb_image_resize.h"
#include "active-app/active-app.h"
#include "string-utils/string-utils.h"
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
CGImageRef resize_cgimage_factor(CGImageRef imageRef, double resize_factor){
  int cur_width = CGImageGetWidth(imageRef);
  int cur_height = CGImageGetHeight(imageRef);
  int new_width = cur_width * resize_factor/100;
  int new_height = cur_height * resize_factor/100;
  if(IMAGE_UTILS_DEBUG_MODE==true)
    log_info("|%dx%d| |%f| => |%dx%d|",cur_width,cur_height,resize_factor,new_width,new_height);
  return(resize_cgimage(imageRef,new_width,new_height));
}

CGImageRef resize_png_file_factor(FILE *fp, double resize_factor){
  int width = 0,height=0,format=0,req_format=STBI_rgb_alpha;
  unsigned char *pixels = stbi_load_from_file(fp,&width,&height,&format,req_format);
  CGImageRef png_image_ref = rgb_pixels_to_png_cgimage_ref(pixels,width,height);
  return(resize_cgimage_factor(png_image_ref,resize_factor));
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

void * CompressToPNG(int width, int height, const void *rgb, const void *mask, long *outsize){
  CFMutableDataRef      data     = CFDataCreateMutable(kCFAllocatorDefault, 0);
  CGImageDestinationRef dest     = CGImageDestinationCreateWithData(data, kUTTypePNG, 1, NULL);
  CGColorSpaceRef       space    = CGColorSpaceCreateDeviceRGB();
  CGDataProviderRef     provider = CGDataProviderCreateWithData(NULL, rgb, 4 * width * height, NULL);
  const CGFloat         decode[] = { 0, 1, 0, 1, 0, 1 };
  CGImageRef            image;

  if (mask) {
    for (int i = 0; i < width * height; i++) {
      ((char *)rgb)[i * 4] = ((char *)mask)[i];
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

CGImageRef rgb_pixels_to_png_cgimage_ref(unsigned char *rgb_pixels, int width, int height){
  CFMutableDataRef      data     = CFDataCreateMutable(kCFAllocatorDefault, 0);
  CGImageDestinationRef dest     = CGImageDestinationCreateWithData(data, kUTTypePNG, 1, NULL);
  CGColorSpaceRef       space    = CGColorSpaceCreateDeviceRGB();
  CGDataProviderRef     provider = CGDataProviderCreateWithData(NULL, rgb_pixels, 4 * width * height, NULL);
  const CGFloat         decode[] = { 0, 1, 0, 1, 0, 1 };
  CGImageRef            image;
  image = CGImageCreate(width, height, 8, 32, 4 * width, space, kCGImageAlphaNoneSkipFirst, provider, decode, true, kCGRenderingIntentDefault);
  CGImageDestinationAddImage(dest, image, NULL);
  CGImageDestinationFinalize(dest);
  return(image);
}

CGImageRef png_file_to_grayscale_cgimage_ref_resized(FILE *fp, double resize_factor){
  int width = 0,height=0,format=0,req_format=STBI_rgb_alpha;
  unsigned char *pixels = stbi_load_from_file(fp,&width,&height,&format,req_format);
  int new_width = width * resize_factor/100;
  int new_height = height * resize_factor/100;
  unsigned char *resized_rgb_pixels = calloc(new_width*new_height*4,sizeof(unsigned char));
  stbir_resize_uint8(pixels,width,height,0,resized_rgb_pixels,new_width,new_height,0,4);
  CGImageRef png_image_ref = rgb_pixels_to_png_cgimage_ref(resized_rgb_pixels,new_width,new_height);
  return(cgimageref_to_grayscale(png_image_ref));
}

CGImageRef cgimageref_to_grayscale(CGImageRef im){
    CGImageRef grayscaleImage;
    CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericGray);
    CGContextRef bitmapCtx = CGBitmapContextCreate(NULL, CGImageGetWidth(im), CGImageGetHeight(im), 8, 0, colorSpace, kCGImageAlphaNone);
    CGContextDrawImage(bitmapCtx, CGRectMake(0,0,CGImageGetWidth(im), CGImageGetHeight(im)), im);
    grayscaleImage = CGBitmapContextCreateImage(bitmapCtx);
    CFRelease(bitmapCtx);
    CFRelease(colorSpace);
    return grayscaleImage;
}

bool write_cgimage_ref_to_tif_file_path(CGImageRef im, char *tif_file_path){
  log_info("write_cgimage_ref_to_tif_file_path=> %s",tif_file_path);
    CFMutableDictionaryRef options = CFDictionaryCreateMutable(kCFAllocatorDefault, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFMutableDictionaryRef tiffOptions = CFDictionaryCreateMutable(kCFAllocatorDefault, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    int fourInt = 4;
    CFNumberRef fourNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &fourInt);
    CFDictionarySetValue(tiffOptions, kCGImagePropertyTIFFCompression, fourNumber);
    CFRelease(fourNumber);
    CFDictionarySetValue(options, kCGImagePropertyTIFFDictionary, tiffOptions);
    CFRelease(tiffOptions);
    int oneInt = 1;
    CFNumberRef oneNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &oneInt);
    CFDictionarySetValue(options, kCGImagePropertyDepth, oneNumber);
    CFRelease(oneNumber);
    CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfstring_from_cstring(tif_file_path), kCFURLPOSIXPathStyle, false);
    CGImageDestinationRef idst = CGImageDestinationCreateWithURL(url, kUTTypeTIFF, 1, NULL);
    CGImageDestinationAddImage(idst, im, options);
    CGImageDestinationFinalize(idst);
    CFRelease(idst);
    CFRelease(options);
    return(true);
}
