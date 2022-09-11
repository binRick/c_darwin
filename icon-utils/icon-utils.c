#pragma once
#ifndef ICON_UTILS_C
#define ICON_UTILS_C
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
////////////////////////////////////////////
#include "icon-utils/icon-utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "icns/src/icns.h"
#include "log/log.h"
#include "ms/ms.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "string-utils/string-utils.h"
#include "timestamp/timestamp.h"

////////////////////////////////////////////
static bool icon_utils_DEBUG_MODE = false;
static bool save_cfdataref_to_icns_file(CFDataRef data_ref, char *png_file_path);
static bool write_icns_file_to_png(char *icns_file_path, char *png_file_path);
static icns_family_t *get_icns_file_info(char *icns_file_path);
///////////////////////////////////////////////////////////////////////
static const icns_type_t iconset_types[] = {
  ICNS_16x16_32BIT_DATA,
  ICNS_16x16_2X_32BIT_ARGB_DATA,
  ICNS_32x32_32BIT_DATA,
  ICNS_32x32_2X_32BIT_ARGB_DATA,
  ICNS_128X128_32BIT_DATA,
  ICNS_128x128_2X_32BIT_ARGB_DATA,
  ICNS_256x256_32BIT_ARGB_DATA,
  ICNS_256x256_2X_32BIT_ARGB_DATA,
  ICNS_512x512_32BIT_ARGB_DATA,
  ICNS_512x512_2X_32BIT_ARGB_DATA,
  ICNS_NULL_TYPE
};
static void __attribute__((constructor)) __constructor__icon_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_icon_utils") != NULL) {
    log_debug("Enabling icon-utils Debug Mode");
    icon_utils_DEBUG_MODE = true;
  }
}

CFDataRef copyIconDataForURL(CFURLRef URL){
  CFDataRef data = NULL;

  if (URL) {
    FSRef ref;
    if (CFURLGetFSRef(URL, &ref)) {
      IconRef  icon = NULL;
      SInt16   label_noOneCares;
      OSStatus err = GetIconRefFromFileInfo(&ref,
                                            /*inFileNameLength*/ 0U, /*inFileName*/ NULL,
                                            kFSCatInfoNone, /*inCatalogInfo*/ NULL,
                                            kIconServicesNoBadgeFlag | kIconServicesUpdateIfNeededFlag,
                                            &icon,
                                            &label_noOneCares);
      if (err == noErr) {
        IconFamilyHandle fam = NULL;
        err = IconRefToIconFamily(icon, kSelectorAllAvailableData, &fam);
        if (err != noErr) {
        } else {
          HLock((Handle)fam);
          data = CFDataCreate(kCFAllocatorDefault, (const UInt8 *)*(Handle)fam, GetHandleSize((Handle)fam));
          HUnlock((Handle)fam);
          DisposeHandle((Handle)fam);
        }
        ReleaseIconRef(icon);
      }
    }
  }
  return(data);
}

CFDataRef copyIconDataForPath(CFStringRef path) {
  CFDataRef data = NULL;
  CFURLRef  URL  = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, path, kCFURLPOSIXPathStyle, /*isDirectory*/ false);

  if (URL) {
    data = copyIconDataForURL(URL);
    CFRelease(URL);
  }
  return(data);
}

CFDataRef get_icon_from_path(char *path) {
  return(copyIconDataForPath(cfstring_from_cstring(path)));
}

void get_icon_data_from_path(char *path) {
  CFDataRef cfdata = get_icon_from_path(path);
/*
 *
 *
 * CFDataRef new_data_ref = CGDataProviderCopyData(CGImageGetDataProvider(new_image_ref));
 *
 *
 * CGDataProviderRef imgDataProvider = CGDataProviderCreateWithCFData(cfdata);
 * if(imgDataProvider == NULL){
 * log_error("invalid provider!");
 * exit(1);
 * }
 * log_info("working with cfdata of size %lu, %dx%d",length,image_width,image_height);
 * CGImageRef image = CGImageCreateWithPNGDataProvider(imgDataProvider, NULL, true, kCGRenderingIntentDefault);
 * int pp = CGImageGetBitsPerPixel(image);
 * log_info("pp:%d",pp);
 */
/*CFDataRef imgData = (CFDataRef)CFArrayGetValueAtIndex(cfdata,0);
 * if(imgData == NULL){
 * log_error("invalid provider!");
 * exit(1);
 * }
 * CGImageRef image1 = CGImageCreateWithPNGDataProvider(imgDataProvider, NULL, true, kCGRenderingIntentDefault);
 * size_t bpp = CGImageGetBytesPerRow(image1);
 * image_width = CGImageGetWidth(image1);
 * image_height = CGImageGetHeight(image1);
 * log_info("got image1 of size %lu, %dx%d",bpp,image_width,image_height);
 * exit(0);
 */
/*
 * log_info("pp:%d",pp);
 *
 *      void *buf = malloc(size);
 * memcpy(buf, CFDataGetBytePtr(cfdata), size);
 * int x, y, channels;
 *
 * unsigned char *img = stbi_load_from_memory(buf,size,&x,&y,&channels,4);
 * log_info("loaded png from memory, x:%d,y:%d,channels:%d",x,y,channels);
 * exit(0);
 *
 * CGColorSpaceRef colorspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
 * CGBitmapInfo    bitmapInfo = CGImageGetBitmapInfo(cfdata);
 * // CGDataProviderRef   provider = CGDataProviderCreateWithCFData(cfdata);
 *
 *
 *
 * CGImageRef image = CGImageCreate(image_width, image_height, 8, 32, image_width * 4, colorspace,
 *                          bitmapInfo | kCGBitmapByteOrder32Little,
 *                          provider, NULL, FALSE, kCGRenderingIntentDefault);
 *
 * image_width = CGImageGetWidth(image);
 * image_height = CGImageGetHeight(image);
 *
 * log_info("path:%s|len:%lu|size:%dx%d",
 *    path, (size_t)length,
 *    image_width,image_height
 *    );
 */
}

bool save_app_icon_to_icns_file(char *app_path, char *icns_file_path){
  CFDataRef cfdata = get_icon_from_path(app_path);

  return(save_cfdataref_to_icns_file(cfdata, icns_file_path));
}

static bool save_cfdataref_to_icns_file(CFDataRef data_ref, char *icns_file_path){
  size_t              length  = CFDataGetLength(data_ref);
  const unsigned char *buffer = CFDataGetBytePtr(data_ref);

  fsio_write_binary_file(icns_file_path, buffer, length);
  if (buffer) {
    free(buffer);
  }
  get_icns_file_info(icns_file_path);
  return(true);
}

static icns_family_t *get_icns_file_info(char *icns_file_path){
  icns_family_t *iconFamily = NULL;
  FILE          *inFile     = fopen(icns_file_path, "r");

  assert(icns_read_family_from_file(inFile, &iconFamily) == 0);
  fclose(inFile);
  return(iconFamily);
}

bool write_app_icon_to_png(char *app_path, char *png_file_path){
  char *icns_path = "/tmp/a.icns";

  assert(save_app_icon_to_icns_file(app_path, icns_path) == true);
  return(write_icns_file_to_png(icns_path, png_file_path));
}

static bool write_icns_file_to_png(char *icns_file_path, char *png_file_path){
  icns_family_t *iconFamily = get_icns_file_info(icns_file_path);

  if (png_file_path == NULL) {
    struct StringFNStrings icns_split = stringfn_split(icns_file_path, '.');
    char                   *png_file_path;
    asprintf(&png_file_path, "%s.png", strdup(icns_split.strings[0]));
    stringfn_release_strings_struct(icns_split);
  }
  log_info("writing png to %s", png_file_path);
  icns_image_t   *iconImage   = calloc(1, sizeof(icns_image_t));
  icns_element_t *iconElement = calloc(1, sizeof(icns_element_t));

  assert(icns_get_element_from_family(iconFamily, iconset_types[9], &iconElement) == 0);
  log_info("got icon element %d", iconElement->elementSize);
  assert(icns_get_image_from_element(iconElement, iconImage) == 0);
  log_info("got image %dx%d %s, %d", iconImage->imageWidth, iconImage->imageWidth, bytes_to_string(iconImage->imageDataSize), iconImage->imagePixelDepth);
  stbi_write_png(png_file_path, iconImage->imageWidth, iconImage->imageHeight, iconImage->imageChannels, iconImage->imageData, 0);
  log_info("wrote png %s", png_file_path);
  return(fsio_file_exists(png_file_path));
}

#endif
