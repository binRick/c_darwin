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
  struct capture_image_result_t *res;

  while (chan_recv(r->recv_chan, &res) == 0) {
    qty++;
    if(!res->pixels || !res->len || !res->file)
      log_error("Failed to receive valid save object");
    else if(!fsio_write_binary_file(res->file, res->pixels, res->len))
      log_error("Failed to save file %s with %lu Pixels", res->file, res->len);
    r->bytes = fsio_file_size(res->file);
    r->file=res->file;
    r->format=res->format;
    r->bytes=res->len;
    chan_send(r->done_chan, (void *)r);
  }
  chan_close(r->done_chan);
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
    chans[i]      = chan_init(vector_size(results_v));
    done_chans[i] = chan_init(vector_size(results_v) / concurrency);
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
  for (size_t i = 0; i < concurrency; i++)
    chan_close(chans[i % concurrency]);

  for (size_t i = 0; i < concurrency; i++)
    while (chan_recv(done_chans[i], &msg) == 0) {
      if (progress_bar_mode) {
        bar->progress += (float)((float)1 / (float)(vector_size(results_v)));
        cbar_display_bar(bar);
      }
      struct save_result_t *r = (struct save_result_t *)msg;
      res->bytes += r->bytes;
      debug("Received save result #%lu/%lu %s", res->qty + 1, vector_size(results_v), bytes_to_string(r->bytes));
      res->qty++;
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
