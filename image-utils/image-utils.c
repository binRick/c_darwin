#pragma once
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define QOI_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
/////////////////////////
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_img/src/img.h"
#include "c_vector/vector/vector.h"
#include "c_workqueue/include/workqueue.h"
#include "core-utils/core-utils.h"
#include "fancy-progress/src/fancy-progress.h"
#include "frameworks/frameworks.h"
#include "image-utils/image-utils.h"
#include "libfort/lib/fort.h"
#include "libimagequant/libimagequant.h"
#include "lodepng.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process-utils/process-utils.h"
#include "process/process.h"
#include "qoi/qoi.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
#include "submodules/log.h/log.h"
#include "system-utils/system-utils.h"
#include "timestamp/timestamp.h"
#include "timg-utils/timg-utils.h"
static bool IMAGE_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__image_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_IMAGE_UTILS") != NULL) {
    log_debug("Enabling Image Utils Debug Mode");
    IMAGE_UTILS_DEBUG_MODE = true;
  }
}

char * convert_png_to_grayscale(char *png_file, size_t resize_factor){
  char *grayscale_file;

  asprintf(&grayscale_file, "%s-grayscale-resized-%lu-grayscale.tif", png_file, resize_factor);
  if (IMAGE_UTILS_DEBUG_MODE) {
    log_info("Converting %s to %s resized by %lu%%", png_file, grayscale_file, resize_factor);
  }
  FILE       *input_png_file = fopen(png_file, "rb");
  CGImageRef png_gs          = png_file_to_grayscale_cgimage_ref_resized(input_png_file, resize_factor);

  assert(write_cgimage_ref_to_tif_file_path(png_gs, grayscale_file) == true);
  if (IMAGE_UTILS_DEBUG_MODE) {
    log_info("Converted to %s grayscale from %s PNG", bytes_to_string(fsio_file_size(grayscale_file)), bytes_to_string(fsio_file_size(png_file)));
  }
  return(grayscale_file);
}

bool save_cgref_to_image_type_file(enum image_type_id_t image_type, CGImageRef image, char *image_file){
  unsigned long started = timestamp();
  bool          success = false; CFStringRef path; CFURLRef url; CGImageDestinationRef destination;

  path        = CFStringCreateWithCString(NULL, image_file, kCFStringEncodingUTF8);
  url         = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, 0);
  destination = CGImageDestinationCreateWithURL(url, image_types[image_type].get_format(), 1, NULL);
  CGImageDestinationAddImage(destination, image, nil);
  success = CGImageDestinationFinalize(destination);
  CFRelease(url); CFRelease(path); CFRelease(destination);
  if (IMAGE_UTILS_DEBUG_MODE) {
    log_debug("Saved %s to %s file %s in %s",
              bytes_to_string(fsio_file_size(image_file)), image_types[image_type].name, image_file, milliseconds_to_string(timestamp() - started)
              );
  }
  return((success == true) && fsio_file_exists(image_file));
}
enum image_conversion_test_type_t {
  IMAGE_CONVERSION_JPG,
  IMAGE_CONVERSION_PNG,
  IMAGE_CONVERSION_BMP,
  IMAGE_CONVERSIONS_QTY,
};
int image_conversion_compressions[] = { 8 };
int image_conversion_qualities[]    = { 100 };

struct image_conversion_test_t {
  char          *file;
  size_t        size;
  int           (*fxn)(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
  int           width, height, compression, quality;
  unsigned long started, dur;
  char          *ext;
};
static struct image_conversion_test_t IC[] = {
  [IMAGE_CONVERSION_PNG] = { .fxn = stbi_write_png, .size = 0, .width = 0, .height = 0, .compression = 0, .quality = 0, .started = 0, .dur = 0, .file = NULL, .ext = "png", .quality = 90, .compression = 3, },
  [IMAGE_CONVERSION_JPG] = { .fxn = stbi_write_jpg, .size = 0, .width = 0, .height = 0, .compression = 0, .quality = 0, .started = 0, .dur = 0, .file = NULL, .ext = "jpg", .quality = 90, .compression = 3, },
  [IMAGE_CONVERSION_BMP] = { .fxn = stbi_write_bmp, .size = 0, .width = 0, .height = 0, .compression = 0, .quality = 0, .started = 0, .dur = 0, .file = NULL, .ext = "bmp", .quality = 90, .compression = 3, },
};

bool image_conversions(char *file){
  fancy_progress_start();
  size_t ROUNDS = 100;
  srand(time(NULL));

  static const char *strings[] = {
    "Lorem ipsum dolor sit amet",
    "Consectetur adipiscing elit",
    "Vivamus faucibus sagittis dui, tincidunt rhoncus mi",
    "Fringilla sollicitudin. Donec eget sagittis",
    "Quam, vitae fringilla nisl",
    "Donec dolor justo, hendrerit sed accumsan id, sodales",
    "Eu odio",
    "Nunc vehicula hendrerit risus, vel condimentum dui rutrum sed.",
    "Quisque metus enim, pellentesque nec nibh sit amet.",
    "Commodo molestie diam."
  };
  char              *s;
  int               height, width, channels;
  unsigned long     started = timestamp();
  unsigned char     *rgb    = stbi_load(file, &width, &height, &channels, STBI_rgb_alpha);
  assert(rgb != NULL);
  asprintf(&s, "Loaded %dx%d %d Channels from %s File in %s", width, height, channels, bytes_to_string(fsio_file_size(file)), milliseconds_to_string(timestamp() - started));
  int progress = 5;
  fancy_progress_step(progress);
  log_info("%s", s);
  for (size_t i = 0; i < IMAGE_CONVERSIONS_QTY; i++) {
    for (size_t c = 0; c < sizeof(image_conversion_compressions) / sizeof(image_conversion_compressions[0]); c++) {
      for (size_t q = 0; q < sizeof(image_conversion_qualities) / sizeof(image_conversion_qualities[0]); q++) {
        char *s;
        IC[i].compression = image_conversion_compressions[c];
        IC[i].quality     = image_conversion_qualities[q];
        //fancy_progress_step(progress + 1);
        asprintf(&(IC[i].file), "/tmp/output-%d-%dx%d-comp-%d-qual-%d.%s", getpid(), width, height, IC[i].compression, IC[i].quality, IC[i].ext);
        log_info("%s", IC[i].file);
        IC[i].started = timestamp();
        if (IC[i].fxn(IC[i].file, width, height, 0, rgb, 0) != 0) {
          log_error("Failed to write %s", IC[i].file);
          continue;
        }
        IC[i].dur  = timestamp() - IC[i].started;
        IC[i].size = fsio_file_size(IC[i].file);
        asprintf(&s, "Finished %s in %s with %s conversion | %s",
                 IC[i].ext, milliseconds_to_string(IC[i].dur), bytes_to_string(IC[i].size),
                 IC[i].file
                 );
        log_info("%s", s);
      }
    }
  }
  fancy_progress_step(100);

  sleep(1);
  fancy_progress_stop();

  return(true);
} /* image_conversions */

static void read_image_format_file(const char *path, unsigned char *buf, int len, int offset) {
  int fd = open(path, O_RDONLY); assert(fd >= 0);   assert(pread(fd, buf, len, offset) >= 0); close(fd);
}
struct image_type_t image_types[IMAGE_TYPES_QTY + 1] = {
  [IMAGE_TYPE_PNG] =  {
    .file_extension             = "png",                         .name = "PNG",
    .get_format                 = ^ CFStringRef (void){ return(kUTTypePNG);                                         },
    .validate_header            = ^ bool (unsigned char *image_buf){ return(('P' == image_buf[1] && 'N' == image_buf[2] && 'G' == image_buf[3]) ? true : false); },
    .read_file_header           = ^ unsigned char *(char *image_path){ unsigned char *buf = calloc(8, sizeof(unsigned char)); read_image_format_file(image_path, buf, 8, 16); return(buf); },
    .get_dimensions_from_header = ^ bool (unsigned char *header, int *width, int *height){
      *width  = read_32bytes_big_endian_image_buffer(header);
      *height = read_32bytes_big_endian_image_buffer(header + 4);
      return((*width > 0 && *height > 0) ? true : false);
    },
  },
  [IMAGE_TYPE_GIF] =  {
    .file_extension             = "gif",
    .name                       = "GIF",
    .get_format                 = ^ CFStringRef (void){ return(kUTTypeGIF);                                         },
    .validate_header            = ^ bool (unsigned char *image_buf){ return(('G' == image_buf[0] && 'I' == image_buf[1] && 'F' == image_buf[2]) ? true : false); },
    .read_file_header           = ^ unsigned char *(char *image_path){ unsigned char *buf = calloc(4, sizeof(unsigned char)); read_image_format_file(image_path, buf, 4, 6); return(buf); },
    .get_dimensions_from_header = ^ bool (unsigned char *buf,int *width,                                                            int *height){
      *width  = read_16bytes_little_endian_image_buffer(buf);
      *height = read_16bytes_little_endian_image_buffer(buf + 2);
      return((*width > 0 && *height > 0) ? true : false);
    },
  },
  [IMAGE_TYPE_TIFF] = {
    .file_extension             = "tiff",
    .name                       = "TIFF",
    .get_format                 = ^ CFStringRef (void){ return(kUTTypeTIFF);                                        },
    .validate_header            = ^ bool (unsigned char *image_buf){ return((image_buf != NULL) ? true : false);    },
    .get_dimensions_from_buffer = ^ bool (unsigned char *buf,size_t len,  int *width, int *height){
      int compression           = 0;
      stbi_load_from_memory(buf, len, width, height, &compression, 4);
      return(true);
    },
  },
  [IMAGE_TYPE_JPEG] = {
    .file_extension             = "jpeg",
    .name                       = "JPEG",
    .get_format                 = ^ CFStringRef (void){ return(kUTTypeJPEG);                                        },
    .validate_header            = ^ bool (unsigned char *image_buf){ return((0xff == image_buf[0] && 0xd8 == image_buf[1]) ? true : false); },
    .get_dimensions_from_buffer = ^ bool (unsigned char *buf,size_t len,  int *width, int *height){
      int compression           = 0;
      stbi_load_from_memory(buf, len, width, height, &compression, 4);
      return(true);
    },
  },
  [IMAGE_TYPE_BMP] =  {
    .file_extension             = "bmp",
    .name                       = "BMP",
    .get_format                 = ^ CFStringRef (void){ return(kUTTypeBMP);                                         },
    .validate_header            = ^ bool (unsigned char *image_buf){ return((image_buf != NULL) ? true : false);    },
    .get_dimensions_from_buffer = ^ bool (unsigned char *buf,size_t len,  int *width, int *height){
      int compression           = 0;
      stbi_load_from_memory(buf, len, width, height, &compression, 4);
      return(true);
    },
  },
  [IMAGE_TYPE_QOI] =  {
    .file_extension             = "qoi",
    .name                       = "QOI",
    .get_format                 = ^ CFStringRef (void){ return(NULL);                                               },
    .validate_header            = ^ bool (unsigned char *image_buf){ return((image_buf != NULL) ? true : false);    },
    .get_dimensions_from_buffer = ^ bool (unsigned char *buf,size_t len,  int *width, int *height){
      qoi_desc desc;
      unsigned char *pixels     = qoi_decode(buf,            len,         &desc,      4);
      *width  = desc.width;
      *height = desc.height;
      if (pixels)     {
        free(pixels);
      }
      return(true);
    },
  },
};

unsigned char *save_cgref_to_image_type_memory(enum image_type_id_t image_type, CGImageRef image, size_t *len){
  CFMutableDataRef      image_data = CFDataCreateMutable(kCFAllocatorDefault, 0);
  CGImageDestinationRef dataDest   = CGImageDestinationCreateWithData(image_data, image_types[image_type].get_format(), 1, NULL);

  CGImageDestinationAddImage(dataDest, image, NULL);
  CGImageDestinationFinalize(dataDest);
  *len = CFDataGetLength(image_data);
  if (*len < 0) {
    log_error("Failed to save %s data to memory", image_types[image_type].name);
    return(NULL);
  }
  unsigned char *buf = calloc(1, *len);

  CFDataGetBytes(image_data, CFRangeMake(0, *len), buf);
  return(buf);
}

unsigned char *save_cgref_to_tiff_memory(CGImageRef image, size_t *len){
  return(save_cgref_to_image_type_memory(IMAGE_TYPE_TIFF, image, len));
}

unsigned char *save_cgref_to_bmp_memory(CGImageRef image, size_t *len){
  return(save_cgref_to_image_type_memory(IMAGE_TYPE_BMP, image, len));
}

unsigned char *save_cgref_to_gif_memory(CGImageRef image, size_t *len){
  return(save_cgref_to_image_type_memory(IMAGE_TYPE_GIF, image, len));
}

unsigned char *save_cgref_to_jpeg_memory(CGImageRef image, size_t *len){
  return(save_cgref_to_image_type_memory(IMAGE_TYPE_JPEG, image, len));
}

unsigned char *save_cgref_to_png_memory(CGImageRef image, size_t *len){
  return(save_cgref_to_image_type_memory(IMAGE_TYPE_PNG, image, len));
}

bool save_cgref_to_png_file(CGImageRef image, char *image_file) {
  return(save_cgref_to_image_type_file(IMAGE_TYPE_PNG, image, image_file));
}

bool save_cgref_to_bmp_file(CGImageRef image, char *image_file) {
  return(save_cgref_to_image_type_file(IMAGE_TYPE_BMP, image, image_file));
}

bool save_cgref_to_gif_file(CGImageRef image, char *image_file) {
  return(save_cgref_to_image_type_file(IMAGE_TYPE_GIF, image, image_file));
}

bool save_cgref_to_jpeg_file(CGImageRef image, char *image_file) {
  return(save_cgref_to_image_type_file(IMAGE_TYPE_JPEG, image, image_file));
}

bool save_cgref_to_tiff_file(CGImageRef image, char *image_file) {
  return(save_cgref_to_image_type_file(IMAGE_TYPE_TIFF, image, image_file));
}

bool save_cgref_to_qoi_file(CGImageRef image, char *image_file) {
  size_t        qoi_len     = 0;
  unsigned char *qoi_pixels = cgref_to_qoi_memory(image, &qoi_len);

  if (qoi_len < 1 || !qoi_pixels) {
    return(false);
  }
  bool ok = fsio_write_binary_file(image_file, qoi_pixels, qoi_len);

  free(qoi_pixels);
  return((ok && (size_t)fsio_file_size(image_file) == (size_t)qoi_len) ? true : false);
}

unsigned char *cgref_to_qoi_memory(CGImageRef image_ref, size_t *qoi_len){
  int           w = CGImageGetWidth(image_ref), h = CGImageGetHeight(image_ref);
  size_t        rgb_len     = 0;
  unsigned char *rgb_pixels = cgimage_ref_to_rgb_pixels(image_ref, &rgb_len);

  if (rgb_len < 1 || !rgb_pixels) {
    return(NULL);
  }
  void *qoi_pixels = qoi_encode(rgb_pixels, &(qoi_desc){
    .width      = w,
    .height     = h,
    .channels   = 4,
    .colorspace = QOI_SRGB
  }, qoi_len);

  if (IMAGE_UTILS_DEBUG_MODE) {
    log_info("encoded %dx%d %s RGB to qoi %s",
             w, h,
             bytes_to_string(rgb_len),
             bytes_to_string(*qoi_len)

             );
  }
  if (rgb_pixels) {
    free(rgb_pixels);
  }
  return(qoi_pixels);
}

///////////////////////////////////////////////////////////////////////////////
CGImageRef resize_cgimage_factor(CGImageRef imageRef, double resize_factor){
  return(resize_cgimage(imageRef, CGImageGetWidth(imageRef) * resize_factor / 100, CGImageGetHeight(imageRef) * resize_factor / 100));
}

CGImageRef png_fp_to_cgimage(FILE *fp){
  int           width = 0, height = 0, format = 0, req_format = STBI_rgb_alpha;
  unsigned char *pixels = stbi_load_from_file(fp, &width, &height, &format, req_format);
  CGImageRef    img_ref = rgb_pixels_to_png_cgimage_ref(pixels, width, height);

  free(pixels);
  return(img_ref);
}

CGImageRef resize_png_file_factor(FILE *fp, double resize_factor){
  int           width = 0, height = 0, format = 0, req_format = STBI_rgb_alpha;
  unsigned char *pixels       = stbi_load_from_file(fp, &width, &height, &format, req_format);
  CGImageRef    png_image_ref = rgb_pixels_to_png_cgimage_ref(pixels, width, height);

  return(resize_cgimage_factor(png_image_ref, resize_factor));
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

unsigned char *rgb_pixels_to_png_pixels(int width, int height, const void *rgb, const void *mask, long *outsize){
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

  long          size = CFDataGetLength(data);
  unsigned char *buf = calloc(size, sizeof(unsigned char));

  CFDataGetBytes(data, CFRangeMake(0, size), buf);
  *outsize = size;

  CGDataProviderRelease(provider);
  CGColorSpaceRelease(space);
  CGImageRelease(image);
  CFRelease(dest);
  CFRelease(data);

  return(buf);
}

unsigned char *cgimage_ref_to_rgb_pixels(CGImageRef image_ref, size_t *len){
  CFDataRef data_ref = CGDataProviderCopyData(CGImageGetDataProvider(image_ref));

  *len = CFDataGetLength(data_ref);
  unsigned char *buf = calloc(1, *len);

  CFDataGetBytes(data_ref, CFRangeMake(0, *len), buf);
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
  int           width = 0, height = 0, format = 0, req_format = STBI_rgb_alpha;
  unsigned char *pixels             = stbi_load_from_file(fp, &width, &height, &format, req_format);
  int           new_width           = width * resize_factor / 100;
  int           new_height          = height * resize_factor / 100;
  unsigned char *resized_rgb_pixels = calloc(new_width * new_height * 4, sizeof(unsigned char));

  stbir_resize_uint8(pixels, width, height, 0, resized_rgb_pixels, new_width, new_height, 0, 4);
  CGImageRef png_image_ref = rgb_pixels_to_png_cgimage_ref(resized_rgb_pixels, new_width, new_height);

  return(cgimageref_to_grayscale(png_image_ref));
}

CGImageRef cgimageref_to_grayscale(CGImageRef im){
  CGImageRef      grayscaleImage;
  CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericGray);
  CGContextRef    bitmapCtx  = CGBitmapContextCreate(NULL, CGImageGetWidth(im), CGImageGetHeight(im), 8, 0, colorSpace, kCGImageAlphaNone);

  CGContextDrawImage(bitmapCtx, CGRectMake(0, 0, CGImageGetWidth(im), CGImageGetHeight(im)), im);
  grayscaleImage = CGBitmapContextCreateImage(bitmapCtx);
  CFRelease(bitmapCtx);
  CFRelease(colorSpace);
  return(grayscaleImage);
}

bool write_cgimage_ref_to_tif_file_path(CGImageRef im, char *tif_file_path){
  CFMutableDictionaryRef options     = CFDictionaryCreateMutable(kCFAllocatorDefault, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  CFMutableDictionaryRef tiffOptions = CFDictionaryCreateMutable(kCFAllocatorDefault, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  int                    fourInt     = 4;
  CFNumberRef            fourNumber  = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &fourInt);

  CFDictionarySetValue(tiffOptions, kCGImagePropertyTIFFCompression, fourNumber);
  CFRelease(fourNumber);
  CFDictionarySetValue(options, kCGImagePropertyTIFFDictionary, tiffOptions);
  CFRelease(tiffOptions);
  int         oneInt    = 1;
  CFNumberRef oneNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &oneInt);

  CFDictionarySetValue(options, kCGImagePropertyDepth, oneNumber);
  CFRelease(oneNumber);
  CFURLRef              url  = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfstring_from_cstring(tif_file_path), kCFURLPOSIXPathStyle, false);
  CGImageDestinationRef idst = CGImageDestinationCreateWithURL(url, kUTTypeTIFF, 1, NULL);

  CGImageDestinationAddImage(idst, im, options);
  CGImageDestinationFinalize(idst);
  CFRelease(idst);
  CFRelease(options);
  return(true);
}

unsigned char *imagequant_encode_rgb_pixels_to_png_buffer(unsigned char *raw_rgba_pixels, int width, int height, int min_quality, int max_quality, size_t *len){
  liq_attr   *handle      = liq_attr_create();
  liq_image  *input_image = liq_image_create_rgba(handle, raw_rgba_pixels, width, height, 0);
  liq_result *quantization_result;

  liq_set_quality(handle, min_quality, max_quality);
  if (liq_image_quantize(input_image, handle, &quantization_result) != LIQ_OK) {
    fprintf(stderr, "Quantization failed\n");
    goto fail;
  }
  *len = width * height;
  unsigned char *raw_8bit_pixels = malloc(*len);

  liq_set_dithering_level(quantization_result, 1.0);
  liq_write_remapped_image(quantization_result, input_image, raw_8bit_pixels, *len);
  const liq_palette *palette = liq_get_palette(quantization_result);

  LodePNGState      state;

  lodepng_state_init(&state);
  state.info_raw.colortype       = LCT_PALETTE;
  state.info_raw.bitdepth        = 8;
  state.info_png.color.colortype = LCT_PALETTE;
  state.info_png.color.bitdepth  = 8;

  for (unsigned int i = 0; i < palette->count; i++) {
    lodepng_palette_add(&state.info_png.color, palette->entries[i].r, palette->entries[i].g, palette->entries[i].b, palette->entries[i].a);
    lodepng_palette_add(&state.info_raw, palette->entries[i].r, palette->entries[i].g, palette->entries[i].b, palette->entries[i].a);
  }

  unsigned char *output_file_data;
  unsigned int  out_status = lodepng_encode(&output_file_data, len, raw_8bit_pixels, width, height, &state);

  return(output_file_data);

fail:
  *len = 0;
  return(NULL);
}

int imagequant_encode_rgb_pixels_to_png_file(unsigned char *raw_rgba_pixels, int width, int height, char *png_file, int min_quality, int max_quality){
  size_t     len          = 0;
  liq_attr   *handle      = liq_attr_create();
  liq_image  *input_image = liq_image_create_rgba(handle, raw_rgba_pixels, width, height, 0);
  liq_result *quantization_result;

  liq_set_quality(handle, min_quality, max_quality);
  if (liq_image_quantize(input_image, handle, &quantization_result) != LIQ_OK) {
    fprintf(stderr, "Quantization failed\n");
    goto fail;
  }
  len = width * height;
  unsigned char *raw_8bit_pixels = malloc(len);

  liq_set_dithering_level(quantization_result, 1.0);
  liq_write_remapped_image(quantization_result, input_image, raw_8bit_pixels, len);
  const liq_palette *palette = liq_get_palette(quantization_result);

  LodePNGState      state;

  lodepng_state_init(&state);
  state.info_raw.colortype       = LCT_PALETTE;
  state.info_raw.bitdepth        = 8;
  state.info_png.color.colortype = LCT_PALETTE;
  state.info_png.color.bitdepth  = 8;

  for (unsigned int i = 0; i < palette->count; i++) {
    lodepng_palette_add(&state.info_png.color, palette->entries[i].r, palette->entries[i].g, palette->entries[i].b, palette->entries[i].a);
    lodepng_palette_add(&state.info_raw, palette->entries[i].r, palette->entries[i].g, palette->entries[i].b, palette->entries[i].a);
  }
  unsigned char *output_file_data;
  size_t        output_file_size;
  unsigned int  out_status = lodepng_encode(&output_file_data, &output_file_size, raw_8bit_pixels, width, height, &state);

  if (out_status) {
    fprintf(stderr, "Can't encode image: %s\n", lodepng_error_text(out_status));
    goto fail;
  }

  FILE *fp = fopen(png_file, "wb");

  if (!fp) {
    fprintf(stderr, "Unable to write to %s\n", png_file);
    goto fail;
  }
  fwrite(output_file_data, 1, output_file_size, fp);
  fclose(fp);

  liq_result_destroy(quantization_result);
  liq_image_destroy(input_image);
  liq_attr_destroy(handle);

  free(raw_8bit_pixels);
  lodepng_state_cleanup(&state);
  return(EXIT_SUCCESS);

fail:
  return(EXIT_FAILURE);
} /* imagequant_encode_rgb_pixels_to_png_file */
#include "window-info/window-info.h"
