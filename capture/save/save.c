#pragma once
#ifndef CAPTURE_SAVE_C
#define CAPTURE_SAVE_C
#define LOCAL_DEBUG_MODE                     CAPTURE_SAVE_DEBUG_MODE
#define BAR_MESSAGE_SAVE_TEMPLATE            "Saving    %lu %s %s%s"
#define BAR_SAVE_MESSAGE_TEMPLATE            "$E BOLD; $E%s: $E RGB(8, 104, 252); $E[$E RGB(8, 252, 104);UNDERLINE; $E$F'-'$F$E RGB(252, 8, 104); $E$N'-'$N$E RESET_UNDERLINE;RGB(8, 104, 252); $E] $E FG_RESET; $E$P%% $E RESET; $E"
#define BAR_TERMINAL_WIDTH_PERCENTAGE        .8
#define BAR_TERMINAL_WIDTH_PERCENTAGE        .8
#define BAR_MIN_WIDTH                        20
#define BAR_MAX_WIDTH_TERMINAL_PERCENTAGE    .5
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "capture/save/save.h"
#include "chan/src/chan.h"
#include "chan/src/queue.h"
#include "clamp/clamp.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

////////////////////////////////////////////
static bool CAPTURE_SAVE_DEBUG_MODE = false;
static int receive_save_request_handler(void *CAP);
///////////////////////////////////////////////////////////////////////
struct save_result_t {
  size_t                        index;
  chan_t                        *recv_chan, *done_chan;
  char                          *file;
  size_t                        bytes;
  enum image_type_id_t          format;
  struct capture_image_result_t *result;
};

static int receive_save_request_handler(void *R){
  unsigned long        started = timestamp(), dur = 0, save_started = 0;
  struct save_result_t *r = (struct save_result_t *)R;
  void                 *msg;
  size_t               qty                = 0;
  char                 *image_loader_name = NULL;
  VipsImage            *image             = NULL;

  while (chan_recv(r->recv_chan, &msg) == 0) {
    qty++;
    struct capture_image_result_t *res = (struct capture_image_result_t *)msg;
    save_started = timestamp();
    switch (res->format) {
    case IMAGE_TYPE_QOI:
      if (!fsio_write_binary_file(res->file, res->pixels, res->len)) {
        log_error("Failed to save QOI file %s with %lu Pixels", res->file, res->len);
      }
      break;
    default:
      image_loader_name = vips_foreign_find_load_buffer(res->pixels, res->len);
      image             = vips_image_new_from_buffer(res->pixels, res->len, "", NULL);
      debug("Loaded %s %s Pixels to %dx%d %s File %s PNG VIPImage in %s using loader %s",
            bytes_to_string(res->len),
            image_type_name(res->format),
            vips_image_get_width(image), vips_image_get_height(image),
            bytes_to_string(VIPS_IMAGE_SIZEOF_IMAGE(image)),
            res->file,
            milliseconds_to_string(dur),
            image_loader_name
            );

      if (!image_loader_name || !image || image_target_file_savers[res->format](image, res->file, NULL)) {
        log_error("Failed to save file %s with loader %s", res->file, image_loader_name);
      }
      if (image) {
        g_object_unref(image);
      }

    }
    dur = timestamp() - save_started;

    log_debug("Thread #%lu>"
            "\n\tReceived %s %s Save Capture Request #%lu to %s"
            "%s",
            r->index + 1,
            bytes_to_string(res->len),
            image_type_name(res->format),
            qty,
            res->file,
            ""
            );
    r->bytes = fsio_file_size(res->file);
    chan_send(r->done_chan, (void *)r);
  }
  chan_close(r->done_chan);
  debug(
    "%lu> Save Handler complete. Processed %lu items in %s"
    "%s",
    r->index,
    qty,
    milliseconds_to_string(timestamp() - started),
    ""
    );
  return(EXIT_SUCCESS);
} /* receive_save_request_handler */
struct save_capture_result_t *save_capture_type_results(enum capture_type_id_t type, enum image_type_id_t format, struct Vector *results_v, size_t concurrency, char *save_directory, bool progress_bar_mode){
  struct save_capture_result_t *res = calloc(1, sizeof(struct save_capture_result_t));

  res->qty     = 0;
  res->bytes   = 0;
  res->started = timestamp();
  size_t    bar_msg_len = 0, bar_len = 0;
  void      *msg;
  char      *bar_msg;
  int       term_width = 80;
  cbar_t    *bar       = NULL;
  pthread_t *threads[concurrency];
  chan_t    *chans[concurrency];
  chan_t    *done_chans[concurrency];

  if (progress_bar_mode) {
    bar = calloc(1, sizeof(struct cbar_t));
    asprintf(&bar_msg, BAR_MESSAGE_SAVE_TEMPLATE, vector_size(results_v), image_type_name(format), get_capture_type_name(type), vector_size(results_v) > 1 ? "s" : "");
    bar_msg_len = strlen(bar_msg);
    term_width  = clamp(get_terminal_width(), 40, 160);
    asprintf(&bar_msg, BAR_SAVE_MESSAGE_TEMPLATE, bar_msg);
    bar_len = term_width * BAR_TERMINAL_WIDTH_PERCENTAGE - bar_msg_len;
    *(bar)  = cbar(clamp(bar_len, BAR_MIN_WIDTH, term_width * BAR_MAX_WIDTH_TERMINAL_PERCENTAGE), bar_msg);
    cbar_hide_cursor();
    bar->progress = 0.00;
    cbar_display_bar(bar);
  }
  debug("Saving %lu %s Results in format %s to %s with concurrency %lu"
        "%s",
        vector_size(results_v),
        get_capture_type_name(type),
        image_type_name(format),
        save_directory,
        concurrency,
        ""
        );
  for (size_t i = 0; i < concurrency; i++) {
    chans[i] = chan_init(vector_size(results_v));
    done_chans[i] = chan_init(vector_size(results_v)/concurrency);
    struct save_result_t *r = calloc(1, sizeof(struct save_result_t));
    r->done_chan = done_chans[i];
    r->recv_chan = chans[i];
    r->index     = i;
    threads[i]   = calloc(1, sizeof(pthread_t));
    pthread_create(threads[i], 0, receive_save_request_handler, (void *)r);
  }
  for (size_t i = 0; i < vector_size(results_v); i++) {
    struct capture_image_result_t *r = (struct capture_image_result_t *)vector_get(results_v, i);
    r->format = format;
    chan_send(chans[i % concurrency], r);
  }
  sleep(1);
  for (size_t i = 0; i < concurrency; i++) {
    chan_close(chans[i % concurrency]);
  }
  size_t done_qty =0;
  for (size_t i = 0; i < concurrency; i++) {
    while (chan_recv(done_chans[i], &msg) == 0) {
      if (progress_bar_mode) {
        bar->progress += (float)((float)1 / (float)(vector_size(results_v)));
        cbar_display_bar(bar);
      }
      struct save_result_t *r = (struct save_result_t *)msg;
      res->bytes += r->bytes;
      log_debug("Received save result #%lu/%lu %s", done_qty+1, vector_size(results_v), bytes_to_string(r->bytes));
      done_qty++;
    }
  }
  res->dur = timestamp() - res->started;
  if (progress_bar_mode) {
    bar->progress = (float)1.00;
    cbar_display_bar(bar);
    cbar_show_cursor();    printf("\n");
    printf("%s",
           "\033[F"
           "\033[K"
           "\033[F"
           "\033[K"
           );
    fflush(stdout);
    free(bar);
  }
  debug("Saved %s among %lu Files in %s",
        bytes_to_string(res->bytes), res->qty,
        milliseconds_to_string(res->dur)
        );

  return(res);
} /* save_capture_type_results */
static void __attribute__((constructor)) __constructor__type(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_type") != NULL) {
    log_debug("Enabling image type Debug Mode");
    CAPTURE_SAVE_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#endif
