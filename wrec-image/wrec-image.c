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
  log_info("[c_img] Parsed %s File %s (%dx%d)",
           bytes_to_string(res->size),
           res->path,
           image_dimensions->width, image_dimensions->height
           );
  FILE *png_fp = fopen(res->path, "r");

  res->rgb_pixels = stbi_load_from_file(png_fp, &res->image_dimensions.width, &res->image_dimensions.height, &res->stb_format, res->stb_req_format);
  fclose(png_fp);
  res->dur = timestamp() - started;
  log_info("[STB] Loaded PNG %s (%dx%d) in %s",
           res->path,
           res->image_dimensions.width, res->image_dimensions.height,
           milliseconds_to_string(res->dur)
           );
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

  log_info("[QOI] Decoded %s %dx%d %d channel %d colorspace from file %s in %s",
           bytes_to_string(size),
           desc.width, desc.height,
           desc.channels, desc.colorspace,
           qoi_file,
           milliseconds_to_string(dur)
           );
  return(0);
}

void *qoi_decode_memory(const void *pixels, int qty){
  unsigned long   started         = timestamp();
  void            *decoded_pixels = NULL;
  struct qoi_desc desc;

  decoded_pixels = qoi_decode(pixels, qty, &desc, 4);
  if (!decoded_pixels) {
    log_error("Couldn't decode %d pixels", qty);
    return(NULL);
  }

  unsigned long dur = timestamp() - started;

  log_info("[QOI] Decoded %s bytes %dx%d %d channel RGB from memory in %s",
           bytes_to_string(qty),
           desc.width, desc.height, desc.channels,
           milliseconds_to_string(dur)
           );
  return(decoded_pixels);
}
struct qoi_encode_result_t *qoi_encode_memory(const void *pixels, int width, int height){
  struct qoi_encode_result_t *qoi_encoded = calloc(1, sizeof(struct qoi_encode_result_t));

  qoi_encoded->desc    = calloc(1, sizeof(struct qoi_desc));
  qoi_encoded->started = timestamp();
  qoi_encoded->desc    = &(struct qoi_desc){
    .width      = width,
    .height     = height,
    .channels   = 4,
    .colorspace = QOI_SRGB,
  };
  qoi_encoded->data = qoi_encode(pixels, &qoi_encoded->desc, &(qoi_encoded->size));
  if (!pixels) {
    log_error("Couldn't encode %dx%d pixels", width, height);
    return(NULL);
  }
  qoi_encoded->dur = timestamp() - qoi_encoded->started;
  log_info("[QOI] Encoded %dx%x RGB to %s in memory in %s",
           qoi_encoded->desc->width, qoi_encoded->desc->height,
           bytes_to_string(qoi_encoded->size), milliseconds_to_string(qoi_encoded->dur));
  return(qoi_encoded);
}
struct qoi_encode_to_file_result_t *qoi_encode_to_file(struct qoi_encode_to_file_request_t *req){
  struct qoi_encode_to_file_result_t *res = calloc(1, sizeof(struct qoi_encode_to_file_result_t));

  res->started = timestamp();
  //char *qoi_file, void *pixels, int width, int height){
  //unsigned long started = timestamp();
  //int           encoded = 0, channels = 4;
//  encoded =
  res->qoi_write_result = qoi_write(req->qoi_file_path, req->rgb_pixels, req->desc);
  /*
   * (qoi_desc){
   * .width      = width,
   * .height     = height,
   * .channels   = channels,
   * .colorspace = QOI_SRGB
   * });
   */
  if (!res->qoi_write_result) {
    log_error("Couldn't write/encode to file %s", req->qoi_file_path);
    return(NULL);
  }
  res->dur           = timestamp() - res->started;
  res->qoi_file_size = fsio_file_size(req->qoi_file_path);
  log_info("[QOI] Encoded %s to %s in %s", bytes_to_string(res->qoi_file_size), req->qoi_file_path, milliseconds_to_string(res->dur));
  //free(pixels);
  return(0);
}
////////////////////////////////////////////////////
