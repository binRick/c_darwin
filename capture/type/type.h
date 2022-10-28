#pragma once
#ifndef CAPTURE_TYPE_H
#define CAPTURE_TYPE_H
#define VIPS_WARNING    1
#define MAX_QUEUES      24
#include "c_workqueue/include/workqueue.h"
#include "chan/src/chan.h"
#include "core/core.h"
#include "image/utils/utils.h"
#include "libimagequant/libimagequant.h"
#include "msf_gif/msf_gif.h"
#include "qoi/qoi.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
enum capture_type_id_t {
  CAPTURE_TYPE_WINDOW,
  CAPTURE_TYPE_DISPLAY,
  CAPTURE_TYPE_SPACE,
  CAPTURE_TYPES_QTY,
};
enum capture_mode_type_t {
  CAPTURE_MODE_TYPE_IMAGE,
  CAPTURE_MODE_TYPE_ANIMATION,
  CAPTURE_MODE_TYPE_EXTRACT,
  CAPTURE_MODE_TYPES_QTY,
};
struct cap_t;
typedef void (*chan_handler_t)(void *CHAN);
typedef void (*queue_handler_t)(void *CHAN);
typedef void *(^recv_msg_b)(void *MSG);
typedef void (*send_msg_t)(void *recv, struct cap_t *cap);
void send_msg(void *recv, struct cap_t *cap);
struct capture_time_t {
  unsigned long started, dur, captured_ts;
};
struct capture_image_request_t {
  struct Vector          *ids;
  int                    concurrency;
  int                    width, height;
  bool                   compress, progress_bar_mode, quantize_mode, db_save_mode, grayscale_mode;
  enum capture_type_id_t type;
  enum image_type_id_t   format;
  struct capture_time_t  time;
  struct compress_req_t  *comp;
  cbar_t                 *bar;
};
struct cgimage_recv_t {
  void                           *msg;
  CGImageRef                     img_ref;
  size_t                         width, height, index, len;
  struct capture_image_request_t *req;
  struct capture_time_t          time;
};
struct capture_image_result_t {
  struct cgimage_recv_t  *msg;
  enum capture_type_id_t type;
  enum image_type_id_t   format;
  char                   *file, *thumbnail_file;
  unsigned char          *pixels;
  size_t                 width, height, len, id;
  bool                   has_alpha, linear_colorspace, analyze, grayscale_mode;
  struct capture_time_t  time;
  unsigned long          ts, delta_ms;
};
enum capture_chan_type_t {
  CAPTURE_CHAN_TYPE_CGIMAGE = 1,
  CAPTURE_CHAN_TYPE_RGB,
  CAPTURE_CHAN_TYPE_BMP,
  CAPTURE_CHAN_TYPE_PNG,
  CAPTURE_CHAN_TYPE_JPEG,
  CAPTURE_CHAN_TYPE_TIFF,
  CAPTURE_CHAN_TYPE_WEBP,
  CAPTURE_CHAN_TYPE_GIF,
  CAPTURE_CHAN_TYPE_QOI,
  CAPTURE_CHAN_TYPE_TIMG,
  CAPTURE_CHAN_TYPE_CGIMAGE_GRAYSCALE,
  CAPTURE_CHAN_TYPES_QTY,
};
enum capture_provider_type_t {
  CAPTURE_PROVIDER_TYPE_CAP,
  CAPTURE_PROVIDER_TYPE_IDS,
};
struct compress_req_t {
  pthread_t *threads[MAX_QUEUES];
  pthread_t *waiter;
  chan_t    *chans[MAX_QUEUES], *done;
  chan_t    *chan;
  chan_t    *waiter_chan;
  cbar_t    *bar;
};
struct compress_t {
  unsigned char                 *pixels;
  size_t                        len;
  size_t                        prev_len;
  int                           min_quality, max_quality;
  enum image_type_id_t          format;
  size_t                        id;
  struct capture_image_result_t *capture_result;
  unsigned long                 started, dur;
  bool                          quantize_mode, grayscale_mode, compress_mode;
  cbar_t                        *bar;
};
struct cap_t {
  char                         *name;
  enum image_type_id_t         format;
  chan_t                       *recv_chan, *send_chan, *done_chan, *db_chan, *db_done_chan;
  bool                         enabled, debug, compress;
  pthread_t                    *threads[MAX_QUEUES], *db_threads[MAX_QUEUES];
  size_t                       qty, total_qty;
  enum capture_provider_type_t provider_type;
  enum capture_chan_type_t     provider;
  recv_msg_b                   recv_msg;
  cbar_t                       *bar;
};
struct capture_chan_t {
  chan_t               *chan, *done_chan, *db_done_chan;
  size_t               qty;
  bool                 enabled, debug;
  queue_handler_t      provider, receiver;
  struct WorkQueue     *providers[MAX_QUEUES], *receivers[MAX_QUEUES];
  enum image_type_id_t type;
};
struct gif_receiver_t {
  unsigned char *gif;
  size_t        len, id;
};
struct png_receiver_t {
  unsigned char *png;
  size_t        len, id;
};
struct rgb_receiver_t {
  unsigned char *rgb;
  size_t        len, id, width, height;
};
struct cgimage_receiver_t {
  CGImageRef image_ref;
  size_t     id;
};
struct cgimage_args_t {
  size_t id;
  int    width, height;
};
char *get_capture_type_name(enum capture_type_id_t type);
struct Vector *db_tables_images(enum capture_type_id_t type, struct Vector *ids);
struct Vector *capture_image(struct capture_image_request_t *req);
#endif
