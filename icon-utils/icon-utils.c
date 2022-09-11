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
#include "log/log.h"
#include "ms/ms.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "string-utils/string-utils.h"
#include "timestamp/timestamp.h"
struct icns_t {
  uint8_t bytes;
  size_t  size;
};
struct app_icon_size_t {
  char   *name;
  size_t pixels;
  int    value;
};
static const icns_type_t            iconset_types[] = {
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
static const struct app_icon_size_t app_icon_sizes[] = {
  { "16 Pixels",  16,  1, },
  { "32 Pixels",  32,  3, },
  { "128 Pixels", 128, 5, },
  { "256 Pixels", 256, 7, },
  { "512 Pixels", 512, 9, },
};
////////////////////////////////////////////
static const size_t                 app_icon_sizes_qty          = ((sizeof(app_icon_sizes) / sizeof(app_icon_sizes[0])));
static const int                    DEFAULT_APP_ICON_SIZE_INDEX = 3;
static const struct app_icon_size_t *DEFAULT_APP_ICON_SIZE      = &(app_icon_sizes[DEFAULT_APP_ICON_SIZE_INDEX]);
static bool                         icon_utils_DEBUG_MODE       = false;
static bool save_cfdataref_to_icns_file(CFDataRef data_ref, FILE *fp);
static bool write_icns_file_to_png(FILE *fp, char *png_file_path, size_t icon_size);
static icns_family_t *get_icns_file_info(FILE *fp);
static struct icns_t *cfdataref_to_icns_bytes(CFDataRef data_ref);
static int get_icon_size_pixels(size_t icon_size);
static int get_icon_size_value(size_t icon_size);
static struct app_icon_size_t *get_icon_size(size_t icon_size);
static icns_type_t get_icon_size_type(size_t icon_size);
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__icon_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_icon_utils") != NULL) {
    icon_utils_DEBUG_MODE = true;
  }
}
static struct app_icon_size_t *get_icon_size(size_t icon_size){
  for (size_t i = 0; i < app_icon_sizes_qty; i++) {
    if (app_icon_sizes[i].pixels == icon_size) {
      return(&(app_icon_sizes[i]));
    }
  }
  return(NULL);
}

static int get_icon_size_value(size_t icon_size){
  struct app_icon_size_t *is = get_icon_size(icon_size);

  if (is) {
    return(is->value);
  }
  DEFAULT_APP_ICON_SIZE->value;
  return(0);
}

static int get_icon_size_pixels(size_t icon_size){
  struct app_icon_size_t *is = get_icon_size(icon_size);

  if (is) {
    return(is->pixels);
  }

  return(0);
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

static bool save_cfdataref_to_icns_file(CFDataRef data_ref, FILE *fp){
  size_t              length  = CFDataGetLength(data_ref);
  const unsigned char *buffer = CFDataGetBytePtr(data_ref);
  size_t              written = fwrite(buffer, 1, length, fp);

  if (buffer) {
    free(buffer);
  }

  fflush(fp);
  fclose(fp);
  assert(written == length);
  return(true);
}

static struct icns_t *cfdataref_to_icns_bytes(CFDataRef data_ref){
  struct   icns_t *data = calloc(1, sizeof(struct icns_t));

  data->bytes = CFDataGetBytePtr(data_ref);
  data->size  = CFDataGetLength(data_ref);
  return(data);
}

static icns_family_t *get_icns_file_info(FILE *fp){
  icns_family_t *iconFamily = NULL;

  assert(icns_read_family_from_file(fp, &iconFamily) == 0);
  return(iconFamily);
}

static icns_type_t get_icon_size_type(size_t icon_size){
  return((icns_type_t)(iconset_types[get_icon_size_value(icon_size)]));
}

static bool write_icns_file_to_png(FILE *fp, char *png_file_path, size_t icon_size){
  icns_family_t *iconFamily = get_icns_file_info(fp);

  fclose(fp);
  icns_image_t   *iconImage   = calloc(1, sizeof(icns_image_t));
  icns_element_t *iconElement = calloc(1, sizeof(icns_element_t));

  assert(icns_get_element_from_family(iconFamily, get_icon_size_type(icon_size), &iconElement) == 0);
  assert(icns_get_image_from_element(iconElement, iconImage) == 0);
  if (fsio_file_exists(png_file_path) == true) {
    fsio_remove(png_file_path);
  }
  stbi_write_png(
    png_file_path,
    iconImage->imageWidth,
    iconImage->imageHeight,
    iconImage->imageChannels,
    iconImage->imageData,
    0
    );
  return(fsio_file_exists(png_file_path));
}

bool write_app_icon_to_png(char *app_path, char *png_file_path, size_t icon_size){
  CFDataRef     data_ref   = get_icon_from_path(app_path);
  struct icns_t *icns_data = cfdataref_to_icns_bytes(data_ref);
  void          *buf       = calloc(1, icns_data->size);
  FILE          *fp        = fmemopen(buf, icns_data->size, "ab");

  save_cfdataref_to_icns_file(data_ref, fp);
  fclose(fp);
  fp = fmemopen(buf, icns_data->size, "r");
  return(write_icns_file_to_png(fp, png_file_path, icon_size));
}

bool write_app_icon_to_icns(char *app_path, char *icns_file_path){
  CFDataRef cfdata = get_icon_from_path(app_path);
  FILE      *fp    = fopen(icns_file_path, "ab");
  bool      ok     = save_cfdataref_to_icns_file(cfdata, fp);

  fclose(fp);
  return(ok);
}

bool app_icon_size_is_valid(size_t icon_size){
  return((get_icon_size(icon_size) != NULL) ? true : false);
}

char *get_icon_size_name(size_t icon_size){
  struct app_icon_size_t *is = get_icon_size(icon_size);

  if (is) {
    return(is->name);
  }
  return(0);
}

#endif
