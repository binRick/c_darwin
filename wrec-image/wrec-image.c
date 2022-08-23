#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "wrec-common/wrec-common.h"
#include "wrec-image/wrec-image.h"
////////////////////////////////////////////////////
struct image_dimensions_t *get_png_file_dimensions(char *path){
  return(get_png_dimensions(path));
}
struct loaded_png_file_t *load_png_file_pixels(char *png_file){
  unsigned long             started           = timestamp();
  struct image_dimensions_t *image_dimensions = get_png_dimensions(png_file);
  struct loaded_png_file_t  *res              = calloc(1, sizeof(struct loaded_png_file_t));

  res->path           = png_file;
  res->stb_req_format = STBI_rgb_alpha;
  res->size           = fsio_file_size(res->path);
  log_info("[c_img] Parsed %s File %s (%dx%d)",
           bytes_to_string(res->size),
           res->path,
           image_dimensions->width, image_dimensions->height
           );
  FILE *png_fp = fopen(res->path, "r");

  res->rgb_pixels = stbi_load_from_file(png_fp, &res->image_dimensions.width, &res->image_dimensions.height, &res->stb_format, res->stb_req_format);
  fclose(png_fp);
  res->dur = timestamp() - started;
  log_info("[STB] Loaded PNG %s (%dx%d) in %s",
           res->path,
           res->image_dimensions.width, res->image_dimensions.height,
           milliseconds_to_string(res->dur)
           );
  return(res);
}
