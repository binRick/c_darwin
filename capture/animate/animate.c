#pragma once
#ifndef ANIMATE_TYPE_C
#define ANIMATE_TYPE_C
////////////////////////////////////////////
#define PREVIEW_ANIMATION_IN_TERMINAL                  true
#define PREVIEW_ANIMATION_TERMINAL_WIDTH               200
#define PREVIEW_ANIMATION_IN_TERMINAL_COLUMN_OFFSET    0.85
#define PREVIEW_ANIMATION_IN_TERMINAL_ROW_OFFSET       0.0
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

#include "capture/animate/animate.h"
#include "capture/type/type.h"
#include "vips/vips.h"
#include "window/utils/utils.h"
#include <png.h>
#include <pthread.h>
static bool CAPTURE_ANIMATE_DEBUG_MODE = false;
#define LOCAL_DEBUG_MODE    CAPTURE_ANIMATE_DEBUG_MODE
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

bool end_animation(struct capture_animation_result_t *acap){
  char *msg;

  errno = 0;
  fprintf(stdout, "%s", kitty_msg_delete_images());
  fflush(stdout);
  acap->result = msf_gif_end(acap->gif);

  asprintf(&msg, AC_SHOW_CURSOR AC_LOAD_PALETTE);
  fprintf(stdout, "%s", msg); fflush(stdout);

  if (acap->file) {
    if (acap->result.data) {
      errno = 0;
      if (!fsio_write_binary_file(acap->file, acap->result.data, acap->result.dataSize))
        log_error("Failed to write %s to %s", bytes_to_string(acap->result.dataSize), acap->file);
      if (CAPTURE_ANIMATE_DEBUG_MODE)
        log_info("Wrote %lu Frames to %s Animated GIF %s to %s",
                 vector_size(acap->frames_v),
                 milliseconds_to_string(timestamp() - acap->started), bytes_to_string(acap->result.dataSize), acap->file
                 );
    }else{
      log_error("Invalid gif data");
      return(false);
    }
  }else{
    log_error("Invalid filename");
    return(false);
  }

  msf_gif_free(acap->result);
  return(true);
} /* end_animation */

bool new_animated_frame(struct capture_animation_result_t *acap, struct capture_image_result_t *r){
  struct animated_frame_t *n = calloc(1, sizeof(struct animated_frame_t));
  int                     f = 0, w = 0, h = 0, frame_cs;
  FILE                    *stbi_fp;
  unsigned long           s;
  unsigned char           *rgb_pixels = NULL;

  n->height = r->height;
  n->width  = r->width;
  n->len    = r->len;
  n->ts     = r->time.captured_ts;

  if (r->type == IMAGE_TYPE_QOI) {
    s = timestamp();
    QOIDecoder *qoi = QOIDecoder_New();
    if (QOIDecoder_Decode(qoi, r->pixels, r->len)) {
      w          = QOIDecoder_GetWidth(qoi);
      h          = QOIDecoder_GetHeight(qoi);
      n->width   = w;
      n->height  = h;
      f          = QOIDecoder_HasAlpha(qoi) ? 4 : 3;
      rgb_pixels = QOIDecoder_GetPixels(qoi);
      if (CAPTURE_ANIMATE_DEBUG_MODE) {
        log_debug("Decoded QOI data in %s|%dx%d|f:%d|alpha:%d|colorsp:%d|",
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
  }else if (r->type == IMAGE_TYPE_BMP)
    rgb_pixels = r->pixels;
  else{
    s          = timestamp();
    stbi_fp    = fmemopen(r->pixels, r->len, "rb");
    rgb_pixels = stbi_load_from_file(stbi_fp, &w, &h, &f, STBI_rgb_alpha);
    fclose(stbi_fp);
    debug("\nSTBI Decoded: %dx%d|%d %s\n", w, h, f, milliseconds_to_string(timestamp() - s));
    acap->max_bit_depth = (int)((f == 4) ?  32 : 24);
    acap->pitch_bytes   = (int)((f == 4) ? (4 * w) : (3 * w));
  }

  if (vector_size(acap->frames_v) == 0) {
    acap->width   = n->width;
    acap->height  = n->width;
    acap->started = n->ts;
    msf_gif_begin(acap->gif, w, h);
  }
  if (vector_size(acap->frames_v) > 0)
    frame_cs = (int)(r->delta_ms / 10);
  else
    frame_cs = (int)((acap->ms_per_frame) / 10);
  if (PREVIEW_ANIMATION_IN_TERMINAL)
    kitty_display_image_buffer_resized_width_at_row_col(r->pixels, r->len,
                                                        PREVIEW_ANIMATION_TERMINAL_WIDTH,
                                                        (size_t)((float)acap->term_width * (float)(PREVIEW_ANIMATION_IN_TERMINAL_ROW_OFFSET)),
                                                        (size_t)((float)acap->term_width * (float)(PREVIEW_ANIMATION_IN_TERMINAL_COLUMN_OFFSET))
                                                        );
  msf_gif_frame(acap->gif, rgb_pixels,
                frame_cs,
                acap->max_bit_depth,
                acap->pitch_bytes
                );
  vector_push(acap->frames_v, (void *)n);
  acap->total_size = animated_frames_len(acap);
  if (rgb_pixels)
    free(rgb_pixels);
  return(true);
} /* new_animated_frame */

size_t animated_frames_len(struct capture_animation_result_t *acap){
  size_t len = 0;

  for (size_t i = 0; i < vector_size(acap->frames_v); i++)
    len += ((struct animated_frame_t *)vector_get(acap->frames_v, i))->len;
  return(len);
}

bool inspect_frames(struct capture_animation_result_t *acap){
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
    if (looped == gif->loop_count)
      break;
    gd_rewind(gif);
  }
  free(buffer);
  gd_close_gif(gif);
}

int poll_new_animated_frame(void *VOID){
  struct capture_animation_result_t *acap = (struct capture_animation_result_t *)VOID;
  char                              *bar_msg;
  void                              *msg;

  if (acap->progress_bar_mode) {
    asprintf(&bar_msg,
             "$E BOLD; $ECapturing %lu %s %s %s Frames at %luFPS: $E RGB(8, 104, 252); $E[$E RGB(8, 252, 104);UNDERLINE; $E$F'-'$F$E RGB(252, 8, 104); $E$N'-'$N$E RESET_UNDERLINE;RGB(8, 104, 252); $E] $E FG_RESET; $E$P%% $E RESET; $E"
             "%s",
             acap->expected_frames_qty,
             get_capture_type_name(acap->type),
             image_type_name(acap->format),
             stringfn_to_lowercase(image_type_name(acap->format)),
             (size_t)((float)((float)1 / (float)acap->ms_per_frame) * (float)(100 * 10)),
             ""
             );
    if (vector_size(acap->frames_v) == 0)
      acap->bar = &(cbar(clamp((int)((float)acap->term_width * (float)(.4)), 20, 60), bar_msg));
    cbar_hide_cursor();
  }

  while (chan_recv(acap->chan, &msg) == 0) {
    struct capture_image_result_t *r = (struct capture_image_result_t *)msg;
    pthread_mutex_lock(acap->mutex);
    float                         bar_progress = vector_size(acap->frames_v) == 0 ? 0 : (float)((float)(vector_size(acap->frames_v) / (float)(acap->expected_frames_qty)) / (float)1);
    new_animated_frame(acap, r);
    if (acap->progress_bar_mode) {
      acap->bar->progress = bar_progress;
      cbar_display_bar((acap->bar));
      fflush(stdout);
    }
    pthread_mutex_unlock(acap->mutex);
  }
  chan_send(acap->done, (void *)0);
  return(0);
} /* poll_new_animated_frame */

struct capture_animation_result_t *init_animated_capture(enum capture_type_id_t type, enum image_type_id_t format, size_t id, size_t ms_per_frame, bool progress_bar_mode){
  struct capture_animation_result_t *n = calloc(1, sizeof(struct capture_animation_result_t));

  n->frames_v          = vector_new();
  n->gif               = calloc(1, sizeof(MsfGifState));
  n->ms_per_frame      = ms_per_frame;
  n->progress_bar_mode = progress_bar_mode;
  n->id                = id;
  n->term_width        = get_terminal_width();
  n->term_width        = clamp(n->term_width, 40, 160);
  n->format            = format;
  n->type              = type;
  n->width             = 0;
  n->height            = 0;
  n->started           = 0;
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
  //setenv("VIPS_WARNING", "1", 1);
  if (getenv("DEBUG") != NULL || getenv("CAPTURE_ANIMATE_DEBUG_MODE") != NULL) {
    log_debug("Enabling capture_window Debug Mode");
    CAPTURE_ANIMATE_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#undef debug
#endif
