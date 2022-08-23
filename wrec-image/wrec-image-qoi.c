#pragma once
#define QOI_IMPLEMENTATION
#include "qoi/qoi.h"
#include "wrec-common/wrec-common.h"
#include "wrec-image/wrec-image.h"
////////////////////////////////////////////////////
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
