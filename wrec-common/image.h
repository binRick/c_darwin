#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "c_img/src/img.h"
#include "tinydir/tinydir.h"
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
