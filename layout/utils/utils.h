#pragma once
#ifndef LAYOUT_UTILS_H
#define LAYOUT_UTILS_H
#include "core/core.h"
#include "submodules/layout/layout.h"
struct layout_t;
enum layout_mode_t {
  LAYOUT_MODE_VERTICAL   = 0x002,
  LAYOUT_MODE_HORIZONTAL = 0x003,
  LAYOUT_MODES_QTY       = 2,
};
//////////////////////////////////////
struct layout_request_t {
  size_t             max_width, max_height;
  size_t             master_width, master_height;
  size_t             qty;
  size_t             master_margins[4], content_margins[4];
  enum layout_mode_t mode;
  bool               debug;
};

struct layout_content_result_t {
  uint32_t x, y;
  uint32_t width, height;
  pid_t    pid;
};

struct layout_render_request_t;
struct layout_result_t {
  struct layout_t                *layout;
  size_t                         qty;
  unsigned long                  started, dur;
  uint32_t                       width, height;
  struct layout_content_result_t *master;
  struct layout_content_result_t **items;
  struct layout_render_request_t *req;
  struct hk_render_t             *render;
  hash_t                         *props;
};

struct layout_image_t {
  unsigned char *qoi;
  size_t        len;
};
struct layout_render_request_t {
  struct layout_result_t *result;
  struct layout_image_t  *master;
  struct Vector          *content_images_v;
};

struct layout_render_result_t {
  unsigned long         started, dur;
  struct layout_image_t *image;
};

struct layout_t {
  lay_context *ctx;
  lay_id      master;
  lay_id      root, content, *contents;
};

void layout_test(void);
struct layout_request_t *layout_init_request();
struct layout_render_request_t *layout_init_render_request();
struct layout_result_t *layout_request(struct layout_request_t *req);
struct layout_render_result_t *layout_render(struct layout_render_request_t *req);
struct Vector *get_layouts_v(void);
bool layout_print_layout(char *name);
bool layout_print_result(struct layout_result_t *res);
#endif
