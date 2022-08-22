#pragma once
#ifndef ANIMATED_GIF_C
#define ANIMATED_GIF_C
#include "animated-gif/animated-gif.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_img/src/img.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/include/vector.h"
#include "ms/ms.h"
#include "msf_gif/msf_gif.h"
#include "stb/stb_image.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"

////////////////////////////////////////////
static int compare_file_time_items(struct file_time_t *e1, struct file_time_t *e2);

////////////////////////////////////////////
int load_pngs_create_animated_gif(const char *ANIMATED_PNGS_DIR){
  char                *ANIMATED_GIF_FILE = "MyGif.gif";
  struct file_time_t  *f, *next_f;
  struct file_times_t *ft       = malloc(sizeof(struct file_times_t));
  MsfGifState         *gifState = calloc(1, sizeof(MsfGifState));
  {
    ft->td_file                 = calloc(1, sizeof(tinydir_file));
    ft->td_dir                  = calloc(1, sizeof(tinydir_dir));
    ft->success                 = false;
    ft->started_ms              = timestamp();
    ft->msf_gif_alpha_threshold = 1;
    ft->files_v                 = vector_new();
    ft->animated_gif_file_name  = ANIMATED_GIF_FILE;
    ft->stb_req_format          = STBI_rgb_alpha;
    ft->total_ms                = 0;
    ft->animated_gif_file_size  = 0;
    ft->pixels_qty              = 0;
    ft->colors_qty              = 0;
    ft->avg_ms                  = 0;
    ft->max_width               = 0;
    ft->max_height              = 0;
    ft->sorted_images_qty       = 0;
    ft->sorted_images_size      = 0;
    ft->verbose_mode            = true;
  }

  if (tinydir_open(ft->td_dir, ANIMATED_PNGS_DIR) == -1) {
    perror("Error opening file");
    goto end_tinydir;
  }
  while (ft->td_dir->has_next) {
    if (tinydir_readfile(ft->td_dir, ft->td_file) == -1) {
      perror("Error getting file");
      goto end_tinydir;
    }
    if (ft->td_file->is_dir != 0 || ft->td_file->is_reg != 1) {
      goto next_file;
    }
    if (false == fsio_file_exists(ft->td_file->path)) {
      goto next_file;
    }
    if (false == stringfn_ends_with(ft->td_file->path, ".png")) {
      goto next_file;
    }
    f = calloc(1, sizeof(struct file_time_t));
    if (!f) {
      goto end_tinydir;
    }
    f->file_size = fsio_file_size(ft->td_file->path);
    if (f->file_size < MINIMUM_PNG_FILE_SIZE) {
      free(f);
      goto next_file;
    }
    f->file_path = strdup(ft->td_file->path);
    if (!f->file_path) {
      free(f);
      goto next_file;
    }
    f->file_info = calloc(1, sizeof(struct stat));
    if (!f->file_info) {
      free(f);
      goto next_file;
    }
    stat(ft->td_file->path, f->file_info);
    if (!f->file_info) {
      free(f);
      goto next_file;
    }
    f->file_creation_ts = (f->file_info->st_birthtimespec.tv_sec * 1000000000) + f->file_info->st_birthtimespec.tv_nsec;
    if (f->file_creation_ts < 1000) {
      free(f);
      goto next_file;
    }
    f->image_dimensions = get_png_dimensions(f->file_path);
    if (!f->image_dimensions) {
      free(f);
      goto next_file;
    }
    if (f->image_dimensions->width < 1 || f->image_dimensions->height < 1 || f->image_dimensions->width < MINIMUM_PNG_WIDTH || f->image_dimensions->height < MINIMUM_PNG_HEIGHT) {
      free(f);
      goto next_file;
    }
    ft->max_width  = (f->image_dimensions->width > ft->max_width) ? f->image_dimensions->width : ft->max_width;
    ft->max_height = (f->image_dimensions->height > ft->max_height) ? f->image_dimensions->height : ft->max_height;
    vector_push(ft->files_v, (void *)f);

next_file:
    if (tinydir_next(ft->td_dir) == -1) {
      perror("Error getting next file");
      goto end_tinydir;
    }
  }

end_tinydir:
  tinydir_close(ft->td_dir);

  ft->sorted_images = calloc((vector_size(ft->files_v) + 1), sizeof(struct file_time_t));
  for (size_t i = 0; i < vector_size(ft->files_v); i++) {
    ft->sorted_images[i] = *((struct file_time_t *)vector_get(ft->files_v, i));
    ft->sorted_images_qty++;
    ft->sorted_images_size += ft->sorted_images[i].file_size;
  }
  qsort(ft->sorted_images, ft->sorted_images_qty, sizeof(struct file_time_t), compare_file_time_items);

  for (size_t i = 0; i < ft->sorted_images_qty; i++) {
    f           = &(ft->sorted_images[i]);
    next_f      = (i < (ft->sorted_images_qty - 1)) ? (&(ft->sorted_images[i + 1])) : (NULL);
    f->frame_ms = (next_f != NULL && (next_f->file_creation_ts > 0) && (f->file_creation_ts > 0))
      ? (((next_f->file_creation_ts - f->file_creation_ts)) / (1000))
      : 0;
    ft->total_ms += f->frame_ms;
  }
  f                       = NULL;
  ft->avg_ms              = ft->total_ms / (ft->sorted_images_qty - 1);
  ft->total_ms            = 0;
  msf_gif_alpha_threshold = ft->msf_gif_alpha_threshold;
  msf_gif_begin(gifState, ft->max_width, ft->max_height);
  for (size_t i = 0; i < ft->sorted_images_qty; i++) {
    f = &(ft->sorted_images[i]);
    if (!f) {
      continue;
    }
    f->started_ms = timestamp();
    f->frame_ms   = (f->frame_ms > 0) ? f->frame_ms : (ft->avg_ms);
    ft->total_ms += f->frame_ms;
  }
  f = NULL;
  if (ft->verbose_mode) {
    printf(AC_YELLOW "Creating %dx%d Animated gif with %lu images of %s, %lu avg interval" AC_RESETALL "\n",
           ft->max_width, ft->max_height, ft->sorted_images_qty, bytes_to_string(ft->sorted_images_size),
           ft->avg_ms
           );
  }
  ft->avg_ms = ft->total_ms / ft->sorted_images_qty;
  for (size_t i = 0; i < ft->sorted_images_qty; i++) {
    f      = &(ft->sorted_images[i]);
    ft->fp = fopen(f->file_path, "r");
    if (!ft->fp) {
      continue;
    }
    int h, w;
    f->stb_pixels               = stbi_load_from_file(ft->fp, &w, &h, &f->stb_format, ft->stb_req_format);
    f->image_dimensions->width  = w;
    f->image_dimensions->height = h;
    fclose(ft->fp);
    if (!f->stb_pixels) {
      continue;
    }
    f->stb_depth    = (f->stb_format == 4) ? 32 : 24;
    f->stb_pitch    = (f->stb_format == 4) ? (4 * f->image_dimensions->width) : (3 * f->image_dimensions->width);
    f->pixels_qty   = f->image_dimensions->width * f->image_dimensions->height;
    f->colors_qty   = f->stb_pitch * f->image_dimensions->height;
    ft->pixels_qty += f->pixels_qty;
    ft->colors_qty += f->colors_qty;
    msf_gif_frame(gifState, f->stb_pixels,
                  (int)(f->frame_ms / 10),
                  f->stb_depth,
                  f->stb_pitch
                  );
    free(f->stb_pixels);
    f->stb_render_ms = timestamp() - f->started_ms;
    if (ft->verbose_mode) {
      printf(AC_YELLOW "Added %s (%dx%d) %s frame in %s" AC_RESETALL "\n",
             bytes_to_string(f->file_size),
             f->image_dimensions->width, f->image_dimensions->height,
             milliseconds_to_string(f->frame_ms),
             milliseconds_to_string(f->stb_render_ms)
             );
    }
  }
  f = NULL;
  MsfGifResult gifResult = msf_gif_end(gifState);

  if (gifResult.data != NULL) {
    ft->fp = fopen(ft->animated_gif_file_name, "wb");
    if (ft->fp) {
      fwrite(gifResult.data, gifResult.dataSize, 1, ft->fp);
      fclose(ft->fp);
      ft->success = true;
    }
    msf_gif_free(gifResult);
  }
  ft->dur_ms                 = timestamp() - ft->started_ms;
  ft->animated_gif_file_size = fsio_file_size(ft->animated_gif_file_name);
  if (ft->success) {
    if (ft->verbose_mode) {
      printf(AC_GREEN "Rendered %s Animated Gif %s (%s, %dx%d, %lu frames, %.1fFPS) from %lu images (%lu million pixels, %lu million colors, %s) in %s!" AC_RESETALL "\n",
             milliseconds_to_string(ft->total_ms),
             ft->animated_gif_file_name,
             bytes_to_string(ft->animated_gif_file_size),
             ft->max_width, ft->max_height,
             ft->sorted_images_qty,
             (float)(((float)1) / ((float)((((float)(ft->total_ms * 1000) / ((float)ft->sorted_images_qty)))) / (1000 * 1000))),
             ft->sorted_images_qty,
             ft->pixels_qty / 1000 / 1000, ft->colors_qty / 1000 / 1000,
             bytes_to_string(ft->sorted_images_size),
             milliseconds_to_string(ft->dur_ms)
             );
    }
  }
  return((ft->success == true) ? 0 : 1);
} /* load_pngs_create_animated_gif */

static int compare_file_time_items(struct file_time_t *e1, struct file_time_t *e2){
  int ret = (e1->file_creation_ts > e2->file_creation_ts)
              ? 1
              : (e1->file_creation_ts < e2->file_creation_ts)
                ? -1
                : 0;

  return(ret);
}

#endif
