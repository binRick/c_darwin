#pragma once
#define MSF_GIF_IMPL
#define STB_IMAGE_IMPLEMENTATION
#define MINIMUM_PNG_FILE_SIZE    67
#include <stdint.h>
#include <sys/stat.h>
////////////////////////////////////////////
#include "animated-gif/animated-gif.h"
////////////////////////////////////////////
#include "bytes/bytes.h"
#include "c_img/src/img.h"
#include "ms/ms.h"
#include "msf_gif/msf_gif.h"
#include "stb/stb_image.h"
#include "submodules/tinydir/tinydir.h"
#include "timestamp//timestamp.h"
#include "ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/include/vector.h"
#include "tempdir.c/tempdir.h"
////////////////////////////////////////////
struct file_time_t;
int compare_file_time_t(struct file_time_t *e1, struct file_time_t *e2);
int load_pngs_create_animated_gif(const char *PATH);
////////////////////////////////////////////
struct file_time_t {
  long unsigned             file_creation_ts, stb_render_ms, started_ms, frame_centiseconds;
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
  MsfGifState        *gifState;
  struct file_time_t *sorted_images;
  size_t             sorted_images_qty, sorted_images_size, pixels_qty, colors_qty, animated_gif_file_size;
  FILE               *fp;
  MsfGifResult       result;
  int                stb_req_format;
  tinydir_file       *td_file;
  tinydir_dir        *td_dir;
  char               *animated_gif_file_name;
  bool               success;
};
