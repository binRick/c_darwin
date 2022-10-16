#pragma once
#ifndef CAPTURE_TYPE_C
#define CAPTURE_TYPE_C
#define LOCAL_DEBUG_MODE                     CAPTURE_TYPE_DEBUG_MODE
#define THUMBNAIL_WIDTH                      150
#define WRITE_FILE                           false
#define WRITE_THUMBNAIL                      false
#define MAX_QUALITY                          70
#define MIN_QUALITY                          20
#define BAR_TERMINAL_WIDTH_PERCENTAGE        .8
#define BAR_TERMINAL_WIDTH_PERCENTAGE        .8
#define BAR_MIN_WIDTH                        20
#define BAR_MAX_WIDTH_TERMINAL_PERCENTAGE    .5
#define BAR_MESSAGE_COMPRESSION_TEMPLATE     "Compressing %lu Images of %s"
#define BAR_MESSAGE_CAPTURE_TEMPLATE         "Capturing %lu %s %s"
#define BAR_MESSAGE_TEMPLATE                 "$E BOLD; $E%s: $E RGB(8, 104, 252); $E[$E RGB(8, 252, 104);UNDERLINE; $E$F'-'$F$E RGB(252, 8, 104); $E$N'-'$N$E RESET_UNDERLINE;RGB(8, 104, 252); $E] $E FG_RESET; $E$P%% $E RESET; $E"
////////////////////////////////////////////
#include "capture/type/type.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "c_workqueue/include/workqueue.h"
#include "capture/utils/utils.h"
#include "chan/src/chan.h"
#include "clamp/clamp.h"
#include "core/debug/debug.h"
#include "gifdec/gifdec.h"
#include "image/utils/utils.h"
#include "kitty/msg/msg.h"
#include "libimagequant/libimagequant.h"
#include "log/log.h"
#include "ms/ms.h"
#include "msf_gif/msf_gif.h"
#include "qoi/qoi.h"
#include "qoi_ci/QOI-stdio.h"
#include "qoi_ci/transpiled/QOI.h"
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"

#include "vips/vips.h"
#include "window/utils/utils.h"
#include <png.h>
#include <pthread.h>
#ifndef QTY
#define QTY(X)    (sizeof(X) / sizeof(X[0]))
#endif
#ifndef info
#define info    log_info
#endif
#ifdef LOCAL_DEBUG_MODE
#define debug(M, ...)    {            \
    do {                              \
      if (LOCAL_DEBUG_MODE) {         \
        log_debug(M, ## __VA_ARGS__); \
      }                               \
    } while (0); }
#endif
static struct Vector *get_cap_providers(enum image_type_id_t format);
static bool CAPTURE_TYPE_DEBUG_MODE = false;
static bool analyze_image_pixels(struct capture_image_result_t *r);
static bool init_capture_request_receiver(struct capture_image_request_t *req, struct cap_t *cap);
static int receive_requests_handler(void *CAP);
static bool issue_capture_image_request(struct capture_image_request_t *req, struct cap_t *cap);
static const struct cap_t *__caps[] = {
  [CAPTURE_CHAN_TYPE_CGIMAGE] = &(struct cap_t)          {
    .name          = "CGImage Capture",
    .enabled       = true, .format = IMAGE_TYPE_CGIMAGE,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_IDS,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct cgimage_recv_t *r = (struct cgimage_recv_t *)MSG;
      size_t id                = (size_t)vector_get(r->req->ids, (size_t)(r->index));
      debug("Capturing #%lu/%lu: (ID %lu)"
            "\n\t|Capture Type:" AC_YELLOW "%s" AC_RESETALL
            "\n\t|Capture Format:" AC_BLUE "%s" AC_RESETALL
            "\n\t|Capture Size Type:" AC_RED "%s" AC_RESETALL
            "\n\t|Capture Size:" AC_MAGENTA "%d" AC_RESETALL
            "%s",
            r->index + 1, vector_size(r->req->ids), id,
            get_capture_type_name(r->req->type),
            image_type_name(r->req->format),
            r->req->width > 0 ? "Width"
              : r->req->height > 0 ? "Height"
                : "None",
            r->req->width > 0 ? r->req->width
              : r->req->height > 0 ? r->req->height
                : 0,
            ""
            );
      r->img_ref          = NULL;
      r->time.started     = timestamp();
      r->time.captured_ts = r->time.started;
      if (r->req->width > 0)                             {
        r->img_ref = capture_type_width(r->req->type, id, r->req->width);
      }else if (r->req->height > 0)                      {
        r->img_ref = capture_type_height(r->req->type, id, r->req->height);
      }else                                              {
        r->img_ref = capture_type_capture(r->req->type, id);
      }
      r->width    = CGImageGetWidth(r->img_ref);
      r->height   = CGImageGetHeight(r->img_ref);
      r->time.dur = timestamp() - r->time.started;
      r->len      = r->width * r->height * CGImageGetBitsPerPixel(r->img_ref) / 8;
      debug(
        "Captured %lux%lu "
        "%s %s "
        "(ID %lu) "
        "in %s"
        "%s",
        r->width, r->height,
        get_capture_type_name(r->req->type), image_type_name(r->req->format),
        id,
        milliseconds_to_string(r->time.dur),
        ""
        );
      return((void *)r);
    },
  },
  [CAPTURE_CHAN_TYPE_RGB] = &(struct cap_t)              {
    .name          = "CGImage To RGB Pixels",
    .enabled       = true, .format = IMAGE_TYPE_RGB,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_image_result_t *r = calloc(1, sizeof(struct capture_image_result_t));
      r->msg              = (struct cgimage_recv_t *)MSG;
      r->len              = 0;
      r->time.captured_ts = r->msg->time.captured_ts;
      r->time.started     = timestamp();
      r->pixels           = save_cgref_to_rgb_memory(r->msg->img_ref, &(r->len));
      r->analyze          = true;
      r->time.dur         = timestamp() - r->time.started;
      assert(r->pixels != NULL);
      debug("Converted CGImageRef to %s RGB Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);
    },
  },
  [CAPTURE_CHAN_TYPE_QOI] = &(struct cap_t)              {
    .name          = "CGImage To QOI Pixels",
    .enabled       = true, .format = IMAGE_TYPE_QOI,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_image_result_t *r = calloc(1, sizeof(struct capture_image_result_t));
      r->format           = IMAGE_TYPE_QOI;
      r->msg              = (struct cgimage_recv_t *)MSG;
      r->time.captured_ts = r->msg->time.captured_ts;
      debug("Converting %lux%lu CGImageref to QOI", r->msg->width, r->msg->height);
      r->len          = 0;
      r->time.started = timestamp();
      r->pixels       = save_cgref_to_qoi_memory(r->msg->img_ref, &(r->len));
      r->time.dur     = timestamp() - r->time.started;
      r->analyze      = true;
      errno           = 0;
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze QOI");
        goto error;
      }
      debug("Converted CGImageRef to %lux%lu %s QOI Pixels in %s",
            r->width, r->height,
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_GIF] = &(struct cap_t)              {
    .name          = "CGImage To GIF Pixels",
    .enabled       = true, .format = IMAGE_TYPE_GIF,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_image_result_t *r = calloc(1, sizeof(struct capture_image_result_t));
      r->format = IMAGE_TYPE_GIF;
      r->msg    = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to GIF", r->msg->width, r->msg->height);
      r->len              = 0;
      r->time.started     = timestamp();
      r->time.captured_ts = r->msg->time.captured_ts;
      r->pixels           = save_cgref_to_gif_memory(r->msg->img_ref, &(r->len));
      r->time.dur         = timestamp() - r->time.started;
      r->analyze          = true;
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze GIF");
        goto error;
      }
      debug("Converted CGImageRef to %s GIF Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_TIFF] = &(struct cap_t)             {
    .name          = "CGImage To TIFF Pixels",
    .enabled       = true, .format = IMAGE_TYPE_TIFF,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_image_result_t *r = calloc(1, sizeof(struct capture_image_result_t));
      r->format = IMAGE_TYPE_TIFF;
      r->msg    = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to TIFF", r->msg->width, r->msg->height);
      r->len          = 0;
      r->time.started = timestamp();
      r->pixels       = save_cgref_to_tiff_memory(r->msg->img_ref, &(r->len));
      r->time.dur     = timestamp() - r->time.started;
      r->analyze      = true;
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze TIFF");
        goto error;
      }
      debug("Converted CGImageRef to %s TIFF Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_BMP] = &(struct cap_t)              {
    .name          = "CGImage To BMP Pixels",
    .enabled       = true, .format = IMAGE_TYPE_BMP,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_image_result_t *r = calloc(1, sizeof(struct capture_image_result_t));
      r->format = IMAGE_TYPE_BMP;
      r->msg    = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to BMP", r->msg->width, r->msg->height);
      r->time.captured_ts = r->msg->time.captured_ts;
      r->len              = 0;
      r->time.captured_ts = r->msg->time.captured_ts;
      r->time.started     = timestamp();
      r->pixels           = save_cgref_to_bmp_memory(r->msg->img_ref, &(r->len));
      r->time.dur         = timestamp() - r->time.started;
      r->analyze          = true;
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze BMP");
        goto error;
      }
      debug("Converted CGImageRef to %s BMP Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_WEBP] = &(struct cap_t)             {
    .name          = "CGImage To WEBP Pixels",
    .enabled       = true, .format = IMAGE_TYPE_WEBP,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_image_result_t *r = calloc(1, sizeof(struct capture_image_result_t));
      r->format = IMAGE_TYPE_WEBP;
      r->msg    = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to WEBP", r->msg->width, r->msg->height);
      r->time.captured_ts = r->msg->time.captured_ts;
      r->analyze          = true;
      r->len              = 0;
      r->time.started     = timestamp();
      r->pixels           = save_cgref_to_webp_memory(r->msg->img_ref, &(r->len));
      r->time.dur         = timestamp() - r->time.started;
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze WEBP");
        goto error;
      }
      debug("Converted CGImageRef to %s WEBP Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_JPEG] = &(struct cap_t)             {
    .name          = "CGImage To JPEG Pixels",
    .enabled       = true, .format = IMAGE_TYPE_JPEG,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_image_result_t *r = calloc(1, sizeof(struct capture_image_result_t));
      r->format = IMAGE_TYPE_JPEG;
      r->msg    = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to JPEG", r->msg->width, r->msg->height);
      r->time.captured_ts = r->msg->time.captured_ts;
      r->analyze          = true;
      r->len              = 0;
      r->time.started     = timestamp();
      r->pixels           = save_cgref_to_jpeg_memory(r->msg->img_ref, &(r->len));
      r->time.dur         = timestamp() - r->time.started;
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze JPEG");
        goto error;
      }
      debug("Converted CGImageRef to %s JPEG Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_PNG] = &(struct cap_t)              {
    .name          = "CGImage To PNG Pixels",
    .enabled       = true, .format = IMAGE_TYPE_PNG,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_image_result_t *r = calloc(1, sizeof(struct capture_image_result_t));
      r->analyze          = true;
      r->format           = IMAGE_TYPE_PNG;
      r->msg              = (struct cgimage_recv_t *)MSG;
      r->time.captured_ts = r->msg->time.captured_ts;
      r->len              = 0;
      r->time.started     = timestamp();
      errno               = 0;
      r->pixels           = save_cgref_to_png_memory(r->msg->img_ref, &(r->len));
      if (!r->pixels || r->len < 1)                      {
        log_error("Failed to convert CGRef to PNG Pixels");
      }else                                              {
        errno = 0;
        if (!analyze_image_pixels(r))                    {
          log_error("Failed to analyze PNG");
        }else                                            {
          debug("Converted CGImageRef to %s %lux%lu %s Pixels in %s",
                bytes_to_string(r->len),
                r->width, r->height, image_type_name(r->format),
                milliseconds_to_string(r->time.dur)
                );
        }
      }
      return((void *)r);
    },
  },
};
static struct cap_t       **caps = { NULL };

char *get_capture_type_name(enum capture_type_id_t type){
  switch (type) {
  case CAPTURE_TYPE_WINDOW: return("window");

  case CAPTURE_TYPE_DISPLAY: return("display");

  case CAPTURE_TYPE_SPACE: return("space");

  default: return("unknown");
  }
}
static struct Vector *get_cap_providers(enum image_type_id_t format){
  unsigned long            started = timestamp();
  struct Vector            *v      = vector_new();
  enum capture_chan_type_t t       = -1;

  for (size_t i = 1; i < CAPTURE_CHAN_TYPES_QTY; i++) {
    if (caps[i]) {
      if (caps[i]->format == format) {
        t = i;
        break;
      }
    }
  }
  if ((int)t == -1) {
    log_error("Unable to find capture type for format %s (%d)", image_type_name(format), format);
    exit(EXIT_FAILURE);
  }
  while (true) {
    if (caps[t]) {
      vector_prepend(v, (void *)(size_t)(t));
    }
    if (caps[t]->provider_type == CAPTURE_PROVIDER_TYPE_IDS) {
      break;
    }
    t = caps[t]->provider;
  }
  debug(
    "Found " AC_YELLOW "%lu" AC_RESETALL
    " Capture Providers for Capture Format " AC_YELLOW "%s" AC_RESETALL
    " in " AC_YELLOW "%s" AC_RESETALL
    "%s",
    vector_size(v),
    image_type_name(format),
    milliseconds_to_string(timestamp() - started),
    ""
    );
  return(v);
}

static bool init_capture_request_receiver(struct capture_image_request_t *req, struct cap_t *cap){
  for (int i = 0; i < req->concurrency; i++) {
    if (!cap->threads[i]) {
      cap->threads[i] = calloc(1, sizeof(pthread_t));
    }
    assert(cap->threads[i] != NULL);
  }
  if (!cap->recv_chan) {
    cap->recv_chan = chan_init(vector_size(req->ids) * 2);
  }
  if (!cap->send_chan) {
    cap->send_chan = chan_init(vector_size(req->ids) * 2);
  }
  if (!cap->done_chan) {
    cap->done_chan = chan_init(req->concurrency);
  }
  cap->qty       = vector_size(req->ids) / req->concurrency;
  cap->total_qty = vector_size(req->ids);
  assert(cap->send_chan != NULL);
  assert(cap->recv_chan != NULL);
  assert(cap->done_chan != NULL);
  assert(cap->qty > 0);
  return(true);
}

static int receive_requests_handler(void *CAP){
  unsigned long started = timestamp();
  struct cap_t  *cap    = (struct cap_t *)CAP;
  void          *msg;
  size_t        qty = 0;

  while (chan_recv(cap->recv_chan, &msg) == 0) {
    qty++;
    debug("%s> Received Capture Request #%lu/%lu.", cap->name, qty, cap->qty);
    void *m = cap->recv_msg(msg);
    if (!m) {
      log_error("%s> Failed to format message", cap->name);
      return(EXIT_FAILURE);
    }
    if (cap->bar) {
      cap->bar->progress += (float)((float)1 / (float)(cap->total_qty));
      cbar_display_bar(cap->bar);
    }
    chan_send(cap->send_chan, m);
  }
  debug(
    AC_YELLOW "%s" AC_RESETALL " Receiver complete. Processed %lu items in %s"
    "%s",
    cap->name,
    qty,
    milliseconds_to_string(timestamp() - started),
    ""
    );
  chan_send(cap->done_chan, (void *)0);
  return(EXIT_SUCCESS);
}

static struct chan_t *compression_wait_chan;
static struct chan_t *compression_done_chan;
static struct chan_t *compression_results_chan;

static int wait_recv_compress_done(void __attribute__((unused)) *REQ){
  struct capture_image_request_t *req = (struct capture_image_request_t *)REQ;
  void                           *msg;
  size_t                         qty = 0, qqty = 0;

  debug("compression recv waiting");
  struct Vector *compressed_results = vector_new();

  while (vector_size(compressed_results) < vector_size(req->ids) && chan_recv(compression_wait_chan, &msg) == 0) {
    qty++;
    if (req->bar) {
      req->bar->progress += (float)((float)1 / (float)(vector_size(req->ids)));
      cbar_display_bar(req->bar);
    }
    vector_push(compressed_results, msg);
  }
  while (qqty < req->concurrency && chan_recv(compression_done_chan, &msg) == 0) {
    qqty++;
  }
  chan_send(compression_results_chan, (void *)compressed_results);
}

static int run_compress_recv(void __attribute__((unused)) *CHAN){
  struct chan_t *chan = (struct chan_t *)CHAN;
  void          *msg;
  char          *buf;
  size_t        blen = 0;
  VipsImage     *v   = NULL;
  size_t        qty  = 0;

  while (chan_recv(chan, &msg) == 0) {
    struct compress_t *c = (struct compress_t *)msg;
    qty++;
    c->started = timestamp();
    debug("Run Compress Recv:   type:%d|%s|PNG:%d|%s", c->format, image_type_name(c->format), IMAGE_TYPE_PNG, image_type_name(IMAGE_TYPE_PNG));
    switch (c->format) {
    case IMAGE_TYPE_WEBP:
      v = vips_image_new_from_buffer(c->pixels, c->len, "", NULL);
      if (v) {
        vips_pngsave_buffer(v, &buf, &blen, "Q", 100, NULL);
      }
      break;
    case IMAGE_TYPE_PNG:
      errno = 0;
      v     = vips_image_new_from_buffer(c->pixels, c->len, "", NULL);
      if (v) {
        vips_pngsave_buffer(v, &buf, &blen, "compression", 9, NULL);
      }
      errno = 0;
      if (c->quantize_mode && !compress_png_buffer(buf, &blen)) {
        log_error("Failed to compress png buffer");
      }

      break;
    case IMAGE_TYPE_TIFF:
      v = vips_image_new_from_buffer(c->pixels, c->len, "", NULL);
      if (v) {
        if (vips_tiffsave_buffer(v, &buf, &blen, "compression", VIPS_FOREIGN_TIFF_COMPRESSION_JPEG, "tile", true, "pyramid", true, NULL)) {
          log_error("failed to save buffer");
        }
      }
      break;
    default:
      errno = 0;
      if (CAPTURE_TYPE_DEBUG_MODE) {
        log_warn("\nCompression not implemented for image type %s.", image_type_name(c->format));
      }
      break;
    }
    char *m;
    if (v && blen > 0 && buf && blen < c->len) {
      free(c->pixels);
      asprintf(&m, "\n✅ Compressed"
               " "
               AC_BLUE "%4s"AC_RESETALL
               " "
               AC_GREEN "%6s"AC_RESETALL
               " "
               "File of"
               " "
               "to"
               " "
               AC_DOTTED_UNDERLINE AC_ITALIC AC_YELLOW "%s" AC_RESETALL
               " "
               "in"
               " "
               AC_BOLD AC_YELLOW "%6s"AC_RESETALL
               "",
               image_type_name(c->format),
               bytes_to_string(c->len),
               bytes_to_string(blen),
               milliseconds_to_string(timestamp() - c->started)
               );
      fflush(stdout);
      printf(
        "\t%s\n",
        m
        );
      fflush(stdout);
      c->pixels = buf;
      c->len    = blen;
    }
    if (v) {
      g_object_unref(v);
    }
    c->dur = timestamp() - c->started;
    chan_send(compression_wait_chan, (void *)c);
    debug("Compressed #%lu from %s to %s in %s",
          c->id,
          bytes_to_string(c->prev_len),
          bytes_to_string(c->len),
          milliseconds_to_string(c->dur)
          );
  }
  chan_send(compression_done_chan, (void *)0);
  return(EXIT_SUCCESS);
} /* run_compress_recv */

static bool start_com(struct capture_image_request_t *req){
  if (req->compress) {
    for (int i = 0; i < req->concurrency; i++) {
      debug("%d", req->compress);
      int q = i % req->concurrency;
      if (pthread_create(req->comp->threads[i], NULL, run_compress_recv, (void *)(req->comp->chans[q])) != 0) {
        log_error("Failed to created Compression Thread #%d", i);
        return(false);
      }
    }
    if (pthread_create(req->comp->waiter, NULL, wait_recv_compress_done, (void *)req) != 0) {
      log_error("Failed to created Waiter Thread");
      return(false);
    }
  }
  return(true);
}

static bool issue_capture_image_request(struct capture_image_request_t *req, struct cap_t *cap){
  for (int i = 0; i < req->concurrency; i++) {
    if (pthread_create(cap->threads[i], NULL, receive_requests_handler, (void *)cap) != 0) {
      log_error("Failed to created Thread %s #%d", cap->name, i);
      return(false);
    }
  }
  return(true);
}

static bool analyze_image_pixels(struct capture_image_result_t *r){
  r->id = (size_t)vector_get(r->msg->req->ids, (size_t)(r->msg->index));
  if (r->analyze) {
    if (r->format == IMAGE_TYPE_QOI) {
      QOIDecoder *qoi = QOIDecoder_New();
      if (QOIDecoder_Decode(qoi, r->pixels, r->len)) {
        r->width             = QOIDecoder_GetWidth(qoi);
        r->height            = QOIDecoder_GetHeight(qoi);
        r->has_alpha         = QOIDecoder_HasAlpha(qoi);
        r->linear_colorspace = QOIDecoder_IsLinearColorspace(qoi);
      }
      if (qoi) {
        QOIDecoder_Delete(qoi);
      }
    }else{
      VipsImage *image = NULL;
      errno = 0;
      if (!(image = vips_image_new_from_buffer(r->pixels, r->len, "", "access", VIPS_ACCESS_SEQUENTIAL, NULL))) {
        log_error("Failed to decode pixels for Item #%lu", r->id);
        return(false);
      }
      r->width             = vips_image_get_width(image);
      r->height            = vips_image_get_height(image);
      r->has_alpha         = vips_image_hasalpha(image);
      r->linear_colorspace = vips_colourspace_issupported(image);
      if (image) {
        g_object_unref(image);
      }
    }
  }
  return(true);
} /* analyze_image_pixels */

static bool wait_cap(struct capture_image_request_t *req, struct cap_t *cap){
  for (int i = 0; i < req->concurrency; i++) {
    pthread_join(cap->threads[i], NULL);
  }
  return(true);
}

struct Vector *capture_image(struct capture_image_request_t *req){
  struct Vector *capture_results_v = vector_new();
  char          *bar_msg;
  size_t        bar_msg_len, bar_len;
  int           term_width          = 80;
  chan_t        *results_chan       = chan_init(vector_size(req->ids));
  struct cap_t  *_caps[QTY(__caps)] = { 0 };

  memcpy(_caps, __caps, sizeof(__caps));
  caps = _caps;

  if (req->progress_bar_mode) {
    req->bar = calloc(1, sizeof(struct cbar_t));
    asprintf(&bar_msg,
             BAR_MESSAGE_CAPTURE_TEMPLATE,
             vector_size(req->ids), image_type_name(req->format),
             get_capture_type_name(req->type)
             );
    bar_msg_len = strlen(bar_msg);
    term_width  = clamp(get_terminal_width(), 40, 160);
    asprintf(&bar_msg, BAR_MESSAGE_TEMPLATE, bar_msg);
    bar_len     = term_width * BAR_TERMINAL_WIDTH_PERCENTAGE - bar_msg_len;
    *(req->bar) = cbar(clamp(bar_len, BAR_MIN_WIDTH, term_width * BAR_MAX_WIDTH_TERMINAL_PERCENTAGE), bar_msg);
    cbar_hide_cursor();
    req->bar->progress = 0.00;
    cbar_display_bar(req->bar);
    fflush(stdout);
  }

  struct Vector *providers = get_cap_providers(req->format);

  for (size_t i = 0; i < vector_size(providers); i++) {
    enum capture_chan_type_t c = (enum capture_chan_type_t)(size_t)vector_get(providers, i);
    if (i > 0) {
      enum capture_chan_type_t prev_c = (enum capture_chan_type_t)(size_t)vector_get(providers, i - 1);
      caps[c]->recv_chan = caps[prev_c]->send_chan;
      if (req->progress_bar_mode) {
        caps[c]->bar = req->bar;
      }
    }
    if (i == vector_size(providers) - 1) {
      caps[c]->send_chan = results_chan;
    }
    if (!init_capture_request_receiver(req, caps[c])) {
      log_error("Failed to initialize capture #%d: %s", c, caps[c]->name);
      goto done;
    }
  }
  for (size_t i = 0; i < vector_size(providers); i++) {
    enum capture_chan_type_t c = (enum capture_chan_type_t)(size_t)vector_get(providers, i);
    if (!issue_capture_image_request(req, caps[c])) {
      log_error("Failed to start capture #%d: %s", c, caps[c]->name);
      goto done;
    }
  }
  for (size_t i = 0; i < vector_size(providers); i++) {
    enum capture_chan_type_t c = (enum capture_chan_type_t)(size_t)vector_get(providers, i);
    if (caps[c]->provider_type == CAPTURE_PROVIDER_TYPE_IDS) {
      for (size_t i = 0; i < vector_size(req->ids); i++) {
        struct cgimage_recv_t *r = calloc(1, sizeof(struct cgimage_recv_t));
        r->index  = i;
        r->req    = req;
        r->width  = req->width;
        r->height = req->height;
        chan_send(caps[c]->recv_chan, (void *)r);
      }
      chan_close(caps[c]->recv_chan);
      size_t completed = 0;
      for (int i = 0; i < req->concurrency; i++) {
        chan_recv(caps[c]->done_chan, (void *)0);
        completed++;
      }
      chan_close(caps[c]->send_chan);
    }
    if (!wait_cap(req, caps[c])) {
      log_error("Failed to wait capture #%d: %s", c, caps[c]->name);
      goto done;
    }
  }
  struct capture_image_result_t *r;
  size_t                        captured_images_len = 0;

  for (size_t i = 0; i < vector_size(req->ids); i++) {
    void *msg;
    if (chan_recv(results_chan, &msg) == 0) {
      r                    = (struct capture_image_result_t *)msg;
      captured_images_len += r->len;
      debug("Received Result #%lu/%lu | %lux%lu | %s | %s | %s | Linear Colorspace: %s | Has Alpha: %s |",
            i + 1, vector_size(req->ids),
            r->width, r->height, bytes_to_string(r->len), milliseconds_to_string(r->time.dur),
            r->file,
            r->linear_colorspace?"Yes":"No",
            r->has_alpha?"Yes":"No"
            );
      r->time.captured_ts = timestamp();
      msg                 = (void *)r;
      vector_push(capture_results_v, msg);
    }
  }
  for (size_t i = 0; i < vector_size(providers); i++) {
    enum capture_chan_type_t c = (enum capture_chan_type_t)(size_t)vector_get(providers, i);
    if (chan_is_closed(results_chan) == 0) {
      chan_close(results_chan);
    }
    if (chan_is_closed(caps[c]->done_chan) == 0) {
      chan_close(caps[c]->done_chan);
    }
    if (chan_is_closed(caps[c]->send_chan) == 0) {
      chan_close(caps[c]->send_chan);
    }
    if (chan_is_closed(caps[c]->recv_chan) == 0) {
      chan_close(caps[c]->recv_chan);
    }
    chan_dispose(caps[c]->recv_chan);
    chan_dispose(caps[c]->done_chan);
    caps[c]->recv_chan = NULL;
    caps[c]->send_chan = NULL;
    caps[c]->done_chan = NULL;
    for (int x = 0; x < req->concurrency; x++) {
      if (caps[c]->threads[x]) {
        //free(caps[c]->threads[x]);
      }
    }
  }
  chan_dispose(results_chan);
  results_chan = NULL;
  if (req->progress_bar_mode) {
    req->bar->progress = (float)1.00;
    cbar_display_bar(req->bar);
    cbar_show_cursor();
    printf(
      "\n"
      );
    fflush(stdout);
    if (req->bar) {
      free(req->bar);
    }
  }

  if (req->compress) {
    void              *compressed_images_msg;
    struct Vector     *compressed_images_msg_v = NULL, *compressed_images_v = vector_new();
    struct compress_t *compressed_image_msg;
    if (req->progress_bar_mode) {
      req->bar = calloc(1, sizeof(struct cbar_t));
      asprintf(&bar_msg, BAR_MESSAGE_COMPRESSION_TEMPLATE, vector_size(req->ids), bytes_to_string(captured_images_len));
      bar_msg_len = strlen(bar_msg);
      asprintf(&bar_msg, BAR_MESSAGE_TEMPLATE, bar_msg);
      bar_len     = term_width * BAR_TERMINAL_WIDTH_PERCENTAGE - bar_msg_len;
      *(req->bar) = cbar(clamp(bar_len, BAR_MIN_WIDTH, term_width * BAR_MAX_WIDTH_TERMINAL_PERCENTAGE), bar_msg);
      cbar_hide_cursor();
      req->bar->progress = 0.00;
      cbar_display_bar(req->bar);
    }
    req->comp = calloc(1, sizeof(struct compress_req_t));
    for (int i = 0; i < req->concurrency; i++) {
      req->comp->chans[i]   = chan_init(vector_size(req->ids));
      req->comp->threads[i] = calloc(1, sizeof(pthread_t));
    }
    req->comp->waiter        = calloc(1, sizeof(pthread_t));
    req->comp->waiter_chan   = chan_init(vector_size(req->ids));
    compression_wait_chan    = chan_init(vector_size(req->ids));
    compression_done_chan    = chan_init(0);
    compression_results_chan = chan_init(0);
    if (req->progress_bar_mode) {
      req->comp->bar = calloc(1, sizeof(struct cbar_t));
    }
    if (!start_com(req)) {
      log_error("Failed to start compressions");
      goto done;
    }
    for (size_t i = 0; i < vector_size(capture_results_v); i++) {
      struct capture_image_result_t *r = (struct capture_image_result_t *)vector_get(capture_results_v, i);
      struct compress_t             *c = calloc(1, sizeof(struct compress_t));
      c->id             = r->id;
      c->pixels         = r->pixels;
      c->len            = r->len;
      c->prev_len       = r->len;
      c->format         = r->format;
      c->bar            = req->bar;
      c->max_quality    = MAX_QUALITY;
      c->min_quality    = MIN_QUALITY;
      c->quantize_mode  = req->quantize_mode;
      c->capture_result = r;
      chan_send(req->comp->chans[i % req->concurrency], (void *)c);
    }
    for (int x = 0; x < req->concurrency; x++) {
      chan_close(req->comp->chans[x]);
    }
    chan_recv(compression_results_chan, &compressed_images_msg);
    compressed_images_msg_v = (struct Vector *)compressed_images_msg;
    debug("\treceived compression results with %lu items", vector_size(compressed_images_msg_v));
    chan_close(compression_done_chan);
    chan_close(compression_wait_chan);

    for (size_t i = 0; i < vector_size(compressed_images_msg_v); i++) {
      compressed_image_msg = (struct compress_t *)vector_get(compressed_images_msg_v, i);
      debug("new vector item #%lu/%lu has %s image from %s image of type %d|%s",
            i + 1, vector_size(compressed_images_msg_v),
            bytes_to_string(compressed_image_msg->len),
            bytes_to_string(compressed_image_msg->prev_len),
            compressed_image_msg->format,
            image_type_name(compressed_image_msg->format)
            );
      struct capture_image_result_t *r = compressed_image_msg->capture_result;
      r->pixels = compressed_image_msg->pixels;
      r->len    = compressed_image_msg->len;
      debug("new vector item #%lu/%lu with id %lu %s file %s has %s image of type %d|%s",
            i + 1, vector_size(compressed_images_msg_v),
            r->id,
            bytes_to_string(fsio_file_size(r->file)),
            r->file,
            bytes_to_string(r->len),
            r->type,
            image_type_name(r->format)
            );
      vector_push(compressed_images_v, r);
    }
    capture_results_v = compressed_images_v;
    debug("compressions done");
    if (req->progress_bar_mode) {
      req->bar->progress = (float)1.00;
      cbar_display_bar(req->bar);
      cbar_show_cursor();
      printf(
        "\n"
        );
      fflush(stdout);
      if (req->bar) {
        free(req->bar);
      }
    }
  }
done:
  caps = NULL;
  return(capture_results_v);
} /* capture*/

struct Vector *db_tables_images(enum capture_type_id_t type, struct Vector *ids){
  struct capture_image_request_t *req = calloc(1, sizeof(struct capture_image_request_t));

  req->ids               = ids;
  req->format            = IMAGE_TYPE_QOI;
  req->compress          = false;
  req->quantize_mode     = false;
  req->type              = type;
  req->progress_bar_mode = false;
  req->width             = 300;
  req->height            = 0;
  req->time.dur          = 0;
  req->time.started      = timestamp();
  Dbg(vector_size(req->ids), %u);
  return(capture_image(req));
}

///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__capture_window(void){
  //setenv("VIPS_WARNING", "1", 1);
  if (getenv("DEBUG") != NULL || getenv("CAPTURE_TYPE_DEBUG_MODE") != NULL) {
    log_debug("Enabling capture_window Debug Mode");
    CAPTURE_TYPE_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#undef debug
#endif
