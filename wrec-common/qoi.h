#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
////////////////////////////////////////////////////////////////////////////////////////
#ifndef QOI_SRGB                                       /////////////////     QOI   ////
#define QOI_SRGB    0                                  ////////////////////////////////
#endif
struct qoi_desc {
  unsigned int  width;
  unsigned int  height;
  unsigned char channels;
  unsigned char colorspace;
};
struct qoi_encode_to_file_result_t {
  int           qoi_write_result;
  unsigned long started, dur;
  size_t        qoi_file_size;
};
struct qoi_encode_to_file_request_t {
  char              *qoi_file_path;
  unsigned long     started, dur;
  size_t            encoded_qoi_file_size;
  void              *rgb_pixels;
  struct   qoi_desc *desc;
};
struct qoi_encode_result_t {
  void            *data;
  size_t          size;
  unsigned long   dur, started;
  struct qoi_desc *desc;
};
