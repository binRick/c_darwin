#pragma once
#ifndef CAPTURE_WINDOW_C
#define CAPTURE_WINDOW_C
#define LOCAL_DEBUG_MODE    CAPTURE_WINDOW_DEBUG_MODE
#define THUMBNAIL_WIDTH     150
#define WRITE_FILE          true
#define WRITE_THUMBNAIL     false
#define MAX_QUALITY    70
#define MIN_QUALITY    20
#define BAR_TERMINAL_WIDTH_PERCENTAGE .8
#define BAR_MIN_WIDTH 20
#define BAR_MAX_WIDTH_TERMINAL_PERCENTAGE .5
#define BAR_MESSAGE_COMPRESSION_TEMPLATE "Compressing %lu Images of %s"
#define BAR_MESSAGE_CAPTURE_TEMPLATE "Capturing %lu Windows"
#define BAR_MESSAGE_TEMPLATE "$E BOLD; $E%s: $E RGB(8, 104, 252); $E[$E RGB(8, 252, 104);UNDERLINE; $E$F'-'$F$E RGB(252, 8, 104); $E$N'-'$N$E RESET_UNDERLINE;RGB(8, 104, 252); $E] $E FG_RESET; $E$P%% $E RESET; $E"
////////////////////////////////////////////
#include "capture/window/window.h"
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
#include "timg/utils/utils.h"
#include "vips/vips.h"
#include "window/utils/utils.h"
#include <png.h>
#include <pthread.h>
#define info    log_info
#ifdef LOCAL_DEBUG_MODE
#define debug(M, ...)    {            \
    do {                              \
      if (LOCAL_DEBUG_MODE) {         \
        log_debug(M, ## __VA_ARGS__); \
      }                               \
    } while (0); }
#endif
static bool CAPTURE_WINDOW_DEBUG_MODE = false;
#define QTY(X)           (sizeof(X) / sizeof(X[0]))
void cgimage_provider(void *ARGS);
void cgimage_receiver(void *ARGS);
void rgb_receiver(void *ARGS);
void gif_receiver(void *ARGS);
void png_receiver(void *ARGS);
static bool analyze_image_pixels(struct capture_result_t *r);
static char *get_image_type_filename(enum capture_type_id_t capture_type, enum image_type_id_t format, size_t id, bool thumbnail);
static const struct cap_t *__caps[] = {
  [CAPTURE_CHAN_TYPE_CGIMAGE] = &(struct cap_t)          {
    .name          = "CGImage Capture",
    .enabled       = true, .format = IMAGE_TYPE_CGIMAGE,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_IDS,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct cgimage_recv_t *r = (struct cgimage_recv_t *)MSG;
      size_t window_id         = (size_t)vector_get(r->req->ids, (size_t)(r->index));
      debug("Capturing #%lu/%lu: %lu |type:%d|format:%d|w:%d|h:%d|",
            r->index, vector_size(r->req->ids), window_id,
            r->req->type, r->req->format,
            r->req->width, r->req->height
            );
      r->img_ref          = NULL;
      r->time.started     = timestamp();
      r->time.captured_ts = r->time.started;
      if (r->req->width > 0)                             {
        r->img_ref = capture_type_width(r->req->type, window_id, r->req->width);
      }else if (r->req->height > 0)                      {
        r->img_ref = capture_type_height(r->req->type, window_id, r->req->height);
      }else                                              {
        r->img_ref = capture_type_capture(r->req->type, window_id);
      }
      debug("got img ref");
      r->width    = CGImageGetWidth(r->img_ref);
      r->height   = CGImageGetHeight(r->img_ref);
      r->time.dur = timestamp() - r->time.started;

      /*
       * size_t rgb_len;
       * unsigned long s = timestamp();
       * unsigned char *rgb = save_cgref_to_rgb_memory(r->img_ref,&rgb_len);
       * assert(rgb != NULL);
       * log_info("Allocated %s RGB Pixels in %s",
       * bytes_to_string(rgb_len),
       * milliseconds_to_string(timestamp() - s)
       * );
       * int w,h,f;
       * unsigned char *rgb_pixels1 = stbi_load_from_memory(rgb, rgb_len, &w, &h, &f, STBI_rgb_alpha);
       * assert(rgb_pixels1 != NULL);
       * log_info("Loaded %s RGB Pixels %dx%d|%d",
       *    bytes_to_string(r->len),
       *    w,h,f
       *    );
       */

      debug("Captured %lux%lu Window #%lu in %s", r->width, r->height, window_id, milliseconds_to_string(r->time.dur));
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
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->msg              = (struct cgimage_recv_t *)MSG;
      r->len              = 0;
      r->time.captured_ts = r->msg->time.captured_ts;
      r->time.started     = timestamp();
      r->pixels           = save_cgref_to_rgb_memory(r->msg->img_ref, &(r->len));
      r->analyze          = true;
      r->time.dur         = timestamp() - r->time.started;
      assert(r->pixels != NULL);

      //if (!analyze_image_pixels(r))                   {
      //  log_error("Failed to analyze RGB");
      //}
      log_info("Converted CGImageRef to %s RGB Pixels in %s",
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
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type             = IMAGE_TYPE_QOI;
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
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_GIF;
      r->msg  = (struct cgimage_recv_t *)MSG;
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
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_TIFF;
      r->msg  = (struct cgimage_recv_t *)MSG;
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
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_BMP;
      r->msg  = (struct cgimage_recv_t *)MSG;
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
  [CAPTURE_CHAN_TYPE_CGIMAGE_GRAYSCALE] = &(struct cap_t){
    .name          = "CGImage To Grayscale CGImage",
    .enabled       = true, .format = IMAGE_TYPE_CGIMAGE_GRAYSCALE,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct cgimage_recv_t *c   = calloc(1, sizeof(struct cgimage_recv_t));
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->msg = (struct capture_result_t *)MSG;
      debug("Converting %lux%lu CGImageref to TIFF Grayscale", r->width, r->height);
      r->len          = 0;
      c->time.started = timestamp();
      c->img_ref      = cgimageref_to_grayscale(r->msg->img_ref);
      c->time.dur     = timestamp() - r->time.started;
      c->width        = r->msg->width = CGImageGetWidth(r->msg->img_ref);
      c->height       = r->msg->height = CGImageGetWidth(r->msg->img_ref);
      c->req          = r->msg->req;
      debug("Converted CGImageRef to %lux%lu Grayscale CGImage in %s",
            c->width, c->height,
            milliseconds_to_string(c->time.dur)
            );

      return((void *)c);
    },
  },
  [CAPTURE_CHAN_TYPE_BMP_GRAYSCALE] = &(struct cap_t)    {
    .name          = "CGImage To BMP Grayscale Pixels",
    .enabled       = true, .format = IMAGE_TYPE_BMP_GRAYSCALE,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE_GRAYSCALE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_BMP_GRAYSCALE;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to BMP Grayscale", r->msg->width, r->msg->height);
      r->time.captured_ts = r->msg->time.captured_ts;
      r->len              = 0;
      r->time.started     = timestamp();
      r->pixels           = save_cgref_to_bmp_memory(r->msg->img_ref, &(r->len));
      r->time.dur         = timestamp() - r->time.started;
      r->analyze          = true;
      r->msg->req         = r->msg->req;
      debug("Converted CGImageRef to %s Grayscale BMP in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze BMP");
        goto error;
      }
      debug("Converted CGImageRef to %s BMP Grayscale Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_QOI_GRAYSCALE] = &(struct cap_t)    {
    .name          = "CGImage To QOI Grayscale Pixels",
    .enabled       = true, .format = IMAGE_TYPE_QOI_GRAYSCALE,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE_GRAYSCALE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_QOI_GRAYSCALE;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to QOI Grayscale", r->msg->width, r->msg->height);
      r->time.captured_ts = r->msg->time.captured_ts;
      r->len              = 0;
      r->time.started     = timestamp();
      r->pixels           = save_cgref_to_qoi_memory(r->msg->img_ref, &(r->len));
      r->time.dur         = timestamp() - r->time.started;
      r->analyze          = true;
      r->analyze          = true;
      r->msg->req         = r->msg->req;
      debug("Converted CGImageRef to %s Grayscale QOI in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze QOI");
        goto error;
      }
      debug("Converted CGImageRef to %s QOI Grayscale Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_JPEG_GRAYSCALE] = &(struct cap_t)   {
    .name          = "CGImage To JPEG Grayscale Pixels",
    .enabled       = true, .format = IMAGE_TYPE_JPEG_GRAYSCALE,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE_GRAYSCALE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_JPEG_GRAYSCALE;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to JPEG Grayscale", r->msg->width, r->msg->height);
      r->time.captured_ts = r->msg->time.captured_ts;
      r->len              = 0;
      r->time.started     = timestamp();
      r->pixels           = save_cgref_to_jpeg_memory(r->msg->img_ref, &(r->len));
      r->time.dur         = timestamp() - r->time.started;
      r->analyze          = true;
      r->msg->req         = r->msg->req;
      debug("Converted CGImageRef to %s Grayscale JPEG in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze JPEG");
        goto error;
      }
      debug("Converted CGImageRef to %s JPEG Grayscale Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_PNG_GRAYSCALE] = &(struct cap_t)    {
    .name          = "CGImage To PNG Grayscale Pixels",
    .enabled       = true, .format = IMAGE_TYPE_PNG_GRAYSCALE,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE_GRAYSCALE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_PNG_GRAYSCALE;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to PNG Grayscale", r->msg->width, r->msg->height);
      r->time.captured_ts = r->msg->time.captured_ts;
      r->len              = 0;
      r->time.started     = timestamp();
      r->pixels           = save_cgref_to_png_memory(r->msg->img_ref, &(r->len));
      r->time.dur         = timestamp() - r->time.started;
      r->analyze          = true;
      r->msg->req         = r->msg->req;
      debug("Converted CGImageRef to %s Grayscale PNG in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze PNG");
        goto error;
      }
      debug("Converted CGImageRef to %s PNG Grayscale Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_GIF_GRAYSCALE] = &(struct cap_t)    {
    .name          = "CGImage To GIF Grayscale Pixels",
    .enabled       = true, .format = IMAGE_TYPE_GIF_GRAYSCALE,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE_GRAYSCALE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_GIF_GRAYSCALE;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to GIF Grayscale", r->msg->width, r->msg->height);
      r->time.captured_ts = r->msg->time.captured_ts;
      r->len              = 0;
      r->time.started     = timestamp();
      r->pixels           = save_cgref_to_gif_memory(r->msg->img_ref, &(r->len));
      r->time.dur         = timestamp() - r->time.started;
      r->analyze          = true;
      r->msg->req         = r->msg->req;
      debug("Converted CGImageRef to %s Grayscale GIF in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze GIF");
        goto error;
      }
      debug("Converted CGImageRef to %s GIF Grayscale Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_TIFF_GRAYSCALE] = &(struct cap_t)   {
    .name          = "CGImage To TIFF Grayscale Pixels",
    .enabled       = true, .format = IMAGE_TYPE_TIFF_GRAYSCALE,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE_GRAYSCALE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_TIFF_GRAYSCALE;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to TIFF Grayscale", r->msg->width, r->msg->height);
      r->time.captured_ts = r->msg->time.captured_ts;
      r->len              = 0;
      r->time.started     = timestamp();
      r->pixels           = save_cgref_to_tiff_memory(r->msg->img_ref, &(r->len));
      r->time.dur         = timestamp() - r->time.started;
      r->analyze          = true;
      r->msg->req         = r->msg->req;
      debug("Converted CGImageRef to %s Grayscale Tiff in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze TIFF");
        goto error;
      }
      debug("Converted CGImageRef to %s TIFF Grayscale Pixels in %s",
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
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_JPEG;
      r->msg  = (struct cgimage_recv_t *)MSG;
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
  [CAPTURE_CHAN_TYPE_PNG_COMPRESSED] = &(struct cap_t)   {
    .name          = "CGImage To PNG Pixels",
    .enabled       = true, .format = IMAGE_TYPE_PNG_COMPRESSED,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)                 {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_PNG_COMPRESSED;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to PNG", r->msg->width, r->msg->height);
      char *s;
      r->time.captured_ts = r->msg->time.captured_ts;
      size_t window_id = (size_t)vector_get(r->msg->req->ids, (size_t)(r->msg->index));
      asprintf(&s, "%sWindow-comp-%lu.png", gettempdir(), window_id);
      save_cgref_to_image_type_file(IMAGE_TYPE_PNG, r->msg->img_ref, s);
      r->len          = 0;
      r->analyze      = true;
      r->time.started = timestamp();

      size_t rgb_len            = 0;
      unsigned char *rgb_pixels = save_cgref_to_rgb_memory(r->msg->img_ref, &rgb_len);
      r->pixels = imagequant_encode_rgb_pixels_to_png_buffer(rgb_pixels, r->msg->width, r->msg->height, 10, 70, &(r->len));
      free(rgb_pixels);
      r->time.dur = timestamp() - r->time.started;
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze PNG");
        goto error;
      }
      debug("Converted CGImageRef to %s PNG Pixels in %s",
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
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->analyze  = true;
      r->type             = IMAGE_TYPE_PNG;
      r->msg              = (struct cgimage_recv_t *)MSG;
      r->time.captured_ts = r->msg->time.captured_ts;
      debug("Converting %lux%lu CGImageref to PNG", r->msg->width, r->msg->height);
      r->len          = 0;
      r->time.started = timestamp();
      errno=0;
      r->pixels = save_cgref_to_png_memory(r->msg->img_ref, &(r->len));
      if (!analyze_image_pixels(r))                      {
        log_error("Failed to analyze PNG");
      }
      debug("Converted CGImageRef to %s %lux%lu %s Pixels in %s",
            bytes_to_string(r->len),
            r->width,r->height,image_type_name(r->type),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);
    },
  },
};
static struct cap_t       **caps = { NULL };

struct Vector *get_cap_providers(enum image_type_id_t format){
  struct Vector            *v = vector_new();
  enum capture_chan_type_t t  = -1;

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
  debug("Found capture #%d", t);
  while (true) {
    debug("Provider: %d", caps[t]->provider);
    if (caps[t]) {
      vector_prepend(v, (void *)(size_t)(t));
    }
    if (caps[t]->provider_type == CAPTURE_PROVIDER_TYPE_IDS) {
      debug("breaking");
      break;
    }
    t = caps[t]->provider;
  }
  return(v);
}

static bool init_cap(struct capture_req_t *req, struct cap_t *cap){
  for (int i = 0; i < req->concurrency; i++) {
    cap->threads[i] = calloc(1, sizeof(pthread_t));
    assert(cap->threads[i] != NULL);
  }
  if (!cap->recv_chan) {
    cap->recv_chan = chan_init(vector_size(req->ids) * 2);
    debug("%s> Recv Chan Created", cap->name);
  }else{
    debug("%s> Recv Chan Already Exists", cap->name);
  }
  if (!cap->send_chan) {
    cap->send_chan = chan_init(vector_size(req->ids) * 2);
    debug("%s> Send Chan Created", cap->name);
  }else{
    debug("%s> Send Chan Already Exists", cap->name);
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

char *get_capture_type_name(enum capture_type_id_t type){
  switch (type) {
  case CAPTURE_TYPE_WINDOW: return("window");

  case CAPTURE_TYPE_DISPLAY: return("display");

  case CAPTURE_TYPE_SPACE: return("space");

  default: return("unknown");
  }
}

static char *get_image_type_filename(enum capture_type_id_t capture_type, enum image_type_id_t format, size_t id, bool thumbnail){
  char *s;

  asprintf(&s, "%s%s-%lu%s-%lld.%s",
           gettempdir(),
           get_capture_type_name(capture_type),
           id,
           thumbnail ? "-thumbnail" : "",
           timestamp(),
           stringfn_to_lowercase(image_type_name(format))
           );
  return(s);
}

static int run_recv_msg(void *CAP){
  struct cap_t *cap = (struct cap_t *)CAP;

  debug("Starting %s", cap->name);
  void          *msg;
  size_t        qty = 0;
  unsigned long s   = timestamp();

  while (chan_recv(cap->recv_chan, &msg) == 0) {
    qty++;
    debug("%s> Got msg %lu/%lu!", cap->name, qty, cap->qty);
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
  debug("%s> run complete |qty:%lu|dur:%s|", cap->name, qty, milliseconds_to_string(timestamp() - s));
  chan_send(cap->done_chan, (void *)0);
  return(EXIT_SUCCESS);
}

static struct chan_t *compression_wait_chan;
static struct chan_t *compression_done_chan;
static struct chan_t *compression_results_chan;

static int wait_recv_compress_done(void __attribute__((unused)) *REQ){
  struct capture_req_t *req = (struct capture_req_t *)REQ;
  void                 *msg;
  size_t               qty = 0, qqty = 0;
  unsigned long        s = timestamp();

  debug("compression recv waiting");
  struct Vector *compressed_results = vector_new();

  while (vector_size(compressed_results) < vector_size(req->ids) && chan_recv(compression_wait_chan, &msg) == 0) {
    qty++;
    debug("waiter compression recvd #%lu/#%lu", qty, vector_size(req->ids));
    if (req->bar) {
      req->bar->progress += (float)((float)1 / (float)(vector_size(req->ids)));
      cbar_display_bar(req->bar);
    }
    vector_push(compressed_results, msg);
  }
  debug("compression recv waited");
  debug("compression done waiting");
  while (qqty < req->concurrency && chan_recv(compression_done_chan, &msg) == 0) {
    qqty++;
    debug("compression done recvd #%lu/#%d", qqty, req->concurrency);
  }
  debug("compression done OK");
  debug("compression waiter ended with %lu items in %s", qty, milliseconds_to_string(timestamp() - s));
  chan_send(compression_results_chan, (void *)compressed_results);
  debug("compression send %lu items to results chan", vector_size(compressed_results));
}

static int run_compress_recv(void __attribute__((unused)) *CHAN){
  struct chan_t *chan = (struct chan_t *)CHAN;
  void          *msg;
  size_t        qty = 0;
  unsigned long s   = timestamp();

  debug("compression recv waiting");
  while (chan_recv(chan, &msg) == 0) {
    qty++;
    debug("> Got compress msg #%lu!", qty);
    struct compress_t *c = (struct compress_t *)msg;
    c->started = timestamp();
    bool              did_compress = false, tried_to_compress = false;
    debug("Run Compress Recv:   type:%d|%s|PNG:%d|%s", c->type, image_type_name(c->type), IMAGE_TYPE_PNG, image_type_name(IMAGE_TYPE_PNG));
    switch (c->type) {
    case IMAGE_TYPE_PNG:
      errno = 0;
      tried_to_compress = true;
      if (!compress_png_buffer(c->pixels, &(c->len))) {
        log_error("Failed to compress #%lu", c->id);
      }
      break;
    default: break;
    }

    c->dur = timestamp() - c->started;
    chan_send(compression_wait_chan, (void *)c);
    did_compress = (c->len < c->prev_len);
    if (!did_compress && tried_to_compress) {
      log_warn("Did not compress Item #%lu (New Size is %s, Old is %s)", c->id,
         bytes_to_string(c->len),
         bytes_to_string(c->prev_len)
         );
    }else if(tried_to_compress && did_compress){
      debug("Compressed #%lu from %s to %s in %s",
                c->id,
                bytes_to_string(c->prev_len),
                bytes_to_string(c->len),
                milliseconds_to_string(c->dur)
                );
    }
  }
  debug("recv compresses ended in %s, processed %lu items. sending to done", milliseconds_to_string(timestamp() - s), qty);
  chan_send(compression_done_chan, (void *)0);
  debug("sent to done");
  return(EXIT_SUCCESS);
} /* run_compress_recv */

static bool start_com(struct capture_req_t *req){
  if (req->compress) {
    for (int i = 0; i < req->concurrency; i++) {
      debug("%d", req->compress);
      int q = i % req->concurrency;
      debug("Creating recv compress chan with index:   %d", q);
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

static bool start_cap(struct capture_req_t *req, struct cap_t *cap){
  for (int i = 0; i < req->concurrency; i++) {
    if (pthread_create(cap->threads[i], NULL, run_recv_msg, (void *)cap) != 0) {
      log_error("Failed to created Thread %s #%d", cap->name, i);
      return(false);
    }
  }
  return(true);
}

static bool analyze_image_pixels(struct capture_result_t *r){
  r->id             = (size_t)vector_get(r->msg->req->ids, (size_t)(r->msg->index));
  r->file           = get_image_type_filename(r->msg->req->type, r->type, r->id, false);
  r->thumbnail_file = get_image_type_filename(r->msg->req->type, r->type, r->id, true);
  if (r->analyze) {
    if (r->type == IMAGE_TYPE_QOI) {
      QOIDecoder *qoi = QOIDecoder_New();
      if (QOIDecoder_Decode(qoi, r->pixels, r->len)) {
        r->width             = QOIDecoder_GetWidth(qoi);
        r->height            = QOIDecoder_GetHeight(qoi);
        r->has_alpha         = QOIDecoder_HasAlpha(qoi);
        r->linear_colorspace = QOIDecoder_IsLinearColorspace(qoi);
        if (WRITE_FILE) {
          fsio_write_binary_file(r->file, r->pixels, r->len);
        }
      }
      QOIDecoder_Delete(qoi);
    }else{
      VipsImage *image = NULL;
      errno = 0;
      if (!(image = vips_image_new_from_buffer(r->pixels, r->len, "", "access", VIPS_ACCESS_SEQUENTIAL, NULL))) {
        log_error("Failed to decode pixels");
        return(false);
      }
      r->width             = vips_image_get_width(image);
      r->height            = vips_image_get_height(image);
      r->has_alpha         = vips_image_hasalpha(image);
      r->linear_colorspace = vips_colourspace_issupported(image);
      if (WRITE_FILE) {
        vips_image_write_to_file(image, r->file, NULL);
        if (fsio_file_size(r->file) == 0) {
          fsio_remove(r->file);
        }
      }
      if (WRITE_THUMBNAIL) {
        VipsImage *thumbnail;
        vips_thumbnail_image(image, &thumbnail, clamp(THUMBNAIL_WIDTH, vips_image_get_width(image), THUMBNAIL_WIDTH), NULL);
        vips_image_write_to_file(image, r->thumbnail_file, NULL);
        log_info("wrote %s %dx%d thumbnail %s",
                 bytes_to_string(VIPS_IMAGE_SIZEOF_IMAGE(thumbnail)),
                 vips_image_get_width(thumbnail),
                 vips_image_get_height(thumbnail),
                 r->thumbnail_file
                 );
        if (fsio_file_size(r->thumbnail_file) == 0) {
          fsio_remove(r->thumbnail_file);
        }
        g_object_unref(thumbnail);
      }
      if (image) {
        g_object_unref(image);
      }
    }
  }
  return(true);
} /* analyze_image_pixels */

static bool wait_com(struct capture_req_t *req){
  for (int i = 0; i < req->concurrency; i++) {
    debug("joining comp thread #%d", i);
    pthread_join(req->comp->threads[i], NULL);
    debug("Thread comp #%d Returned", i);
  }
  return(true);
}

static bool wait_cap(struct capture_req_t *req, struct cap_t *cap){
  for (int i = 0; i < req->concurrency; i++) {
    pthread_join(cap->threads[i], NULL);
    debug("%s Thread #%d Returned", cap->name, i);
  }
  return(true);
}

struct Vector *capture(struct capture_req_t *req){
  struct Vector *capture_results_v             = vector_new();
  char *bar_msg;
  size_t bar_msg_len,bar_len;
  int term_width = clamp(get_terminal_width(), 40, 160);
  chan_t        *results_chan  = chan_init(vector_size(req->ids));
  caps = __caps;

  if(req->progress_bar_mode){
    req->bar = calloc(1, sizeof(struct cbar_t));
    asprintf(&bar_msg, BAR_MESSAGE_CAPTURE_TEMPLATE,vector_size(req->ids));
    bar_msg_len  = strlen(bar_msg);
    term_width = get_terminal_width();
    asprintf(&bar_msg,BAR_MESSAGE_TEMPLATE,bar_msg);
    bar_len     = term_width * BAR_TERMINAL_WIDTH_PERCENTAGE - bar_msg_len;
    *(req->bar) = cbar(clamp(bar_len, BAR_MIN_WIDTH, term_width * BAR_MAX_WIDTH_TERMINAL_PERCENTAGE), bar_msg);
    cbar_hide_cursor();
    req->bar->progress = 0.00;
    cbar_display_bar(req->bar);
  }

  debug("request type: %d|format:%d", req->type, req->format);
  struct Vector *providers = get_cap_providers(req->format);

  debug("Found %lu providers", vector_size(providers));
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
    debug("\tInitializing Provider #%d: %s", c, caps[c]->name);
    if (!init_cap(req, caps[c])) {
      log_error("Failed to initialize capture #%d: %s", c, caps[c]->name);
      goto fail;
    }
    debug("\tInitialized capture #%d: %s", c, caps[c]->name);
  }
  for (size_t i = 0; i < vector_size(providers); i++) {
    enum capture_chan_type_t c = (enum capture_chan_type_t)(size_t)vector_get(providers, i);
    if (!start_cap(req, caps[c])) {
      log_error("Failed to start capture #%d: %s", c, caps[c]->name);
      goto fail;
    }
    debug("\tstarted capture #%d: %s", c, caps[c]->name);
  }
  for (size_t i = 0; i < vector_size(providers); i++) {
    enum capture_chan_type_t c = (enum capture_chan_type_t)(size_t)vector_get(providers, i);
    if (caps[c]->provider_type == CAPTURE_PROVIDER_TYPE_IDS) {
      for (size_t i = 0; i < vector_size(req->ids); i++) {
        struct cgimage_recv_t *r = calloc(1, sizeof(struct cgimage_recv_t));
        r->index = i;
        r->req   = req;
        chan_send(caps[c]->recv_chan, (void *)r);
      }
      chan_close(caps[c]->recv_chan);
      if (CAPTURE_WINDOW_DEBUG_MODE) {
        debug("\tWAITING FOR DONE #%d: %s", c, caps[c]->name);
      }
      size_t completed = 0;
      for (int i = 0; i < req->concurrency; i++) {
        chan_recv(caps[c]->done_chan, (void *)0);
        completed++;
        debug("\t%d/%d DONE #%d: %s",
              i, req->concurrency,
              c, caps[c]->name
              );
      }
      debug("\tDONE #%d: %s", c, caps[c]->name);
      chan_close(caps[c]->send_chan);
    }
    debug("\tWaiting capture #%d: %s", c, caps[c]->name);
    if (!wait_cap(req, caps[c])) {
      log_error("Failed to wait capture #%d: %s", c, caps[c]->name);
      goto fail;
    }
    debug("\tWaited capture #%d: %s", c, caps[c]->name);
  }
  struct capture_result_t *r;
  size_t captured_images_len = 0;

  for (size_t i = 0; i < vector_size(req->ids); i++) {
    void *msg;
    if (chan_recv(results_chan, &msg) == 0) {
      r = (struct capture_result_t *)msg;
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
        free(caps[c]->threads[x]);
      }
    }
  }
  chan_dispose(results_chan);
  results_chan = NULL;
  if (req->progress_bar_mode) {
    req->bar->progress = (float)1.00;
    cbar_display_bar(req->bar);
    cbar_show_cursor();
//    printf("\n");
    fflush(stdout);
    free(req->bar);
  }

  if (req->compress) {
    void *compressed_images_msg;
    struct Vector *compressed_images_msg_v = NULL, *compressed_images_v = vector_new();
    struct compress_t *compressed_image_msg;
    if(req->progress_bar_mode){
      req->bar = calloc(1, sizeof(struct cbar_t));
      asprintf(&bar_msg, BAR_MESSAGE_COMPRESSION_TEMPLATE,vector_size(req->ids),bytes_to_string(captured_images_len));
      bar_msg_len  = strlen(bar_msg);
      asprintf(&bar_msg,BAR_MESSAGE_TEMPLATE,bar_msg);
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
    req->comp->waiter      = calloc(1, sizeof(pthread_t));
    req->comp->waiter_chan = chan_init(vector_size(req->ids));
    compression_wait_chan  = chan_init(vector_size(req->ids));
    compression_done_chan  = chan_init(0);
    compression_results_chan = chan_init(0);
    if (req->progress_bar_mode) {
      req->comp->bar = calloc(1, sizeof(struct cbar_t));
    }
    if (!start_com(req)) {
      log_error("Failed to start compressions");
      goto fail;
    }
    for (size_t i = 0; i < vector_size(capture_results_v); i++) {
      struct capture_result_t *r = (struct capture_result_t *)vector_get(capture_results_v, i);
      struct compress_t       *c = calloc(1, sizeof(struct compress_t));
      c->id             = r->id;
      c->pixels         = r->pixels;
      c->len            = r->len;
      c->prev_len       = r->len;
      c->type           = r->type;
      c->bar           = req->bar;
      c->max_quality    = MAX_QUALITY;
      c->min_quality    = MIN_QUALITY;
      c->capture_result = r;
      chan_send(req->comp->chans[i%req->concurrency], (void *)c);
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
      debug("new vector ite #%lu/%lu has %s image from %s image of type %d|%s",
                i + 1, vector_size(compressed_images_msg_v),
                bytes_to_string(compressed_image_msg->len),
                bytes_to_string(compressed_image_msg->prev_len),
                compressed_image_msg->type,
                image_type_name(compressed_image_msg->type)
                );
      struct capture_result_t *r = compressed_image_msg->capture_result;
      r->pixels = compressed_image_msg->pixels;
      r->len    = compressed_image_msg->len;
//      Dbg(r->len,%lu);
//      fsio_write_binary_file(r->file, r->pixels, r->len);
      debug("new vector item #%lu/%lu with id %lu %s file %s has %s image of type %d|%s",
                i + 1, vector_size(compressed_images_msg_v),
                r->id,
                bytes_to_string(fsio_file_size(r->file)),
                r->file,
                bytes_to_string(r->len),
                r->type,
                image_type_name(r->type)
                );
      vector_push(compressed_images_v, r);
    }
    capture_results_v = compressed_images_v;
    debug("compressions done");
    if (req->progress_bar_mode) {
      req->bar->progress = (float)1.00;
      cbar_display_bar(req->bar);
      cbar_show_cursor();
      printf("\n");
      fflush(stdout);
      free(req->bar);
    }
  }
  return(capture_results_v);
fail:
  return(capture_results_v);
} /* capture*/

bool end_animation(struct animated_capture_t *acap){
  errno = 0;
  fprintf(stdout, "%s", kitty_msg_delete_images());
  fflush(stdout);
  acap->result = msf_gif_end(acap->gif);

  char *msg;
  asprintf(&msg, AC_SHOW_CURSOR AC_LOAD_PALETTE);
  fprintf(stdout, "%s", msg); fflush(stdout);

  if (acap->file) {
    if (acap->result.data) {
      FILE *fp = fopen(acap->file, "wb");
      fwrite(acap->result.data, acap->result.dataSize, 1, fp);
      fclose(fp);
      if (CAPTURE_WINDOW_DEBUG_MODE) {
        log_info("Wrote %lu Frames to %s Animated GIF %s to %s",
                 vector_size(acap->frames_v),
                 milliseconds_to_string(timestamp() - acap->started), bytes_to_string(acap->result.dataSize), acap->file
                 );
      }
    }else{
      log_error("Invalid gif data");
      return(false);
    }
  }else{
    log_error("Invalid filename");
    return(false);
  }

  msf_gif_free(acap->result);
  //char *m;
//  asprintf(&m,"%s", AC_RESTORE_PRIVATE_PALETTE);
//fprintf(stdout,"%s",m);
//  fflush(stdout);
//free(m);
/*
  if(acap->progress_bar_mode){
  acap->bar->progress = (float)1.00;
  cbar_display_bar((acap->bar));
  cbar_show_cursor();
 // printf("\n");
  fflush(stdout);
}
*/
  return(true);
} /* end_animation */

bool new_animated_frame(struct animated_capture_t *acap, struct capture_result_t *r){
  struct animated_frame_t *n = calloc(1, sizeof(struct animated_frame_t));

  n->height = r->height;
  n->width  = r->width;
  n->len    = r->len;
  n->ts     = r->time.captured_ts;
  int           f = 0, w = 0, h = 0;
  unsigned long s;
  unsigned char *pixels = NULL;

  if (r->type == IMAGE_TYPE_QOI) {
    debug("QOI Decoding!");
    s = timestamp();
    QOIDecoder *qoi = QOIDecoder_New();
    if (QOIDecoder_Decode(qoi, r->pixels, r->len)) {
      w         = QOIDecoder_GetWidth(qoi);
      h         = QOIDecoder_GetHeight(qoi);
      n->width  = w;
      n->height = h;
      f         = QOIDecoder_HasAlpha(qoi) ? 4 : 3;
      pixels    = QOIDecoder_GetPixels(qoi);
      if (CAPTURE_WINDOW_DEBUG_MODE) {
        debug("QOI in %s|%dx%d|f:%d|alpha:%d|colorsp:%d|",
              milliseconds_to_string(timestamp() - s),
              w, h, f,
              QOIDecoder_HasAlpha(qoi),
              QOIDecoder_IsLinearColorspace(qoi)
              );
        acap->max_bit_depth = (int)((QOIDecoder_HasAlpha(qoi)) ?  32 : 24);
        acap->pitch_bytes   = (4 * w);
      }
    }
    QOIDecoder_Delete(qoi);
  }else if (r->type == IMAGE_TYPE_BMP) {
    pixels = r->pixels;
  }else{
    s = timestamp();
    FILE *fp = fmemopen(r->pixels, r->len, "rb");
    pixels = stbi_load_from_file(fp, &w, &h, &f, STBI_rgb_alpha);
    fclose(fp);
    if (CAPTURE_WINDOW_DEBUG_MODE) {
      log_info("STBI in %s",
               milliseconds_to_string(timestamp() - s)
               );
    }
    acap->max_bit_depth = (int)((f == 4) ?  32 : 24);
    acap->pitch_bytes   = (int)((f == 4) ? (4 * w) : (3 * w));
  }

  if (vector_size(acap->frames_v) == 0) {
    acap->width   = n->width;
    acap->height  = n->width;
    acap->started = n->ts;
    msf_gif_begin(acap->gif, w, h);
  }
  debug("req type:%d|f:%d|max bit:%d|pitch bytes:%d|stbi rgb alpha:%d|size:%s|file:%s|%dx%d",
        r->type,
        f,
        acap->max_bit_depth,
        acap->pitch_bytes,
        STBI_rgb_alpha,
        bytes_to_string(r->len),
        r->file,
        w, h
        );
  int cs;


  if (vector_size(acap->frames_v) > 0) {
    cs = (int)(r->delta_ms / 10);
      debug("cs:%d|cs per frame:%d|delta:%lu|",
                cs,
                (int)(acap->ms_per_frame / 10),
                r->delta_ms
                );
  }else{
    cs = (int)((acap->ms_per_frame) / 10);
  }
  //printf(AC_ESC "7");
  // kitty_display_image_buffer(r->pixels,r->len);
  kitty_display_image_buffer_resized_width_at_row_col(r->pixels, r->len, 200, 0, (size_t)((float)acap->term_width * (float)(.85)));
  //printf("\x1b[%dA", 5);
  //printf("\x1b[%dC", 5);
  //printf(AC_ESC "8");
  msf_gif_frame(acap->gif, pixels,
                cs,
                acap->max_bit_depth,
                acap->pitch_bytes
                );
  vector_push(acap->frames_v, (void *)n);
  acap->total_size = animated_frames_len(acap);
  return(true);
} /* new_animated_frame */

size_t animated_frames_len(struct animated_capture_t *acap){
  size_t len = 0;

  for (size_t i = 0; i < vector_size(acap->frames_v); i++) {
    len += ((struct animated_frame_t *)vector_get(acap->frames_v, i))->len;
  }
  return(len);
}

bool inspect_frames(struct animated_capture_t *acap){
  gd_GIF        *gif    = gd_open_gif(acap->file);
  size_t        len     = (gif->width * gif->height * 3);
  char          *buffer = malloc(len);
  unsigned long ts;

  for (unsigned looped = 1;; looped++) {
    while (gd_get_frame(gif)) {
      ts = timestamp();
      gd_render_frame(gif, buffer);
      debug("Loaded %s Buffer from frame #%d/%d in %s",
            bytes_to_string(len),
            looped, gif->loop_count,
            milliseconds_to_string(timestamp() - ts)
            );
    }
    if (looped == gif->loop_count) {
      break;
    }
    gd_rewind(gif);
  }
  free(buffer);
  gd_close_gif(gif);
}

int poll_new_animated_frame(void *VOID){
  struct animated_capture_t *acap = (struct animated_capture_t *)VOID;
  char                      *bar_msg;
  void *msg;
    if(acap->progress_bar_mode){
  asprintf(&bar_msg,
           "$E BOLD; $ECapturing %lu %s %s Frames at %luFPS: $E RGB(8, 104, 252); $E[$E RGB(8, 252, 104);UNDERLINE; $E$F'-'$F$E RGB(252, 8, 104); $E$N'-'$N$E RESET_UNDERLINE;RGB(8, 104, 252); $E] $E FG_RESET; $E$P%% $E RESET; $E"
           "%s",
           acap->expected_frames_qty,
           get_capture_type_name(acap->type),
           stringfn_to_lowercase(image_type_name(acap->format)),
           (size_t)((float)((float)1 / (float)acap->ms_per_frame) * (float)(100 * 10)),
           ""
           );
  if(vector_size(acap->frames_v)==0)
    acap->bar = &(cbar(clamp((int)((float)acap->term_width * (float)(.4)), 20, 60), bar_msg));
  cbar_hide_cursor();
    }

  while (chan_recv(acap->chan, &msg) == 0) {
    struct capture_result_t *r = (struct capture_result_t *)msg;
    pthread_mutex_lock(acap->mutex);
    float bar_progress  = vector_size(acap->frames_v) == 0 ? 0 : (float)((float)(vector_size(acap->frames_v) / (float)(acap->expected_frames_qty)) / (float)1);
    if (CAPTURE_WINDOW_DEBUG_MODE) {
      log_info("msg %s|%lu|progress:%f|frames:%lu|expected frames:%lu|bar mode:%d|", 
          r->file, r->len, 
          bar_progress,
          vector_size(acap->frames_v), 
          acap->expected_frames_qty,
          acap->progress_bar_mode
          );
    }
    new_animated_frame(acap, r);
    if(acap->progress_bar_mode){
       acap->bar->progress = bar_progress; 
      cbar_display_bar((acap->bar));
      fflush(stdout);
    }
    pthread_mutex_unlock(acap->mutex);
  }
  chan_send(acap->done, (void *)0);
  if (CAPTURE_WINDOW_DEBUG_MODE) {
    log_info("poller finished");
  }
  return(0);
} /* poll_new_animated_frame */

struct animated_capture_t *init_animated_capture(enum capture_type_id_t type, enum image_type_id_t format, size_t id, size_t ms_per_frame, bool progress_bar_mode){
  struct animated_capture_t *n = calloc(1, sizeof(struct animated_capture_t));

  n->frames_v     = vector_new();
  n->gif          = calloc(1, sizeof(MsfGifState));
  n->ms_per_frame = ms_per_frame;
  n->progress_bar_mode = progress_bar_mode;
  n->id           = id;
  n->term_width   = get_terminal_width();
  n->term_width   = clamp(n->term_width, 40, 160);
  n->format       = format;
  n->type         = type;
  n->width        = 0;
  n->height       = 0;
  n->started      = 0;
  asprintf(&(n->file), "%s%s-%lu-animation.%s", gettempdir(), get_capture_type_name(n->type), id, stringfn_to_lowercase(image_type_name(n->format)));

  n->thread = calloc(1, sizeof(pthread_t));
  n->mutex  = calloc(1, sizeof(pthread_mutex_t));
  assert(pthread_mutex_init(n->mutex, NULL) == 0);
  n->chan = chan_init(100);
  n->done = chan_init(0);
  pthread_create(n->thread, NULL, poll_new_animated_frame, (void *)n);

  return(n);
}

///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__capture_window(void){
//  if(!getenv("TMPDIR") || !stringfn_starts_with(getenv("TMPDIR"),"/tmp/"))
//    setenv("TMPDIR", "/tmp/", 1);
  setenv("VIPS_WARNING", "1", 1);

  if (getenv("DEBUG") != NULL || getenv("CAPTURE_WINDOW_DEBUG_MODE") != NULL) {
    log_debug("Enabling capture_window Debug Mode");
    CAPTURE_WINDOW_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#endif
