#pragma once
#include "wrec-common/wrec-common.h"
///////////////////////////////////
struct animated_png_render_request_t {
  char          *png_dir;
  char          *gif;
  unsigned long max_age_seconds, max_age_minutes, max_age_hours;
  unsigned long start_ts, end_ts;
  size_t        max_pngs, max_bytes;
  int           resize_width, resize_height;
};
int load_pngs_create_animated_gif(struct animated_png_render_request_t *req);
///////////////////////////////////
