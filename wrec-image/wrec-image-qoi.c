#pragma once
#define QOI_IMPLEMENTATION
#include "qoi/qoi.h"
#include "wrec-common/wrec-common.h"
#include "wrec-image/wrec-image.h"

////////////////////////////////////////////////////
struct qoi_decode_result_t *qoi_decode_file(char *qoi_file){
  struct qoi_decode_result_t *res = calloc(1, sizeof(struct qoi_decode_result_t));

  res->started    = timestamp();
  res->size       = fsio_file_size(qoi_file);
  res->rgb_pixels = NULL;
  res->desc       = calloc(1, sizeof(qoi_desc));

  res->rgb_pixels = qoi_read(qoi_file, res->desc, RGBA_CHANNELS);
  if (res->rgb_pixels == NULL) {
    printf("Couldn't load/decode %s\n", qoi_file);
    return(NULL);
  }

  res->dur = timestamp() - res->started;
  log_info("[QOI-DECODE-FILE] Decoded %s RGB Pixels %dx%d %d channels %d colorspace from file %s in %s",
           bytes_to_string(res->size),
           res->desc->width, res->desc->height,
           res->desc->channels, res->desc->colorspace,
           qoi_file,
           milliseconds_to_string(res->dur)
           );
  return(res);
}

struct qoi_decode_result_t *qoi_decode_memory(const void *pixels, int pixels_qty){
  struct qoi_decode_result_t *res = calloc(1, sizeof(struct qoi_decode_result_t));

  res->started    = timestamp();
  res->rgb_pixels = NULL;
  res->desc       = calloc(1, sizeof(qoi_desc));

  res->rgb_pixels = qoi_decode(pixels, pixels_qty, res->desc, QOI_SRGB);
  if (res->rgb_pixels == NULL) {
    log_error("Couldn't decode %d pixels", pixels_qty);
    return(NULL);
  }

  res->dur = timestamp() - res->started;
  log_info("[QOI-DECODE-MEMORY] Decoded %s QOI pixels from memory to %dx%d %d channels RGB memory in %s",
           bytes_to_string(pixels_qty),
           res->desc->width, res->desc->height, res->desc->channels,
           milliseconds_to_string(res->dur)
           );
  return(res);
}
struct qoi_encode_result_t *qoi_encode_memory(const void *pixels, int width, int height){
  struct qoi_encode_result_t *qoi_encoded = calloc(1, sizeof(struct qoi_encode_result_t));

  qoi_encoded->desc             = calloc(1, sizeof(struct qoi_desc));
  qoi_encoded->started          = timestamp();
  qoi_encoded->desc->width      = width;
  qoi_encoded->desc->height     = width;
  qoi_encoded->desc->colorspace = QOI_SRGB;
  qoi_encoded->desc->channels   = RGBA_CHANNELS;
  int size;

  qoi_encoded->data = qoi_encode(pixels, qoi_encoded->desc, &size);
  if (!pixels) {
    log_error("Couldn't encode %dx%d pixels", width, height);
    return(NULL);
  }
  qoi_encoded->size = (size_t)size;
  qoi_encoded->dur  = timestamp() - qoi_encoded->started;
  log_info("[QOI-ENCODE-MEMORY] Encoded %dx%x RGB to %s in memory in %s",
           qoi_encoded->desc->width, qoi_encoded->desc->height,
           bytes_to_string(qoi_encoded->size), milliseconds_to_string(qoi_encoded->dur));
  return(qoi_encoded);
}
struct qoi_encode_to_file_result_t *qoi_encode_to_file(struct qoi_encode_to_file_request_t *req){
  struct qoi_encode_to_file_result_t *res = calloc(1, sizeof(struct qoi_encode_to_file_result_t));

  res->started          = timestamp();
  res->qoi_write_result = qoi_write(req->qoi_file_path, req->rgb_pixels, req->desc);
  if (!res->qoi_write_result) {
    log_error("Couldn't write/encode to file %s", req->qoi_file_path);
    return(NULL);
  }
  res->dur           = timestamp() - res->started;
  res->qoi_file_size = fsio_file_size(req->qoi_file_path);
  log_info("[QOI-ENCODE-FILE] Encoded %s to %s in %s", bytes_to_string(res->qoi_file_size), req->qoi_file_path, milliseconds_to_string(res->dur));
  return(0);
}
////////////////////////////////////////////////////
