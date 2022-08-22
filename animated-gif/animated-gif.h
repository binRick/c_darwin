#pragma once
#ifndef ANIMATED_GIF_H
#define ANIMATED_GIF_H
#define MSF_GIF_IMPL
#define STB_IMAGE_IMPLEMENTATION
#define MINIMUM_PNG_FILE_SIZE    67
#define MINIMUM_PNG_WIDTH        10
#define MINIMUM_PNG_HEIGHT       10
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
////////////////////////////////////////////
#include "submodules/tinydir/tinydir.h"
////////////////////////////////////////////
int load_pngs_create_animated_gif(const char *PATH);
////////////////////////////////////////////
struct file_time_t {
  long unsigned             file_creation_ts, stb_render_ms, started_ms, frame_ms;
  char                      *file_path;
  size_t                    file_size, pixels_qty, colors_qty;
  int                       stb_format, stb_depth, stb_pitch;
  unsigned char             *stb_pixels;
  struct image_dimensions_t *image_dimensions;
  struct stat               *file_info;
};
int compare_file_time_t(struct file_time_t *e1, struct file_time_t *e2);
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

#endif
