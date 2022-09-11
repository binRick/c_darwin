#pragma once
#ifndef ICON_UTILS_C
#define ICON_UTILS_C
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "/usr/local/Cellar/libpng/1.6.37/include/libpng16/png.h"
#include "icns/src/icns.h"
#if PNG_LIBPNG_VER >= 10209
#define PNG2ICNS_EXPAND_GRAY    1
#endif
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
static const int                    APP_ICON_INITIAL_INDEX = 1;
static const struct app_icon_size_t app_icon_sizes[]       = {
  { "16 Pixels",   16,   APP_ICON_INITIAL_INDEX,      },
  { "32 Pixels",   32,   APP_ICON_INITIAL_INDEX + 2,  },
  { "128 Pixels",  128,  APP_ICON_INITIAL_INDEX + 4,  },
  { "256 Pixels",  256,  APP_ICON_INITIAL_INDEX + 6,  },
  { "512 Pixels",  512,  APP_ICON_INITIAL_INDEX + 8,  },
  { "1024 Pixels", 1024, APP_ICON_INITIAL_INDEX + 10, },
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
static int read_png(FILE *fp, png_bytepp buffer, int32_t *bpp, int32_t *width, int32_t *height);
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

bool write_app_icon_from_png(char *app_path, char *png_file_path){
  log_info("writing png %s to app %s", png_file_path, app_path);
  int req_channels = STBI_rgb_alpha, width, height, channels, components_qty, width1, height1;
  int hdr_ok = stbi_info(png_file_path, &width1, &height1, &components_qty);
  log_info("hdr ok:%d|%dx%d,components_qty:%d", hdr_ok, width1, height1, components_qty);
  assert(hdr_ok == 1);
  int is_16bit = stbi_is_16_bit(png_file_path);
  log_info("is 16bit:%d", is_16bit);

  FILE          *fp         = fopen(png_file_path, "rb");
  unsigned char *rgb_pixels = stbi_load(png_file_path, &width, &height, &channels, req_channels);
  fclose(fp);
  log_info("read %s: %dx%d|channels:%d", png_file_path, width, height, channels);

  int           icnsErr = ICNS_STATUS_OK;
  icns_family_t *iconFamily;
  char          *icnsfile_path = "/tmp/abc.icns";
  FILE          *icnsfile      = fopen(icnsfile_path, "wb+");
  FILE          *pngfile       = fopen(png_file_path, "rb");

  png_bytep     png_buffer;
  int           png_width, png_height, png_bpp;
  if (!read_png(pngfile, &png_buffer, &png_bpp, &png_width, &png_height)) {
    log_error("Failed to read PNG file");
    fclose(pngfile);

    return(FALSE);
  }
  log_info("png bpp:%d", png_bpp);

  icns_create_family(&iconFamily);
  icns_image_t     icnsImage;
  icns_image_t     icnsMask;
  icns_type_t      iconType;
  icns_type_t      maskType;
  icns_icon_info_t iconInfo;
  icns_element_t   *iconElement   = NULL;
  icns_element_t   *maskElement   = NULL;
  char             iconStr[5]     = { 0, 0, 0, 0, 0 };
  char             maskStr[5]     = { 0, 0, 0, 0, 0 };
  int              iconDataOffset = 0;
  int              maskDataOffset = 0;
  char             isHiDPI        = 1;

  icnsImage.imageWidth      = width;
  icnsImage.imageHeight     = height;
  icnsImage.imageChannels   = components_qty;
  icnsImage.imagePixelDepth = 8;
  icnsImage.imageDataSize   = width * height * components_qty;
  icnsImage.imageData       = rgb_pixels;

  iconInfo.isImage        = 1;
  iconInfo.iconWidth      = icnsImage.imageWidth;
  iconInfo.iconHeight     = icnsImage.imageHeight;
  iconInfo.iconBitDepth   = png_bpp;
  iconInfo.iconChannels   = (png_bpp == 32 ? 4 : 1);
  iconInfo.iconPixelDepth = png_bpp / iconInfo.iconChannels;

  iconType = icns_get_type_from_image_info(iconInfo);

  icns_init_image_for_type(iconType, &icnsImage);
  icnsImage.imageWidth      = width;
  icnsImage.imageHeight     = height;
  icnsImage.imageChannels   = components_qty;
  icnsImage.imagePixelDepth = 8;
  icnsImage.imageDataSize   = width * height * components_qty;
  icnsImage.imageData       = rgb_pixels;
  log_info(
    "icns data size: %lu|size: %dx%d|channels: %d|pixelDepth:%d",
    (size_t)(icnsImage.imageDataSize),
    icnsImage.imageWidth, icnsImage.imageHeight,
    icnsImage.imageChannels,
    icnsImage.imagePixelDepth
    );

  if (png_bpp != 32) {
    log_error("Bit depth %d unsupported in '%s'", png_bpp, png_file_path);
    free(png_buffer);

    return(FALSE);
  }
  icns_set_print_errors(0);

  icns_type_str(iconType, iconStr);
  log_info("icon type:%s|maskType:%s|bpp:%d|size:%dx%d|", iconStr, maskStr, png_bpp, png_width, png_height);

  if (iconType == ICNS_NULL_TYPE) {
    log_error("Unable to determine icon type: PNG file '%s' is %dx%d", png_file_path, png_width, png_height);
    free(png_buffer);

    return(FALSE);
  }
  maskType = icns_get_mask_type_for_icon_type(iconType);
  icns_type_str(maskType, maskStr);
  if (maskType != ICNS_NULL_TYPE) {
    log_info("Using icns type '%s', mask '%s' for '%s'", iconStr, maskStr, png_file_path);
  }else {
    log_info("Using icns type '%s' (ARGB) for '%s'", iconStr, png_file_path);
  }

  icnsErr = icns_new_element_from_image(&icnsImage, iconType, &iconElement);
  if (iconElement != NULL) {
    if (icnsErr == ICNS_STATUS_OK) {
      log_info("icon element size %d|icon family size:%d|icon fam type:%d|", iconElement->elementSize, iconFamily->resourceSize, iconFamily->resourceType);
      icns_set_element_in_family(&iconFamily, iconElement);
    }else{
      log_error("icns_set_element_in_family error");
      exit(1);
    }
    free(iconElement);
  }else{
    log_error("icns_new_element_from_image error");
    exit(1);
  }
/*
 * if( (iconType != ICNS_1024x1024_32BIT_ARGB_DATA) && (iconType != ICNS_512x512_32BIT_ARGB_DATA) && (iconType != ICNS_256x256_32BIT_ARGB_DATA) )
 *  {
 *    icns_init_image_for_type(maskType, &icnsMask);
 *
 *    iconDataOffset = 0;
 *    maskDataOffset = 0;
 *
 *    while ((iconDataOffset < icnsImage.imageDataSize) && (maskDataOffset < icnsMask.imageDataSize))
 *    {
 *      icnsMask.imageData[maskDataOffset] = icnsImage.imageData[iconDataOffset+3];
 *      iconDataOffset += 4;
 *      maskDataOffset += 1;
 *    }
 *
 *    icnsErr = icns_new_element_from_mask(&icnsMask, maskType, &maskElement);
 *
 *    if (maskElement != NULL)
 *    {
 *      if (icnsErr == ICNS_STATUS_OK)
 *      {
 *        icns_set_element_in_family(iconFamily, maskElement);
 *      }
 *      free(maskElement);
 *    }
 *
 *    icns_free_image(&icnsMask);
 *  }
 */

  if (icns_write_family_to_file(icnsfile, iconFamily) != ICNS_STATUS_OK) {
    fprintf(stderr, "Failed to write icns file\n");      fclose(icnsfile);

    exit(1);
  }

  fclose(icnsfile);

  printf("Saved icns file to %s\n", icnsfile_path);

  if (iconFamily != NULL) {
    free(iconFamily);
  }
  /*
   *
   * icns_set_print_errors(1);
   *
   * icnsErr = icns_new_element_from_image(&icnsImage, iconType, &iconElement);
   * if (iconElement != NULL)
   * {
   * if (icnsErr == ICNS_STATUS_OK)
   * {
   * icns_set_element_in_family(iconFamily, iconElement);
   * }else{
   * log_error("Failed to set element in family");
   * free(iconElement);
   * exit(1);
   * }
   * }else{
   * log_error("Failed to create new element from image");
   * exit(1);
   * }
   *
   * if(maskType != ICNS_NULL_TYPE){
   * icns_init_image_for_type(maskType, &icnsMask);
   *
   * iconDataOffset = 0;
   * maskDataOffset = 0;
   *
   * while (((size_t)iconDataOffset < (size_t)icnsImage.imageDataSize) && ((size_t)maskDataOffset < (size_t)icnsMask.imageDataSize))
   * {
   * icnsMask.imageData[maskDataOffset] = icnsImage.imageData[iconDataOffset+3];
   * iconDataOffset += 4;
   * maskDataOffset += 1;
   * }
   *
   * icnsErr = icns_new_element_from_mask(&icnsMask, maskType, &maskElement);
   *
   * if (maskElement != NULL)
   * {
   * if (icnsErr == ICNS_STATUS_OK)
   * {
   * icns_set_element_in_family(iconFamily, maskElement);
   * }
   * free(maskElement);
   * }
   *
   * icns_free_image(&icnsMask);
   * }
   *
   * free(png_buffer);
   *
   *
   *
   * if (icns_write_family_to_file(icnsfile, iconFamily) != ICNS_STATUS_OK)
   * {
   * log_error("Failed to write icns file");
   * fclose(icnsfile);
   * exit(1);
   * }
   *
   * fclose(icnsfile);
   *
   * log_info("Saved icns file to %s",icnsfile_path);
   */
/*
 *  icnsImage.imageWidth = width;
 *  icnsImage.imageHeight = height;
 *  icnsImage.imageChannels = 4;
 *  icnsImage.imagePixelDepth = 8;
 *  icnsImage.imageDataSize = width * height * 4;
 *  icnsImage.imageData = rgb_pixels;
 *
 *
 *  iconInfo.isImage = 1;
 *  iconInfo.iconWidth = icnsImage.imageWidth;
 *  iconInfo.iconHeight = icnsImage.imageHeight;
 *  iconInfo.iconBitDepth = 32;
 *  iconInfo.iconChannels = 4;
 *  iconInfo.iconPixelDepth = 8;
 *
 *  iconType = icns_get_type_from_image_info(iconInfo);
 *  maskType = icns_get_mask_type_for_icon_type(iconType);
 *
 *  icns_type_str(iconType,iconStr);
 *  icns_type_str(maskType,maskStr);
 *
 *  ("iconType:%s|%d",iconStr,iconType);
 *  log_info("maskType:%s|%d",maskStr,maskType);
 *
 *
 *
 *  icns_get_element_from_family(iconFamily, iconType, &iconElement);
 *  icns_new_element_from_image(&icnsImage, iconType, &iconElement);
 *  icns_set_element_in_family(iconFamily, iconElement);
 *  icns_free_image(&icnsMask);
 *
 *
 *  icns_write_family_to_file(icnsfile, iconFamily);
 *
 *  fclose(icnsfile);
 *
 *
 *  stbi_image_free(rgb_pixels);
 */
  /*
   * iconInfo.iconChannels = (bpp == 32 ? 4 : 1);
   * iconInfo.iconPixelDepth = bpp / iconInfo.iconChannels;
   *
   * iconType = icns_get_type_from_image_info(iconInfo);
   * maskType = icns_get_mask_type_for_icon_type(iconType);
   *
   * icns_type_str(iconType,iconStr);
   * icns_type_str(maskType,maskStr);
   *
   */

  return(true);
} /* write_app_icon_from_png */

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

static int read_png(FILE *fp, png_bytepp buffer, int32_t *bpp, int32_t *width, int32_t *height){
  png_structp png_ptr;
  png_infop   info;
  png_uint_32 w;
  png_uint_32 h;
  png_bytep   *rows;

  int         bit_depth;
  int32_t     color_type;

  int         row;
  int         rowsize;

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return(FALSE);
  }

  info = png_create_info_struct(png_ptr);
  if (info == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return(FALSE);
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info, NULL);
    return(FALSE);
  }

  png_init_io(png_ptr, fp);

  png_read_info(png_ptr, info);
  png_get_IHDR(png_ptr, info, &w, &h, &bit_depth, &color_type, NULL, NULL, NULL);

  switch (color_type) {
  case PNG_COLOR_TYPE_GRAY:
#ifdef PNG2ICNS_EXPAND_GRAY
    png_set_expand_gray_1_2_4_to_8(png_ptr);
#else
    png_set_gray_1_2_4_to_8(png_ptr);
#endif

    if (bit_depth == 16) {
      png_set_strip_16(png_ptr);
      bit_depth = 8;
    }

    png_set_gray_to_rgb(png_ptr);
    png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    break;

  case PNG_COLOR_TYPE_GRAY_ALPHA:
#ifdef PNG2ICNS_EXPAND_GRAY
    png_set_expand_gray_1_2_4_to_8(png_ptr);
#else
    png_set_gray_1_2_4_to_8(png_ptr);
#endif

    if (bit_depth == 16) {
      png_set_strip_16(png_ptr);
      bit_depth = 8;
    }

    png_set_gray_to_rgb(png_ptr);
    break;

  case PNG_COLOR_TYPE_PALETTE:
    png_set_palette_to_rgb(png_ptr);

    if (png_get_valid(png_ptr, info, PNG_INFO_tRNS)) {
      png_set_tRNS_to_alpha(png_ptr);
    }else{
      png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    }
    break;

  case PNG_COLOR_TYPE_RGB:
    if (bit_depth == 16) {
      png_set_strip_16(png_ptr);
      bit_depth = 8;
    }

    png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    break;

  case PNG_COLOR_TYPE_RGB_ALPHA:
    if (bit_depth == 16) {
      png_set_strip_16(png_ptr);
      bit_depth = 8;
    }

    break;
  } /* switch */

  *width  = w;
  *height = h;
  *bpp    = bit_depth * 4;

  png_read_update_info(png_ptr, info);

  rowsize = png_get_rowbytes(png_ptr, info);
  rows    = malloc(sizeof(png_bytep) * h);
  *buffer = malloc(rowsize * h + 8);

  rows[0] = *buffer;
  for (row = 1; row < h; row++) {
    rows[row] = rows[row - 1] + rowsize;
  }

  png_read_image(png_ptr, rows);
  png_destroy_read_struct(&png_ptr, &info, NULL);

  free(rows);

  return(TRUE);
} /* read_png */

#endif
