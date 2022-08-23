////////////////////////////////////////////////////
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
////////////////////////////////////////////////////
#define QOI_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "qoi/qoi.h"
#include "stb/stb_image.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "wildcardcmp/wildcardcmp.h"
////////////////////////////////////////////////////
#include "wrec-capture/wrec-capture.h"
#include "wrec-common/wrec-common.h"
////////////////////////////////////////////////////
#define STR_ENDS_WITH(S, E)    (strcmp(S + strlen(S) - (sizeof(E) - 1), E) == 0)

struct image_dimensions_t *get_png_file_dimensions(char *path){
  return(get_png_dimensions(path));
}
struct loaded_png_file_t *load_png_file_pixels(char *png_file){
  unsigned long             started           = timestamp();
  struct image_dimensions_t *image_dimensions = get_png_dimensions(png_file);
  struct loaded_png_file_t  *res              = calloc(1, sizeof(struct loaded_png_file_t));

  res->path           = png_file;
  res->stb_req_format = STBI_rgb_alpha;
  res->size           = fsio_file_size(res->path);
  log_debug("c_img> %s: %dx%d | %s", res->path, image_dimensions->width, image_dimensions->height, bytes_to_string(res->size));
  FILE *png_fp = fopen(res->path, "r");

  res->pixels = stbi_load_from_file(png_fp, &res->image_dimensions.width, &res->image_dimensions.height, &res->stb_format, res->stb_req_format);
  fclose(png_fp);
  res->dur = timestamp() - started;
  log_debug("stb_load_file> %s: %dx%d | stb format: %d |%lums|", res->path, res->image_dimensions.width, res->image_dimensions.height, res->stb_format, res->dur);
  return(res);
}

int qoi_decode_file(char *qoi_file){
  unsigned long started = timestamp();
  size_t        size    = fsio_file_size(qoi_file);
  void          *pixels = NULL;
  qoi_desc      desc;

  int           req_colorspace = 4;

  pixels = qoi_read(qoi_file, &desc, req_colorspace);

  if (pixels == NULL) {
    printf("Couldn't load/decode %s\n", qoi_file);
    return(1);
  }

  unsigned long dur = timestamp() - started;

  log_info("Decoded %s %dx%d %d channel %d colorspace qoi file %s in %s",
           bytes_to_string(size),
           desc.width, desc.height,
           desc.channels, desc.colorspace,
           qoi_file,
           milliseconds_to_string(dur)
           );
  return(0);
}

void *qoi_decode_from_memory(const void *pixels, int qty){
  unsigned long started         = timestamp();
  void          *decoded_pixels = NULL;
  qoi_desc      desc;

  decoded_pixels = qoi_decode(pixels, qty, &desc, 4);
  if (!decoded_pixels) {
    log_error("Couldn't decode %d pixels", qty);
    return(NULL);
  }

  unsigned long dur = timestamp() - started;

  log_debug("Decoded %s bytes to %dx%d %d channel RGB in %s",
            bytes_to_string(qty),
            desc.width, desc.height, desc.channels,
            milliseconds_to_string(dur)
            );
  return(decoded_pixels);
}

void *qoi_encode_to_memory(const void *pixels, int width, int height){
  unsigned long started       = timestamp();
  int           encoded_bytes = 0;

  qoi_desc desc = {
    .width      = width,
    .height     = height,
    .channels   = 4,
    .colorspace = QOI_SRGB,
  };
  void     *qoi_pixels = qoi_encode(pixels, &desc, &encoded_bytes);

  if (!pixels) {
    log_error("Couldn't encode %dx%d pixels", width, height);
    return(NULL);
  }

  unsigned long dur = timestamp() - started;

  log_debug("Encoded %s bytes to memory in %s", bytes_to_string(encoded_bytes), milliseconds_to_string(dur));
  return(qoi_pixels);
}

int qoi_encode_file(char *qoi_file, void *pixels, int width, int height){
  unsigned long started = timestamp();
  int           encoded = 0, channels = 4;

  log_debug("Encoding pixels to qoi file %s", qoi_file);
  encoded = qoi_write(qoi_file, pixels, &(qoi_desc){
    .width      = width,
    .height     = height,
    .channels   = channels,
    .colorspace = QOI_SRGB
  });

  if (!encoded) {
    printf("Couldn't write/encode %s\n", qoi_file);
    return(1);
  }
  unsigned long dur  = timestamp() - started;
  size_t        size = fsio_file_size(qoi_file);

  log_info("Encoded %s file in %s", bytes_to_string(size), milliseconds_to_string(dur));

  free(pixels);
  return(0);
}
////////////////////////////////////////////////////
