#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "c_img/src/img.h"
#include "tinydir/tinydir.h"
#include "wrec-common/qoi.h"
#include <ApplicationServices/ApplicationServices.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
////////////////////////////////////////////////////////////////////////////////
struct loaded_png_file_t *load_png_file_pixels(char *png_file);
struct image_dimensions_t *get_png_file_dimensions(char *path);
struct qoi_encode_to_file_result_t *qoi_encode_to_file(struct qoi_encode_to_file_request_t *req);
int qoi_decode_file(char *qoi_file);
struct qoi_encode_result_t *qoi_encode_memory(const void *rgb_pixels, int width, int height);
void *qoi_decode_memory(const void *qoi_data, int qoi_data_size);
////////////////////////////////////////////////////////////////////////////////
struct loaded_png_file_t {
  int                       width, height, stb_format, stb_req_format;
  unsigned char             *rgb_pixels;
  char                      *path;
  size_t                    size;
  unsigned long             dur;
  struct image_dimensions_t image_dimensions;
};
struct file_time_t {
  long unsigned             file_creation_ts, stb_render_ms, started_ms, frame_ms;
  char                      *file_path;
  size_t                    file_size, pixels_qty, colors_qty;
  int                       stb_format, stb_depth, stb_pitch;
  unsigned char             *stb_pixels;
  struct image_dimensions_t *image_dimensions;
  struct stat               *file_info;
};
struct file_times_t {
  struct Vector      *files_v;
  struct file_time_t **files;
  long unsigned      render_ms, started_ms, total_ms, dur_ms, avg_ms;
  int                max_width, max_height;
  struct file_time_t *sorted_images;
  size_t             sorted_images_qty, sorted_images_size, pixels_qty, colors_qty, animated_gif_file_size;
  FILE               *fp;
  int                stb_req_format;
  tinydir_file       *td_file;
  tinydir_dir        *td_dir;
  char               *animated_gif_file_name;
  bool               success, verbose_mode;
  int                msf_gif_alpha_threshold;
};
