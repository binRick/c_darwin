#pragma once
#ifndef CAPTURE_WINDOW_H
#define CAPTURE_WINDOW_H
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
//#include "vips/vips.h"
enum capture_type_id_t {
  CAPTURE_TYPE_WINDOW  = 100,
  CAPTURE_TYPE_DISPLAY = 200,
  CAPTURE_TYPE_SPACE   = 300,
  CAPTURE_TYPES_QTY    = 3,
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
struct capture_req_t {
  struct Vector          *ids;
  int                    concurrency;
  int                    width, height;
  bool                   compress;
  enum capture_type_id_t type;
  enum image_type_id_t   format;
  struct capture_time_t  time;
};
struct animated_frame_t {
  unsigned char *pixels;
  size_t        width, height, len, id;
  unsigned long ts;
};
struct animated_capture_t {
  MsfGifState            *gif;
  MsfGifResult           result;
  char                   *file;
  struct Vector          *frames_v;
  size_t                 width, height, len, id, ms_per_frame, bit_depth, total_size;
  enum capture_type_id_t type;
  enum image_type_id_t   format;
  unsigned long          started;
  int                    max_bit_depth, pitch_bytes;
  chan_t                 *chan, *done;
  pthread_t              *thread;
  pthread_mutex_t        *mutex;
};
struct cgimage_recv_t {
  void                  *msg;
  CGImageRef            img_ref;
  size_t                width, height, index, len;
  struct capture_req_t  *req;
  struct capture_time_t time;
};
struct capture_result_t {
  struct cgimage_recv_t *msg;
  enum image_type_id_t  type;
  char                  *file, *thumbnail_file;
  unsigned char         *pixels;
  size_t                width, height, len, id;
  bool                  has_alpha, linear_colorspace, analyze;
  struct capture_time_t time;
  unsigned long         ts, delta_ms;
};
enum capture_chan_type_t {
  CAPTURE_CHAN_TYPE_CGIMAGE = 1,
  CAPTURE_CHAN_TYPE_RGB,
  CAPTURE_CHAN_TYPE_BMP,
  CAPTURE_CHAN_TYPE_PNG,
  CAPTURE_CHAN_TYPE_JPEG,
  CAPTURE_CHAN_TYPE_TIFF,
  CAPTURE_CHAN_TYPE_TIFF_GRAYSCALE,
  CAPTURE_CHAN_TYPE_CGIMAGE_GRAYSCALE,
  CAPTURE_CHAN_TYPE_PNG_COMPRESSED,
  CAPTURE_CHAN_TYPE_PNG_GRAYSCALE,
  CAPTURE_CHAN_TYPE_GIF_GRAYSCALE,
  CAPTURE_CHAN_TYPE_JPEG_GRAYSCALE,
  CAPTURE_CHAN_TYPE_BMP_GRAYSCALE,
  CAPTURE_CHAN_TYPE_RGB_GRAYSCALE,
  CAPTURE_CHAN_TYPE_QOI_GRAYSCALE,
  CAPTURE_CHAN_TYPE_GIF,
  CAPTURE_CHAN_TYPE_QOI,
  CAPTURE_CHAN_TYPE_TIMG,
  CAPTURE_CHAN_TYPES_QTY,
};
enum capture_provider_type_t {
  CAPTURE_PROVIDER_TYPE_CAP,
  CAPTURE_PROVIDER_TYPE_IDS,
};
struct cap_t {
  char                         *name;
  enum image_type_id_t         format;
  chan_t                       *recv_chan, *send_chan, *done_chan;
  bool                         enabled, debug;
  pthread_t                    *threads[MAX_QUEUES];
  size_t                       qty;
  enum capture_provider_type_t provider_type;
  enum capture_chan_type_t     provider;
  recv_msg_b                   recv_msg;
};
struct capture_windows_t {
  struct Vector            *window_ids;
  int                      concurrency;
  enum capture_chan_type_t type;
  int                      width, height;
  bool                     compress;
};
struct capture_chan_t {
  chan_t               *chan, *done_chan;
  size_t               qty;
  bool                 enabled, debug;
  queue_handler_t      provider, receiver;
  struct WorkQueue     *providers[MAX_QUEUES], *receivers[MAX_QUEUES];
  enum image_type_id_t type;
};
struct gif_receiver_t {
  unsigned char *gif;
  size_t        len, window_id;
};
struct png_receiver_t {
  unsigned char *png;
  size_t        len, window_id;
};
struct rgb_receiver_t {
  unsigned char *rgb;
  size_t        len, window_id, width, height;
};
struct cgimage_receiver_t {
  CGImageRef image_ref;
  size_t     window_id;
};
struct cgimage_args_t {
  size_t window_id;
  int    width, height;
};
char *get_capture_type_name(enum capture_type_id_t type);
bool inspect_frames(struct animated_capture_t *acap);
bool new_animated_frame(struct animated_capture_t *acap, struct capture_result_t *r);
int poll_new_animated_frame(void *VOID);
struct animated_capture_t *init_animated_capture(enum capture_type_id_t type, enum image_type_id_t format, size_t id, size_t ms_per_frame);
size_t animated_frames_len(struct animated_capture_t *acap);
bool end_animation(struct animated_capture_t *acap);
struct Vector *get_cap_providers(enum image_type_id_t type);
struct Vector *capture(struct capture_req_t *req);
struct animated_capture_t *init_animated_capture();
#endif