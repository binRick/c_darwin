#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "c_workqueue/include/workqueue.h"
#include "chan/src/chan.h"
#include "chan/src/queue.h"
#include "kitty/msg/msg.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "vips-basics-test/vips-basics-test.h"
#include "vips/vips.h"
#define UNUSED    __attribute__((unused))
#define KITTY_PRINT_PATH(PATH)    { do {                                                                                     \
                                      if (greatest_get_verbosity() > 1) {                                                    \
                                        kitty_display_image_buffer(fsio_read_binary_file(outfile), fsio_file_size(outfile)); \
                                        printf("\n");                                                                        \
                                      }                                                                                      \
                                    } while (0); }
#include "incbin/incbin.h"
INCBIN(communist_goals, "assets/communist-goals.png");
INCBIN(kitty_icon, "assets/kitty_icon.png");
INCBIN(spinner0, "assets/spinner0.gif");
INCBIN(spinner1, "assets/spinner1.gif");
#define PROGRESS    false
#define QTY(X)    (sizeof(X) / sizeof(X[0]))
static float factors[] = {
  0.10,
  0.50,
  1.50,
};
static const char
*gifs[] = {
  "/tmp/spinner1.gif",
  "/tmp/spinner0.gif",
},
*frame_exts[] = { "qoi", "png", "gif", "jpg", "webp", },
*files[]      = {
  "/tmp/communist-goals.png",
  "/tmp/kitty_icon.png",
},
*exts[] = { "qoi", "png", "gif", "jpg", "tif", "webp", };
static size_t
  gifs_qty    = QTY(gifs),
  files_qty   = QTY(files),
  exts_qty    = QTY(exts),
  factors_qty = QTY(factors);

static int annotate_image(VipsObject *context, VipsImage *image, VipsImage **out) {
  int       page_height = vips_image_get_page_height(image);
  int       n_pages     = image->Ysize / page_height;
  VipsImage **overlay   = (VipsImage **)
                          vips_object_local_array(context, n_pages);
  VipsImage **page = (VipsImage **)
                     vips_object_local_array(context, n_pages);
  VipsImage **annotated = (VipsImage **)
                          vips_object_local_array(context, n_pages);
  double    red[]         = { 255, 0, 0, 255 };
  double    transparent[] = { 0, 0, 0, 0 };

  int       i;

  for ( i = 0; i < n_pages; i++ )
    if (vips_crop(image, &page[i], 0, page_height * i, image->Xsize, page_height, NULL))
      return(-1);

  if (!(overlay[0] = vips_image_new_from_image(page[0], red, VIPS_NUMBER(red)))
      || vips_draw_rect(
        overlay[0], transparent, VIPS_NUMBER(transparent),
        10, 10, overlay[0]->Xsize - 20, overlay[0]->Ysize - 20, "fill", true, NULL
        )
      )
    return(-1);

  for ( i = 0; i < n_pages; i++ )
    if (vips_composite2(page[i], overlay[0], &annotated[i],
                        VIPS_BLEND_MODE_OVER, NULL))
      return(-1);

  if (vips_arrayjoin(annotated, out, n_pages, "across", 1, NULL))
    return(-1);

  return(0);
} /* annotate_image */

typedef void (^worker_cb)(void *);

struct recv_t {
  size_t          qty;
  chan_t          *chan;
  struct Vector   *recvd;
  pthread_mutex_t mutex;
};

struct work_done_t {
  size_t        len, id;
  unsigned char *buf;
  char          *format;
};
struct work_t {
  worker_cb cb;
  struct worker_args_t {
    struct args_image_t {
      VipsImage     *image_in, *image_out;
      unsigned char *buf_out;
      char          *path_in, *path_out;
      size_t        buf_out_len;
      char          *format;
      float         factor;
      chan_t        *bufs;
    }      image;
    size_t id;
    chan_t *results;
    void   *arg;
    chan_t *chan;
  }      args;
  size_t dur, started;
};

void recv_fxn(void *RECV){
  struct recv_t *r  = (struct rect_t *)RECV;
  size_t        qty = 0;
  void          *msg;

  while (qty < r->qty && chan_recv(r->chan, &msg) == 0) {
    log_info("result #%lu/%lu", ++qty, r->qty);
    pthread_mutex_lock(&(r->mutex));
    vector_push(r->recvd, (void *)msg);
    pthread_mutex_unlock(&(r->mutex));
  }
  log_info("results done");
}

void work_fxn(void *WORK){
  struct work_t *w = (struct work_t *)WORK;

  w->started = timestamp();
  w->cb((void *)&(w->args));
  w->dur = timestamp() - w->started;
  chan_send(w->args.results, (void *)w);
}

struct Vector *run_queues(size_t concurrency, struct Vector *items, worker_cb cb){
  size_t           items_qty = vector_size(items), id = 0;
  struct Vector    *results_v = vector_new();
  struct WorkQueue *queues[concurrency], *recv;
  chan_t           *results;

  results = chan_init(items_qty);
  recv    = workqueue_new();
  struct recv_t *r = calloc(1, sizeof(struct recv_t));

  r->chan  = results;
  r->qty   = items_qty;
  r->recvd = results_v;
  workqueue_push(recv, recv_fxn, (void *)r);
  for (size_t i = 0; i < concurrency; i++)
    queues[i] = workqueue_new();
  for (size_t ii = 0; ii < items_qty; ii++) {
    struct work_t *w = calloc(1, sizeof(struct work_t));
    w->cb           = cb;
    w->args.id      = id++;
    w->args.results = results;
    w->args.arg     = (void *)vector_get(items, ii);
    workqueue_push(queues[ii % concurrency], work_fxn, (void *)w);
  }
  for (size_t i = 0; i < concurrency; i++)
    workqueue_drain(queues[i]);
  for (size_t i = 0; i < concurrency; i++)
    workqueue_release(queues[i]);
  chan_close(recv);
  workqueue_drain(recv);
  workqueue_release(recv);
  log_info("%lu results", vector_size(results_v));
  return(results_v);
}

TEST t_vips_resize_queued(){
  struct  chan_t *bufs = chan_init(1000);
  size_t         concurrency = 5;
  size_t         TEST_INDEX = 5;
  worker_cb      worker = ^ void (void *W){
    struct worker_args_t *w = (struct worker_args_t *)W;
    struct args_image_t  *a = (struct args_image_t *)w->arg;
    if (!(a->image_in = vips_image_new_from_file(a->path_in, "access", VIPS_ACCESS_SEQUENTIAL, NULL))) {
      log_error("Failed to load file");
      return;
    }
    if (vips_resize(a->image_in, &(a->image_out), a->factor, NULL)) {
      log_error("Failed to resize file");
      return;
    }
    if (vips_image_write_to_buffer(a->image_out, a->format, &(a->buf_out), &(a->buf_out_len), NULL)) {
      log_error("Failed to save file");
      return;
    }
    struct work_done_t *d = calloc(1, sizeof(struct work_done_t));
    d->len    = a->buf_out_len;
    d->buf    = a->buf_out;
    d->id     = w->id;
    d->format = a->format;
    chan_send(a->bufs, (void *)d);
  };
  unsigned long ts = timestamp();
  size_t        total_len = 0, qty = 0;
  struct Vector *items_v = vector_new();

  for (size_t i = 0; i < files_qty; i++)
    for (size_t o = 0; o < exts_qty; o++)
      for (size_t f = 0; f < factors_qty; f++) {
        char                *outfile;
        asprintf(&outfile, "/tmp/output-resized-%.0f-percent-%lu-%lu-%lu.%s", factors[f] * 100, i, o, TEST_INDEX, exts[o]);
        struct args_image_t *a = calloc(1, sizeof(struct args_image_t));
        a->path_in     = files[i];
        a->path_out    = outfile;
        a->image_in    = NULL;
        a->image_out   = NULL;
        a->factor      = factors[f];
        a->buf_out_len = 0;
        asprintf(&a->format, ".%s", exts[o]);
        a->bufs = bufs;
        vector_push(items_v, (void *)a);
      }
  struct Vector *results_v = run_queues(concurrency, items_v, worker);
  struct work_t *w;

  for (size_t i = 0; i < vector_size(results_v); i++) {
    w = (struct work_t *)vector_get(results_v, i);
    log_info("%lu> %s", w->args.id + 1, milliseconds_to_string(w->dur));
  }
  void *msg;

  chan_close(bufs);
  struct work_done_t *d;

  while (chan_recv(bufs, &msg) == 0) {
    d = (struct work_done_t *)msg;
    log_info("buf %lu %s %s", d->id, bytes_to_string(d->len), d->format);
  }
} /* t_vips_resize_queued */

TEST t_vips_queued(){
  size_t        id = 0;
  size_t        concurrency = 200, items = 10000;
  struct Vector *results_v = vector_new();
  struct WorkQueue
                *queues[concurrency],
  *recv;
  chan_t        *results;

  results = chan_init(items);
  worker_cb worker = ^ void (void *W){
    usleep(1000 * 50);
  };

  recv = workqueue_new();
  struct recv_t *r = calloc(1, sizeof(struct recv_t));

  r->chan  = results;
  r->qty   = items;
  r->recvd = results_v;
  workqueue_push(recv, recv_fxn, (void *)r);
  for (size_t i = 0; i < concurrency; i++)
    queues[i] = workqueue_new();
  for (size_t ii = 0; ii < items; ii++) {
    struct work_t *w = calloc(1, sizeof(struct work_t));
    w->cb           = worker;
    w->args.id      = id++;
    w->args.results = results;
    workqueue_push(queues[ii % concurrency], work_fxn, (void *)w);
  }
  for (size_t i = 0; i < concurrency; i++)
    workqueue_drain(queues[i]);
  for (size_t i = 0; i < concurrency; i++)
    workqueue_release(queues[i]);
  chan_close(recv);
  workqueue_drain(recv);
  workqueue_release(recv);
  log_info("%lu results", vector_size(results_v));
  struct work_t *w;

  for (size_t i = 0; i < vector_size(results_v); i++) {
    w = (struct work_t *)vector_get(results_v, i);
    log_info("%lu> %s", w->args.id + 1, milliseconds_to_string(w->dur));
  }
  PASS();
} /* t_vips_queued */

TEST t_vips_gif_frames(){
  size_t        TEST_INDEX = 4;
  VipsImage     *image, *annotated, *resized, *frames;
  VipsObject    *context = NULL;
  char          *outfile, *infile, *frameoutfile;
  float         resize = 0.10;
  int           n_pages;
  size_t        qty = 0, total_bytes = 0;
  unsigned long ts = timestamp();

  for (size_t i = 0; i < gifs_qty; i++) {
    infile = gifs[i];
    if (!(image = vips_image_new_from_file(infile, "access", VIPS_ACCESS_SEQUENTIAL, NULL)))
      FAILm("Failed to load file");
    for (size_t o = 0; o < QTY(frame_exts); o++)
      if (vips_image_get_n_pages(image) > 1)
        for (int f = 0; f < vips_image_get_n_pages(image); f++) {
          asprintf(&outfile, "/tmp/gif-%s-%lu-%lu-frame-%d.%s", basename(stringfn_substring(infile, 0, strlen(infile) - 4)), i, TEST_INDEX, f, frame_exts[o]);
          if (fsio_file_exists(outfile)) fsio_remove(outfile);
          unsigned long ts = timestamp();
          if (!(frames = vips_image_new_from_file(infile, "access", VIPS_ACCESS_SEQUENTIAL, "page", f, NULL)))
            FAIL();
          if (vips_image_write_to_file(frames, outfile, NULL))
            FAIL();
          if (greatest_get_verbosity() > 0)
            log_debug(AC_BLUE "Wrote %s Frame #%d/%d to %s in %s",
                      bytes_to_string(fsio_file_size(outfile)),
                      f + 1, vips_image_get_n_pages(image),
                      outfile, milliseconds_to_string(timestamp() - ts)
                      );
          total_bytes += fsio_file_size(outfile);
          qty++;
          if (fsio_file_size(outfile) > 0)
            KITTY_PRINT_PATH(outfile);
          g_object_unref(frames);
        }
    g_object_unref(image);
  }
  log_debug(AC_BLUE "Wrote %lu %s Images in %s", qty,
            bytes_to_string(total_bytes),
            milliseconds_to_string(timestamp() - ts)
            );

  PASS();
} /* t_vips_gif_frames */

TEST t_vips_resize(){
  unsigned long ts = timestamp();
  size_t        total_len = 0, qty = 0;
  size_t        TEST_INDEX = 3;
  VipsImage     *image;

  for (size_t i = 0; i < files_qty; i++)
    for (size_t o = 0; o < exts_qty; o++) {
      char  *outfile;
      float resize = 0.10;
      asprintf(&outfile, "/tmp/output-resized-%.0f-percent-%lu-%lu-%lu.%s", resize * 100, i, o, TEST_INDEX, exts[o]);

      if (!(image = vips_image_new_from_file(files[i], "access", VIPS_ACCESS_SEQUENTIAL, NULL)))
        FAILm("Failed to load file");

      VipsImage *out;

      if (vips_resize(image, &out, resize, NULL))
        FAILm("Failed to resize image");
      if (greatest_get_verbosity() > 1)
        log_debug(AC_BLUE "Resizing by %f %s %s [%dx%d] (%d) %s" AC_RESETALL,
                  resize,
                  bytes_to_string(fsio_file_size(files[i])),
                  files[i],
                  vips_image_get_width(image),
                  vips_image_get_height(image),
                  vips_image_get_format(image), outfile
                  );

      if (vips_image_write_to_file(out, outfile, NULL))
        FAILm("Failed to write outfile");
      if (greatest_get_verbosity() > 1)
        log_debug(AC_MAGENTA "%s %s [%dx%d] (%d)" AC_RESETALL,
                  bytes_to_string(fsio_file_size(outfile)),
                  outfile,
                  vips_image_get_width(out),
                  vips_image_get_height(out),
                  vips_image_get_format(out)
                  );
      KITTY_PRINT_PATH(outfile);

      unsigned long ts = timestamp();
      if (!(out = vips_image_new_from_file(outfile, "access", VIPS_ACCESS_SEQUENTIAL, NULL)))
        FAILm("Failed to Read outfile");
      if (greatest_get_verbosity() > 0)
        log_debug(AC_GREEN "%s %s [%dx%d] (%d) %s" AC_RESETALL,
                  bytes_to_string(fsio_file_size(outfile)),
                  outfile,
                  vips_image_get_width(out),
                  vips_image_get_height(out),
                  vips_image_get_format(out), milliseconds_to_string(timestamp() - ts)
                  );

      qty++;
      total_len += fsio_file_size(outfile);
      g_object_unref(out);
      g_object_unref(image);
    }
  char *msg;

  asprintf(&msg, "Resized %s %lu Images in %s", bytes_to_string(total_len), qty, milliseconds_to_string(timestamp() - ts));
  PASSm(msg);
} /* t_vips_resize */

TEST t_vips_annotate(){
  size_t        TEST_INDEX = 2;
  unsigned long ts = timestamp();
  size_t        total_len = 0, qty = 0;
  VipsImage     *image, *annotated;
  VipsObject    *context = NULL;

  for (size_t i = 0; i < files_qty; i++)
    for (size_t o = 0; o < exts_qty; o++) {
      char *outfile, *infile;
      asprintf(&outfile, "/tmp/output-%lu-%lu-%lu.%s", i, o, TEST_INDEX, exts[o]);
      asprintf(&infile, "%s", files[i]);

      unsigned long ts = timestamp();
      context = VIPS_OBJECT(vips_image_new());
      if (!(image = vips_image_new_from_file(infile, "access", VIPS_ACCESS_SEQUENTIAL, NULL)))
        FAIL();
      if (annotate_image(context, image, &annotated))
        FAIL();
      if (vips_image_write_to_file(annotated, outfile, NULL))
        FAIL();
      unsigned long dur = timestamp() - ts;

      if (greatest_get_verbosity() > 0)
        log_debug(AC_CYAN "Annotated %s Image %s in %s : [%dx%d]" AC_RESETALL,
                  outfile,
                  bytes_to_string(fsio_file_size(outfile)),
                  milliseconds_to_string(dur),
                  vips_image_get_width(annotated),
                  vips_image_get_height(annotated)
                  );
      qty++;
      total_len += fsio_file_size(outfile);
      KITTY_PRINT_PATH(outfile);

      g_object_unref(image);
      g_object_unref(context);
    }

  char *msg;

  asprintf(&msg, "Annotated %s %lu Images in %s", bytes_to_string(total_len), qty, milliseconds_to_string(timestamp() - ts));
  PASSm(msg);
} /* t_vips_basics_test2 */

SUITE(s_vips_queued) {
  RUN_TEST(t_vips_queued);
}
SUITE(s_vips_annotate) {
  RUN_TEST(t_vips_annotate);
}
SUITE(s_vips_gif_frames) {
  RUN_TEST(t_vips_gif_frames);
}
SUITE(s_vips_resize) {
  RUN_TEST(t_vips_resize);
  RUN_TEST(t_vips_resize_queued);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  VIPS_INIT(argv);
  fsio_write_binary_file(files[0], gcommunist_goalsData, gcommunist_goalsSize);
  fsio_write_binary_file(files[1], gkitty_iconData, gkitty_iconSize);
  fsio_write_binary_file(gifs[1], gspinner0Data, gspinner0Size);
  fsio_write_binary_file(gifs[0], gspinner1Data, gspinner1Size);
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_vips_queued);
  RUN_SUITE(s_vips_annotate);
  RUN_SUITE(s_vips_resize);
  RUN_SUITE(s_vips_gif_frames);
  GREATEST_MAIN_END();
  vips_shutdown();
}
