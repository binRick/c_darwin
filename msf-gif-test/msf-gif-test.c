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
  long unsigned ts;
  char          *path;
  size_t        dur_ms;
  int           width, height;
};

int compare_file_time_t(void *_e1, void *_e2){
  struct file_time_t *e1 = (struct file_time_t *)_e1;
  struct file_time_t *e2 = (struct file_time_t *)_e2;

  printf("comparing %s:%lu | %s:%lu\n", e1->path, e1->ts, e2->path, e2->ts);
  if (e1->ts > e2->ts) {
    return(1);
  }
  if (e1->ts < e2->ts) {
    return(-1);
  }
  return(0);
}

TEST t_tinydir_0(void *PATH){
  struct Vector *file_times = vector_new();
  tinydir_dir   dir;

  if (tinydir_open(&dir, (char *)PATH) == -1) {
    perror("Error opening file");
    goto bail;
  }

  while (dir.has_next) {
    tinydir_file file;
    if (tinydir_readfile(&dir, &file) == -1) {
      perror("Error getting file");
      goto bail;
    }

    if (file.is_dir) {
      printf(" (DIRECTORY)");
    }else{
      printf(" (FILE)");
      printf("\t%s", file.name);
      struct stat        info = {};
      stat(file.path, &info);
      struct file_time_t *ft = malloc(sizeof(struct file_time_t));
      ft->path = strdup(file.path);
      ft->ts   = (info.st_birthtimespec.tv_sec * 1000000000) + info.st_birthtimespec.tv_nsec;
      vector_push(file_times, (void *)ft);
    }

    if (tinydir_next(&dir) == -1) {
      perror("Error getting next file");
      goto bail;
    }
    printf("\n");
  }

bail:
  tinydir_close(&dir);

  unsigned long      started = timestamp();
  struct file_time_t *buffer = calloc((vector_size(file_times) + 1), sizeof(struct file_time_t));

  for (size_t i = 0; i < vector_size(file_times); i++) {
    struct file_time_t *f = vector_get(file_times, i);
    printf("File #%lu :: %s :: %ld\n",
           i + 1, f->path, f->ts
           );
    struct file_time_t c = {
      .ts   = f->ts,
      .path = f->path,
      //.dur = i < vector_size(file_times) ? vector_get(file_times,i+1)->ts : avg_dur;
    };
    buffer[i] = c;
  }
  int max_w = 0, max_h = 0;

  qsort(buffer, vector_size(file_times), sizeof(struct file_time_t), compare_file_time_t);
  FILE               *p;

  struct file_time_t *f;
  int                qty = vector_size(file_times);
  int                img_durations[qty];
  size_t             last_ts  = 0;
  int                avg_dur  = 0;
  size_t             total_ms = 0;

  for (size_t i = 0; i < vector_size(file_times); i++) {
    f = &(buffer[i]);
    struct image_dimensions_t *d = get_png_dimensions(f->path);
    if (!d) {
      continue;
    }
    max_w     = (d->width > max_w) ? d->width : max_w;
    max_h     = (d->height > max_h) ? d->height : max_h;
    f->dur_ms = 0;
    f->width  = d->width;
    f->height = d->height;
    if (i > 0 && i < (vector_size(file_times))) {
      f->dur_ms = (size_t)((f->ts - last_ts) / 1000000);
      total_ms += f->dur_ms;
    }
    printf("Sorted File #%lu :: %s :: %ld :: %dx%d :: last ts %lu |dur:%lu|totalms:%lu|\n",
           i + 1, f->path, f->ts, f->width, f->height,
           last_ts, f->dur_ms, total_ms
           );
    last_ts = (size_t)f->ts;
    free(d);
  }
  printf("total time:  %lums :: %s\n", total_ms, milliseconds_to_long_string(total_ms));

  printf("Creating %dx%d Animated gif\n", max_w, max_h);
  int         centisecondsPerFrame = 5;
  MsfGifState gifState             = {};

  msf_gif_begin(&gifState, max_w, max_h);
  size_t avg_ms = total_ms / vector_size(file_times);

  for (size_t i = 0; i < vector_size(file_times); i++) {
    f = &(buffer[i]);
    p = fopen(buffer[i].path, "r");
    int           w, h, orig_format, req_format = STBI_rgb_alpha, depth, pitch;
    unsigned char *pixels = stbi_load_from_file(p, &w, &h, &orig_format, req_format);
    fclose(p);

    if (!pixels || w < 1 || h < 1) {
      continue;
    }
    if (req_format == STBI_rgb) {
      depth = 24;
      pitch = 3 * w;
    } else {
      depth = 32;
      pitch = 4 * w;
    }
    size_t cs = (f->dur_ms > 0) ? (f->dur_ms / 10) : (avg_ms / 10);
    printf("cs:%lu\n", cs);
    msf_gif_frame(&gifState, pixels, cs, depth, pitch);
    free(pixels);
  }
  MsfGifResult  result = msf_gif_end(&gifState);
  unsigned long dur_ms = timestamp() - started;

  printf("Rendered %s byte animated gif in %s!\n", bytes_to_string(result.dataSize), milliseconds_to_long_string(dur_ms));
  printf("avg ms:%lu\n", avg_ms);
  if (result.data) {
    FILE *fp = fopen("MyGif.gif", "wb");
    fwrite(result.data, result.dataSize, 1, fp);
    fclose(fp);
  }
  msf_gif_free(result);

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
