#pragma once
#ifndef CAPTURE_WINDOW_C
#define CAPTURE_WINDOW_C
#define THUMBNAIL_WIDTH    150
#define WRITE_FILE         true
#define WRITE_THUMBNAIL    false
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
#include "image/utils/utils.h"
#include "libimagequant/libimagequant.h"
#include "log/log.h"
#include "ms/ms.h"
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
#define debug(M, ...)    { do {                               \
                             if (CAPTURE_WINDOW_DEBUG_MODE) { \
                               log_debug(M, ## __VA_ARGS__);  \
                             }                                \
                           } while (0); }
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
  [CAPTURE_CHAN_TYPE_CGIMAGE] = &(struct cap_t)       {
    .name          = "CGImage Capture",
    .enabled       = true, .format = IMAGE_TYPE_CGIMAGE,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_IDS,
    .recv_msg      = ^ void *(void *MSG)              {
      struct cgimage_recv_t *r = (struct cgimage_recv_t *)MSG;
      size_t window_id         = (size_t)vector_get(r->req->ids, (size_t)(r->index));
      debug("Capturing #%lu/%lu: %lu |type:%d|format:%d|w:%d|h:%d|",
            r->index, vector_size(r->req->ids), window_id,
            r->req->type, r->req->format,
            r->req->width, r->req->height
            );
      r->img_ref      = NULL;
      r->time.started = timestamp();
      if (r->req->width > 0)                          {
        r->img_ref = capture_type_width(r->req->type, window_id, r->req->width);
      }else if (r->req->height > 0)                   {
        r->img_ref = capture_type_height(r->req->type, window_id, r->req->height);
      }else                                           {
        r->img_ref = capture_type_capture(r->req->type, window_id);
      }
      debug("got img ref");
      r->width    = CGImageGetWidth(r->img_ref);
      r->height   = CGImageGetHeight(r->img_ref);
      r->time.dur = timestamp() - r->time.started;
      debug("Captured %lux%lu Window #%lu in %s", r->width, r->height, window_id, milliseconds_to_string(r->time.dur));
      return((void *)r);
    },
  },
  [CAPTURE_CHAN_TYPE_RGB] = &(struct cap_t)           {
    .name          = "CGImage To RGB Pixels",
    .enabled       = true, .format = IMAGE_TYPE_RGB,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)              {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->msg          = (struct cgimage_recv_t *)MSG;
      r->len          = 0;
      r->time.started = timestamp();
      r->pixels       = save_cgref_to_rgb_memory(r->msg->img_ref, &(r->len));
      r->time.dur     = timestamp() - r->time.started;
      if (!analyze_image_pixels(r))                   {
        log_error("Failed to analyze RGB");
        //goto error;
      }
      debug("Converted CGImageRef to %s RGB Pixels in %s",
            bytes_to_string(r->len),
            milliseconds_to_string(r->time.dur)
            );
      return((void *)r);

      error :
      return((void *)0);
    },
  },
  [CAPTURE_CHAN_TYPE_QOI] = &(struct cap_t)           {
    .name          = "CGImage To QOI Pixels",
    .enabled       = true, .format = IMAGE_TYPE_QOI,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)              {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_QOI;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to QOI", r->msg->width, r->msg->height);
      r->len          = 0;
      r->time.started = timestamp();
      r->pixels       = save_cgref_to_qoi_memory(r->msg->img_ref, &(r->len));
      r->time.dur     = timestamp() - r->time.started;

      errno = 0;

      if (!analyze_image_pixels(r))                   {
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
  [CAPTURE_CHAN_TYPE_GIF] = &(struct cap_t)           {
    .name          = "CGImage To GIF Pixels",
    .enabled       = true, .format = IMAGE_TYPE_GIF,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)              {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_GIF;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to GIF", r->msg->width, r->msg->height);
      r->len          = 0;
      r->time.started = timestamp();
      r->pixels       = save_cgref_to_gif_memory(r->msg->img_ref, &(r->len));
      r->time.dur     = timestamp() - r->time.started;
      if (!analyze_image_pixels(r))                   {
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
  [CAPTURE_CHAN_TYPE_TIFF] = &(struct cap_t)          {
    .name          = "CGImage To TIFF Pixels",
    .enabled       = true, .format = IMAGE_TYPE_TIFF,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)              {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_TIFF;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to TIFF", r->msg->width, r->msg->height);
      r->len          = 0;
      r->time.started = timestamp();
      r->pixels       = save_cgref_to_tiff_memory(r->msg->img_ref, &(r->len));
      r->time.dur     = timestamp() - r->time.started;
      if (!analyze_image_pixels(r))                   {
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
  [CAPTURE_CHAN_TYPE_BMP] = &(struct cap_t)           {
    .name          = "CGImage To BMP Pixels",
    .enabled       = true, .format = IMAGE_TYPE_BMP,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)              {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_BMP;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to BMP", r->msg->width, r->msg->height);
      r->len          = 0;
      r->time.started = timestamp();
      r->pixels       = save_cgref_to_bmp_memory(r->msg->img_ref, &(r->len));
      r->time.dur     = timestamp() - r->time.started;
      if (!analyze_image_pixels(r))                   {
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
  [CAPTURE_CHAN_TYPE_JPEG] = &(struct cap_t)          {
    .name          = "CGImage To JPEG Pixels",
    .enabled       = true, .format = IMAGE_TYPE_JPEG,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)              {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_JPEG;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to JPEG", r->msg->width, r->msg->height);
      r->len          = 0;
      r->time.started = timestamp();
      r->pixels       = save_cgref_to_jpeg_memory(r->msg->img_ref, &(r->len));
      r->time.dur     = timestamp() - r->time.started;
      if (!analyze_image_pixels(r))                   {
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
  [CAPTURE_CHAN_TYPE_PNG_COMPRESSED] = &(struct cap_t){
    .name          = "CGImage To PNG Pixels",
    .enabled       = true, .format = IMAGE_TYPE_PNG_COMPRESSED,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)              {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_PNG_COMPRESSED;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to PNG", r->msg->width, r->msg->height);
      char *s;
      size_t window_id = (size_t)vector_get(r->msg->req->ids, (size_t)(r->msg->index));
      asprintf(&s, "%sWindow-comp-%lu.png", gettempdir(), window_id);
      save_cgref_to_image_type_file(IMAGE_TYPE_PNG, r->msg->img_ref, s);
      r->len          = 0;
      r->time.started = timestamp();

      size_t rgb_len            = 0;
      unsigned char *rgb_pixels = save_cgref_to_rgb_memory(r->msg->img_ref, &rgb_len);
      r->pixels = imagequant_encode_rgb_pixels_to_png_buffer(rgb_pixels, r->msg->width, r->msg->height, 10, 70, &(r->len));
      free(rgb_pixels);
      r->time.dur = timestamp() - r->time.started;
      if (!analyze_image_pixels(r))                   {
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
  [CAPTURE_CHAN_TYPE_PNG] = &(struct cap_t)           {
    .name          = "CGImage To PNG Pixels",
    .enabled       = true, .format = IMAGE_TYPE_PNG,
    .debug         = true,
    .provider_type = CAPTURE_PROVIDER_TYPE_CAP,
    .provider      = CAPTURE_CHAN_TYPE_CGIMAGE,
    .recv_msg      = ^ void *(void *MSG)              {
      struct capture_result_t *r = calloc(1, sizeof(struct capture_result_t));
      r->type = IMAGE_TYPE_PNG;
      r->msg  = (struct cgimage_recv_t *)MSG;
      debug("Converting %lux%lu CGImageref to PNG", r->msg->width, r->msg->height);
      r->len          = 0;
      r->time.started = timestamp();
      size_t rgb_len = 0;
      r->pixels = save_cgref_to_jpeg_memory(r->msg->img_ref, &(r->len));
      VipsImage *image;
      vips_jpegload_buffer(r->pixels, r->len, &image, NULL);
      r->time.dur = timestamp() - r->time.started;
      log_debug("Loaded %s RGB Pixels to %dx%d %s PNG VIPImage in %s",
                bytes_to_string(rgb_len),
                vips_image_get_width(image), vips_image_get_height(image),
                bytes_to_string(VIPS_IMAGE_SIZEOF_IMAGE(image)),
                milliseconds_to_string(r->time.dur)
                );
      vips_pngsave_buffer(image, r->pixels, &(r->len), NULL);
      char *s;
      size_t window_id = (size_t)vector_get(r->msg->req->ids, (size_t)(r->msg->index));
      asprintf(&s, "%sWindow-%lu.png", gettempdir(), window_id);
//VipsTarget *target;
      vips_pngsave(image, s, NULL);
      VipsImage *image1;
      vips_pngload(s, &image1, NULL);
      vips_pngsave_buffer(image1, &(r->pixels), &(r->len), NULL);
/*
 *      vips_image_write_to_target( image, ".png", target, NULL );
 *    vips_image_write_to_file(image,s,NULL);
 */
      /*
       * r->pixels = imagequant_encode_rgb_pixels_to_png_buffer(rgb_pixels, r->msg->width, r->msg->height, 10, 70, &(r->len));
       * int           width = 0, height = 0, format = 0, req_format = STBI_rgb_alpha;
       * unsigned char *rgb_pixels1 = stbi_load_from_memory(r->pixels, r->len, &width, &height, &format, req_format);
       * log_info("%d,%d,%d",width,height,format);
       * CGImageRef ir =     rgb_pixels_to_png_cgimage_ref(r->pixels,width,height);
       * log_debug("%lux%lu",CGImageGetWidth(ir),CGImageGetHeight(ir));
       * log_debug("%lux%lu",CGImageGetWidth(r->msg->img_ref),CGImageGetHeight(r->msg->img_ref));
       * r->pixels = save_cgref_to_image_type_memory1(ir,&(r->len));
       * //fsio_write_binary_file(s,r->pixels,r->len);
       * save_cgref_to_png_file(ir,s);
       *
       */

      //    r->pixels = save_cgref_to_png_memory(r->msg->img_ref, &(r->len));
      if (!analyze_image_pixels(r))                   {
        log_error("Failed to analyze PNG");
        //goto error;
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
    log_error("Unable to find capture type for format %d", format);
    return(v);
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
  cap->qty = vector_size(req->ids) / req->concurrency;
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
    chan_send(cap->send_chan, m);
    debug("Send message to send chan");
  }
  debug("%s> run complete |qty:%lu|dur:%s|", cap->name, qty, milliseconds_to_string(timestamp() - s));
  chan_send(cap->done_chan, (void *)0);
  return(EXIT_SUCCESS);
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
  if (r->type == IMAGE_TYPE_QOI) {
    QOIDecoder *qoi = QOIDecoder_New();
    if (QOIDecoder_Decode(qoi, r->pixels, r->len)) {
      r->width             = QOIDecoder_GetWidth(qoi);
      r->height            = QOIDecoder_GetHeight(qoi);
      r->has_alpha         = QOIDecoder_HasAlpha(qoi);
      r->linear_colorspace = QOIDecoder_IsLinearColorspace(qoi);
      fsio_write_binary_file(r->file, r->pixels, r->len);
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
  return(true);
} /* analyze_image_pixels */

static bool wait_cap(struct capture_req_t *req, struct cap_t *cap){
  for (int i = 0; i < req->concurrency; i++) {
    pthread_join(cap->threads[i], NULL);
    debug("%s Thread #%d Returned", cap->name, i);
  }
  return(true);
}

struct Vector *capture_windows(struct capture_req_t *req){
  //CAPTURE_WINDOW_DEBUG_MODE=true;
  caps = __caps;
  struct Vector *v            = vector_new();
  chan_t        *results_chan = chan_init(vector_size(req->ids));

  debug("request type: %d|format:%d", req->type, req->format);
  struct Vector *providers = get_cap_providers(req->format);

  debug("Found %lu providers", vector_size(providers));
  for (size_t i = 0; i < vector_size(providers); i++) {
    enum capture_chan_type_t c = (enum capture_chan_type_t)(size_t)vector_get(providers, i);
    if (i > 0) {
      enum capture_chan_type_t prev_c = (enum capture_chan_type_t)(size_t)vector_get(providers, i - 1);
      caps[c]->recv_chan = caps[prev_c]->send_chan;
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
      for (int i = 0; i < req->concurrency; i++) {
        chan_recv(caps[c]->done_chan, (void *)0);
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

  for (size_t i = 0; i < vector_size(req->ids); i++) {
    void *msg;
    if (chan_recv(results_chan, &msg) == 0) {
      r = (struct capture_result_t *)msg;
      debug("Received Result #%lu/%lu | %lux%lu | %s | %s | %s | Linear Colorspace: %s | Has Alpha: %s |",
            i + 1, vector_size(req->ids),
            r->width, r->height, bytes_to_string(r->len), milliseconds_to_string(r->time.dur),
            r->file,
            r->linear_colorspace?"Yes":"No",
            r->has_alpha?"Yes":"No"

            );
      vector_push(v, msg);
    }
  }
  for (size_t i = 0; i < vector_size(providers); i++) {
    enum capture_chan_type_t c = (enum capture_chan_type_t)(size_t)vector_get(providers, i);
    if (caps[c]->recv_chan) {
      chan_dispose(caps[c]->recv_chan);
    }
    // if(caps[c]->send_chan)
    //   chan_dispose(caps[c]->send_chan);
    if (caps[c]->done_chan) {
      chan_dispose(caps[c]->done_chan);
    }
    caps[c]->recv_chan = NULL;
    caps[c]->send_chan = NULL;
    caps[c]->done_chan = NULL;
    for (int x = 0; x < req->concurrency; x++) {
      if (caps[c]->threads[x]) {
        free(caps[c]->threads[x]);
      }
    }
  }
  //chan_dispose(results_chan);
  return(v);

fail:
  return(vector_new());
} /* capture_windows */

bool end_animation(struct animated_capture_t *acap){
  errno        = 0;
  acap->result = msf_gif_end(acap->gif);
  if (acap->file) {
    if (acap->result.data) {
      FILE *fp = fopen(acap->file, "wb");
      fwrite(acap->result.data, acap->result.dataSize, 1, fp);
      fclose(fp);
      log_info("Wrote %lu Frames to %s Animated GIF %s to %s",
               vector_size(acap->frames_v),
               milliseconds_to_string(timestamp() - acap->started), bytes_to_string(acap->result.dataSize), acap->file
               );
    }else{
      log_error("Invalid gif data");
    }
  }else{
    log_error("Invalid filename");
  }
  msf_gif_free(acap->result);
  return(true);
}

bool new_animated_frame(struct animated_capture_t *acap, struct capture_result_t *r){
  struct animated_frame_t *n = calloc(1, sizeof(struct animated_frame_t));

  n->height = r->height;
  n->width  = r->width;
  n->len    = r->len;
  n->ts     = r->time.started - (r->time.dur / 2);
  int           f = 0, w = 0, h = 0;
  FILE          *fp     = fopen(r->file, "rb");
  unsigned char *pixels = stbi_load_from_file(fp, &w, &h, &f, STBI_rgb_alpha);

  fclose(fp);
  if (vector_size(acap->frames_v) == 0) {
    acap->width   = n->width;
    acap->height  = n->width;
    acap->started = n->ts;
    msf_gif_begin(acap->gif, w, h);
  }
  acap->max_bit_depth = (int)((f == 4) ?  32 : 24);
  acap->pitch_bytes   = (int)((f == STBI_rgb_alpha) ? (4 * w) : (3 * w));
  pixels = r->pixels;
  log_info("%d|%d|%d|%d|%s|%s|%dx%d",
           f,
           acap->max_bit_depth,
           acap->pitch_bytes,
           STBI_rgb_alpha,
           bytes_to_string(r->len),
           r->file,
           w, h
           );
  msf_gif_frame(acap->gif, pixels,
                (int)(acap->ms_per_frame / 10),
                acap->max_bit_depth,
                acap->pitch_bytes
                );
  free(pixels);
  vector_push(acap->frames_v, (void *)n);
  acap->total_size = animated_frames_len(acap);
  return(true);
}

size_t animated_frames_len(struct animated_capture_t *acap){
  size_t len = 0;

  for (size_t i = 0; i < vector_size(acap->frames_v); i++) {
    len += ((struct animated_frame_t *)vector_get(acap->frames_v, i))->len;
  }
  return(len);
}

struct animated_capture_t *init_animated_capture(enum capture_type_id_t type, enum image_type_id_t format, size_t id, size_t ms_per_frame){
  struct animated_capture_t *n = calloc(1, sizeof(struct animated_capture_t));

  n->frames_v     = vector_new();
  n->gif          = calloc(1, sizeof(MsfGifState));
  n->ms_per_frame = ms_per_frame;
  n->id           = id;
  n->format       = format;
  n->type         = type;
  n->width        = 0;
  n->height       = 0;
  n->started      = 0;
  asprintf(&(n->file), "%s%s-%lu-animation.%s", gettempdir(), get_capture_type_name(n->type), id, "gif");
  return(n);
}

///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__capture_window(void){
  setenv("TMPDIR", "/tmp/", 1);
  setenv("VIPS_WARNING", "1", 1);
  if (getenv("DEBUG") != NULL || getenv("CAPTURE_WINDOW_DEBUG_MODE") != NULL) {
    log_debug("Enabling capture_window Debug Mode");
    CAPTURE_WINDOW_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
