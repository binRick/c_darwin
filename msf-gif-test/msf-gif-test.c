#define MSF_GIF_IMPL
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stdint.h>
#include <sys/stat.h>
////////////////////////////////////////////
#include "bytes/bytes.h"
#include "c_img/src/img.h"
#include "ms/ms.h"
#include "msf_gif/msf_gif.h"
#include "stb/stb_image.h"
#include "submodules/log.h/log.h"
#include "submodules/tinydir/tinydir.h"
#include "timestamp//timestamp.h"
////////////////////////////////////////////
#include "msf-gif-test/msf-gif-test.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/include/vector.h"
////////////////////////////////////////////
void __attribute__((constructor)) __constructor__msf_gif_test();
void __attribute__((destructor)) __destructor__msf_gif_test();
void __msf_gif_test__setup_executable_path(const char **argv);
struct file_time_t {
  long unsigned             ts, render_ms, started_ms, frame_centiseconds;
  char                      *path;
  size_t                    file_size;
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
  size_t             sorted_images_qty, sorted_images_size;
  FILE               *fp;
  MsfGifResult       result;
  int                stb_req_format;
  tinydir_file       *td_file;
  tinydir_dir        *td_dir;
  char               *animated_gif_file_name;
};

int compare_file_time_t(struct file_time_t *e1, struct file_time_t *e2){
  int ret = (e1->ts > e2->ts)
              ? 1
              : (e1->ts < e2->ts)
                ? -1
                : 0;

  return(ret);
}

TEST t_tinydir_0(void *PATH){
  struct file_time_t  *f, *next_f;
  struct file_times_t *ft = malloc(sizeof(struct file_times_t));
  {
    ft->gifState               = calloc(1, sizeof(MsfGifState));
    ft->td_file                = calloc(1, sizeof(tinydir_file));
    ft->td_dir                 = calloc(1, sizeof(tinydir_dir));
    ft->animated_gif_file_name = "MyGif.gif";
    ft->stb_req_format         = STBI_rgb_alpha;
    ft->started_ms             = timestamp();
    ft->total_ms               = 0;
    ft->max_width              = 0;
    ft->max_height             = 0;
    ft->sorted_images_qty      = 0;
    ft->sorted_images_size     = 0;
    ft->files_v                = vector_new();
  }

  if (tinydir_open(ft->td_dir, (char *)PATH) == -1) {
    perror("Error opening file");
    goto end_tinydir;
  }

  while (ft->td_dir->has_next) {
    if (tinydir_readfile(ft->td_dir, ft->td_file) == -1) {
      perror("Error getting file");
      goto end_tinydir;
    }

    if (!ft->td_file->is_dir) {
      f            = calloc(1, sizeof(struct file_time_t));
      f->file_info = calloc(1, sizeof(struct stat));
      f->path      = strdup(ft->td_file->path);
      if (!f->path) {
        continue;
      }
      f->file_size = fsio_file_size(f->path);
      if (f->file_size < 32) {
        continue;
      }
      stat(ft->td_file->path, f->file_info);
      if (!f->file_info) {
        continue;
      }
      f->ts               = (f->file_info->st_birthtimespec.tv_sec * 1000000000) + f->file_info->st_birthtimespec.tv_nsec;
      f->image_dimensions = get_png_dimensions(f->path);
      if (!f->image_dimensions) {
        continue;
      }
      if (f->image_dimensions->width < 1 || f->image_dimensions->height < 1) {
        continue;
      }
      ft->max_width  = (f->image_dimensions->width > ft->max_width) ? f->image_dimensions->width : ft->max_width;
      ft->max_height = (f->image_dimensions->height > ft->max_height) ? f->image_dimensions->height : ft->max_height;
      vector_push(ft->files_v, (void *)f);
    }

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
  qsort(ft->sorted_images, ft->sorted_images_qty, sizeof(struct file_time_t), compare_file_time_t);

  for (size_t i = 0; i < ft->sorted_images_qty; i++) {
    f                     = &(ft->sorted_images[i]);
    next_f                = (i < ft->sorted_images_qty) ? (&(ft->sorted_images[i + 1])) : (NULL);
    f->frame_centiseconds = (next_f != NULL && next_f->ts > 0 && f->ts > 0) ? ((next_f->ts - f->ts) / 1000000 / 10) : 0;
    ft->total_ms         += f->frame_centiseconds * 10;
    printf("Sorted File #%lu :: %s :: %ld :: %dx%d ::\n",
           i + 1, f->path, f->ts, f->image_dimensions->width, f->image_dimensions->height
           );
  }
  ft->avg_ms = ft->total_ms / ft->sorted_images_qty;

  printf("Creating %dx%d Animated gif with %lu images of %s\n", ft->max_width, ft->max_height, ft->sorted_images_qty, bytes_to_string(ft->sorted_images_size));
  msf_gif_begin(ft->gifState, ft->max_width, ft->max_height);
  for (size_t i = 0; i < ft->sorted_images_qty; i++) {
    f = &(ft->sorted_images[i]);
    if (!f) {
      continue;
    }
    f->started_ms         = timestamp();
    f->frame_centiseconds = (f->frame_centiseconds > 0) ? f->frame_centiseconds : (ft->avg_ms / 10);
    ft->fp                = fopen(ft->sorted_images[i].path, "r");
    if (!ft->fp) {
      continue;
    }
    f->stb_pixels = stbi_load_from_file(ft->fp, &f->image_dimensions->width, &f->image_dimensions->height, &f->stb_format, ft->stb_req_format);
    fclose(ft->fp);
    if (!f->stb_pixels) {
      continue;
    }
    f->stb_depth = (ft->stb_req_format == STBI_rgb) ? 24 : 32;
    f->stb_pitch = (ft->stb_req_format == STBI_rgb) ? (3 * f->image_dimensions->height) : (4 * f->image_dimensions->width);
    msf_gif_frame(ft->gifState, f->stb_pixels, (int)f->frame_centiseconds, f->stb_depth, f->stb_pitch);
    free(f->stb_pixels);
    f->render_ms = timestamp() - f->started_ms;
    printf("Added %s (%dx%d) %s frame of %lu centiseconds (%s) in %s\n",
           bytes_to_string(f->file_size),
           f->image_dimensions->width, f->image_dimensions->height,
           bytes_to_string(f->file_size), f->frame_centiseconds,
           milliseconds_to_string(f->frame_centiseconds * 10),
           milliseconds_to_string(f->render_ms)
           );
  }
  ft->result = msf_gif_end(ft->gifState);
  ft->dur_ms = timestamp() - ft->started_ms;
  if (ft->result.data != NULL) {
    ft->fp = fopen(ft->animated_gif_file_name, "wb");
    if (ft->fp) {
      fwrite(ft->result.data, ft->result.dataSize, 1, ft->fp);
      fclose(ft->fp);
    }
    msf_gif_free(ft->result);
  }
  printf(AC_GREEN "Rendered %s byte %s %dx%d %s from %lu images of %s in %s!" AC_RESETALL "\n",
         bytes_to_string(ft->result.dataSize),
         milliseconds_to_string(ft->total_ms),
         ft->max_width, ft->max_height,
         ft->animated_gif_file_name,
         ft->sorted_images_qty,
         bytes_to_string(ft->sorted_images_size),
         milliseconds_to_string(ft->dur_ms)
         );
  PASS();
} /* t_tinydir_0 */

GREATEST_MAIN_DEFS();

SUITE(s_tinydir){
  RUN_TESTp(t_tinydir_0, ".pngs");
  PASS();
}

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  (void)argc; (void)argv;
  RUN_SUITE(s_tinydir);

  GREATEST_MAIN_END();
  return(0);
}