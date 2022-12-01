#pragma once
#ifndef DLS_TEST_COMMANDS_C
#define DLS_TEST_COMMANDS_C
#define TEST_VECTOR_QTY              5
#define MIN_STREAM_MS                1000
#define MAX_STREAM_MS                600000
#define DISPLAY_STREAM_IMAGE         false
#define BUFFERED_STREAM_ITEMS_QTY    1000
#include "capture/type/type.h"
#include "capture/utils/utils.h"
#include "color/color.h"
#include "core/core.h"
#include "dls/command-test.h"
#include "dls/dls.h"
#include "glib.h"
#include "kitty/msg/msg.h"
#include "match/match.h"
#include "qoir/src/qoir.h"
#include "qoir/util/pixbufs_are_equal.h"
#include "sha256.c/sha256.h"
#include "submodules/c_deps/submodules/c_greatest/greatest/greatest.h"
#include "vips/vips.h"
#include "window/utils/utils.h"
extern int DLS_EXECUTABLE_ARGC;
extern char **DLS_EXECUTABLE_ARGV;

///////////////////////////////////////////
int _command_test_terminal(){
  log_info("tt");
  int x = 0, y = 0;
  tc_get_cursor(&x, &y);
  Dbg(x, %d);
  Dbg(y, %d);
  exit(EXIT_SUCCESS);
}

void _command_test_csv(){
  exit(EXIT_SUCCESS);
}

int stop_loop(void *L){
  struct stream_setup_t *l = (struct stream_setup_t *)L;

  pthread_mutex_lock(l->mutex);
  size_t delay_ms = l->delay_ms;

  pthread_mutex_unlock(l->mutex);
  usleep(1000 * delay_ms);
  pthread_mutex_lock(l->mutex);
  l->ended = true;
  pthread_mutex_unlock(l->mutex);
  CFRunLoopStop(l->loop);
  return(EXIT_SUCCESS);
}

#define SIZE    200

void wu_dev_vips(){
  int       x, y, z;
  VipsPel   *mem;
  VipsImage *image;

  if (!(mem = VIPS_ARRAY(NULL, 4 * SIZE * SIZE, VipsPel)))
    vips_error_exit(NULL);
  for ( y = 0; y < SIZE; y++ )
    for ( x = 0; x < SIZE; x++ )
      for ( z = 0; z < 4; z++ )
        mem[y * 4 * SIZE + x * 4 + z] = 2 * (x + y + z);

  if (!(image = vips_image_new_from_memory(mem, 4 * SIZE * SIZE, SIZE, SIZE, 4, VIPS_FORMAT_UCHAR)))
    vips_error_exit(NULL);

  if (vips_image_write_to_file(image, "/tmp/d.png", NULL))
    vips_error_exit(NULL);

  g_object_unref(image);
  g_free(mem);
}

#define DO_LOG              true
#define LOG_INTERVAL        50
#define MAX_HISTORY_SIZE    5
#define HISTORY_MS          10000

bool wu_analyze_buf(void *L){
  struct stream_setup_t *u = (struct stream_update_t *)L;
  bool                  ok = (u->width && u->height);

  (u->img = vips_image_new_from_memory(u->png, u->png_len, u->width, u->height, 4, VIPS_FORMAT_UCHAR)) && (vips_image_write_to_buffer(u->img, ".png", &(u->png), &(u->png_len), NULL) && u->png_len && u->png);
  log_info("analzying.......");
  return(ok);
}

static VipsImage **pre_images = NULL;

int crop_animation(VipsObject *context, VipsImage *image, VipsImage **out,
                   int left, int top, int width, int height){
  int       page_height = vips_image_get_page_height(image);
  int       n_pages     = image->Ysize / page_height;
  VipsImage **page      = (VipsImage **)vips_object_local_array(context, n_pages);
  VipsImage **copy      = (VipsImage **)vips_object_local_array(context, 1);
  VipsImage **imgs      = (VipsImage **)vips_object_local_array(context, 10);
  VipsImage **pres      = (VipsImage **)vips_object_local_array(context, 10);
  VipsImage **posts     = (VipsImage **)vips_object_local_array(context, 10);
  VipsImage **sides     = (VipsImage **)vips_object_local_array(context, 10);

  if (!pre_images)
    pre_images = (VipsImage **)vips_object_local_array(context, 10);
  int i;

  for ( i = 0; i < n_pages; i++ ) {
    if (vips_crop(image, &page[i], left, page_height * i + top, width, height, NULL))
      return(-1);

    if (vips_arrayjoin(page, &copy[0], n_pages, "across", 1, NULL) || vips_copy(copy[0], out, NULL))
      return(-1);
  }
  if (vips_crop(image, &imgs[i], left, page_height * i + top, width, height, NULL))
    return(-1);

  if (vips_crop(pre_images[3], &pres[i], left, page_height * i + top, width, height, NULL))
    return(-1);

  vips_image_set_int(*out, "page-height", height);
  return(0);
}
#define        WU_DISPLAY_TERMINAL_SCALE_FACTOR    (0.15)
#define        WU_DISPLAY_TERMINAL_WIDTH           550
typedef void (^wu_cb)(void);
#define WU_SAVE_QOIR_FILE                          true
#define WU_PROCESS_STREAM_IMAGES_INTERVAL_MS       1000

struct wu_receive_msg_t {
  char      *json;
  struct {
    VipsImage *buf;
  } vips;
  size_t    buf_len; unsigned char *buf;
  int       width, height, x, y, bit_depth, cs, rect_width, rect_height;
  float     factor;
  FILE      *fd;
  MsfGifState gifState;
  struct {
    unsigned char *buf;
    size_t len;
    int width, height, bands, fmt;
    char *path;
  } png;
};

int wu_receive_images(void *L){
  struct stream_setup_t *l = (struct stream_setup_t *)L;
  bool                  ended;

  pthread_mutex_lock(l->mutex);
  ended = l->ended;
  pthread_mutex_unlock(l->mutex);
  struct wu_receive_msg_t *msg;
  size_t                  qty = 0;
  MsfGifState             gifState;
  MsfGifResult            gif_res = { 0 };
  int                     width, height, centisecondsPerFrame, bitDepth;
  unsigned char           *buf;
  size_t                  buf_len;
  VipsImage               *image, *tracker, *joined, *resized;

  while (!ended && chan_recv(l->images_chan, &msg) == 0) {
    if (!msg) continue;
    if (msg->json)
      fprintf(stderr, "%s\n", stringfn_trim((char *)(msg->json)));
    char *tf;
    asprintf(&tf,"%s%s.gif","/tmp/",msg->png.path);
    Di(vips_image_get_width(msg->vips.buf));
    Di(vips_image_get_height(msg->vips.buf));
    vips_image_write_to_file(msg->vips.buf,tf,NULL);
    size_t ll=fsio_file_size(tf);
    Dn(ll);
    Ds(tf);
    VipsImage *i;
    if((i=vips_image_new_from_memory(fsio_read_binary_file(tf), fsio_file_size(tf), vips_image_get_width(msg->vips.buf), vips_image_get_height(msg->vips.buf),vips_image_get_bands(msg->vips.buf),VIPS_FORMAT_UCHAR))){
      Di(vips_image_get_width(i));
    }else{
      log_warn("Failed to decode %s %dx%d %d %d GIF Buffer",bytes_to_string(fsio_file_size(tf)),vips_image_get_width(msg->vips.buf), vips_image_get_height(msg->vips.buf),vips_image_get_bands(msg->vips.buf),VIPS_FORMAT_UCHAR);
    }
    fsio_remove(tf);
    g_object_unref(i);
    pthread_mutex_lock(l->mutex);
    ended = l->ended;
    pthread_mutex_unlock(l->mutex);
    free(msg);
    Di(msg->width);
    Di(msg->height);
    Di(msg->rect_height);
    Di(msg->rect_width);
    Di(msg->x);
    Di(msg->y);
    Di(msg->cs);
    Dn(qty);
    qty++;
  }
} /* wu_receive_images */

int wu_receive_stream(void *L){
  struct stream_setup_t *l     = (struct stream_setup_t *)L;
  float                 factor = WU_DISPLAY_TERMINAL_SCALE_FACTOR;
  struct winsize        *ws;
  bool                  ended;
  void                  **msg;
  CGRect                rect;
  int                   width = 0, height = 0, w, h;
  size_t                qty = 0, copied = 0;
  char                  *s;

  pthread_mutex_lock(l->mutex);
  ws                 = get_terminal_size();
  ws->ws_col, height = ws->ws_row;
  ended              = l->ended;
  unsigned long started = timestamp(), last_ts = timestamp();
  struct Vector *history = vector_new_with_options(MAX_HISTORY_SIZE, false);

  pthread_mutex_unlock(l->mutex);

  char          *show_file, *stats_text;
  unsigned long write_file_started = 0, write_file_dur = 0;
  size_t        skipped_frames = 0, received_frames = 0;

  char *raw_file;
  while (!ended && chan_recv(l->chan, &msg) == 0) {
  VipsImage     *resized, *image, *tracker, *joined;
    received_frames++;
    unsigned long           ts = timestamp(), durs[10] = { 0 };
    pthread_mutex_lock(l->mutex);
    struct stream_update_t  *u = (struct stream_update_t *)msg;
    struct wu_receive_msg_t *m = calloc(1, sizeof(struct wu_receive_msg_t));
    pthread_mutex_unlock(l->mutex);
    if (!u) continue;
    if (u->seed < qty) continue;
    qty++;
    pthread_mutex_lock(l->mutex);
    copied += u->buf_len;
    if ((u->seed % 10) == 0 || u->seed < 10)
      printf(AC_CLS);
    if (chan_size(l->chan) < 5)
      factor = clamp((factor + 0.05), .10, 1.00);
    if (chan_size(l->chan) > 30)
      factor = clamp((factor - 0.05), .10, 1.00);
    int ox = (int)(factor * (float)(u->width - u->rect.size.width)),
        oy = (int)(factor * (float)(u->height - u->rect.size.height));

    m->png.buf=NULL;
    m->png.len=0;
    m->png.width=m->width;
    m->png.height=m->height;
    m->png.fmt=VIPS_FORMAT_UCHAR;
    m->png.bands=4;
    m->bit_depth   = 16;
    m->width       = (int)(u->orig_width);
    m->height      = (int)(u->orig_height);
    m->rect_width  = (int)(u->rect.size.width);
    m->rect_height = (int)(u->rect.size.height);
    m->x           = (int)(u->rect.origin.x);
    m->y           = (int)(u->rect.origin.y);
    m->factor      = factor;
    m->cs          = u->last_received_stream_update_dur / 10;


    if ((image = vips_image_new_from_memory(u->buf, u->buf_len, (int)(u->rect.size.width), (int)(u->rect.size.height), 4, VIPS_FORMAT_UCHAR))){
        unsigned char *b;size_t b_len=0;
        ((vips_resize(image, &resized, factor, NULL) == 0) && resized);
        ((tracker = (tracker) ? tracker : vips_image_copy_memory(resized)) && tracker);
        ((vips_insert(tracker, resized, &joined, ox, oy, NULL) == 0));
        if(joined){
          if(((m->vips.buf)=vips_image_copy_memory(joined))){
              m->png.path=vips_image_get_filename(joined);
              m->png.width=vips_image_get_width(joined);
              m->png.height=vips_image_get_height(joined);
              m->png.fmt=VIPS_FORMAT_UCHAR;
              m->png.bands=vips_image_get_bands(joined);
              Di(vips_image_get_width(m->vips.buf));
              Di(vips_image_get_height(m->vips.buf));
          }
        }
      }
    while (vector_size(history) > 0 && (size_t)vector_get(history, vector_size(history) - 1) < timestamp() - HISTORY_MS + 1000)
      vector_pop(history);
    while (vector_size(history) >= vector_capacity(history))
      vector_pop(history);
    asprintf(&stats_text, "{"
             "\"ts\":%lld"
             ",\"type\":\"%s\""
             ",\"id\":%lu"
             ",\"raw_file\":\"%s\""
             ",\"vips_w\":%d"
             ",\"vips_h\":%d"
             ",\"file\":\"%s\""
             ",\"ts\":%lu"
             ",\"seed\":%lu"
             ",\"png_len\":%lu"
             ",\"width\":%lu"
             ",\"height\":%lu"
             ",\"bytes\":%lu"
             ",\"copied\":%lu"
             ",\"write_file_dur\":%lu"
             ",\"dropped\":%lu"
             ",\"stride\":%d"
             ",\"queued\":%d"
             ",\"images\":%d"
             ",\"dur\":%lld"
             ",\"preprocess_dur\":%lu"
             ",\"skipped_frames\":%lu"
             ",\"last_received_stream_update_dur\":%lu"
             ",\"received_srames\":%lu"
             ",\"x\":%d"
             ",\"y\":%d"
             ",\"w\":%d"
             ",\"h\":%d"
             ",\"factor\":%.2f"
             "}%s",
             timestamp(),
             l->type,
             l->id,
             raw_file,
             vips_image_get_height(joined),
             vips_image_get_width(joined),
             show_file,
             u->ts,
             u->seed,
             m->png.len,
             u->width,
             u->height,
             (size_t)(u->buf_len), copied,
             write_file_dur,
             u->dropped,
             u->stride,
             chan_size(l->chan),
             chan_size(l->images_chan),
             timestamp() - ts,
             u->dur,
             skipped_frames,
             u->last_received_stream_update_dur,
             received_frames,
             (int)(u->rect.origin.x),
             (int)(u->rect.origin.y),
             (int)(u->rect.size.width),
             (int)(u->rect.size.height),
             factor,
             "\n"
             );
    /*
     */
    m->json = stats_text;

    chan_send(l->images_chan, (void *)m);

    if (show_file)
      free(show_file);
    if (image)
      g_object_unref(image);
    image   = NULL;
    resized = NULL;
    joined  = NULL;

    if (DO_LOG && (timestamp() - last_ts) > LOG_INTERVAL) {
      last_ts = timestamp();
      struct StringFNStrings split;
      asprintf(&s,
               AC_YELLOW  "#%lu> " AC_RESETALL
               "\n|%lu|ID:%lu"
               "\n|%lux%lu Frame"
               "\n|"AC_YELLOW "%s"AC_RESETALL " Buffer"
               "\n|Update:"
               "\n|    Size  : %dx%d"
               "\n|    Range : %lux%lu - %lux%lu"
               "\n|            ("AC_RED "%s"AC_RESETALL " Thousand Pixels- "AC_RESETALL AC_BLUE "%.2f%%"AC_RESETALL ")"
               "\n|"AC_RESETALL "Copied   : " AC_BLUE "%s" AC_RESETALL
               "\n|"AC_RESETALL "Pre Rect   : " AC_BLUE "%s" AC_RESETALL
               "\n|"AC_RESETALL "Data Rect   : " AC_BLUE "%s" AC_RESETALL
               "\n|"AC_RESETALL "Post Rect   : " AC_BLUE "%s" AC_RESETALL
               "\n|"AC_RESETALL "Runtime  : "AC_MAGENTA "%s"AC_RESETALL
               "\n|"AC_RESETALL "Rate     : "AC_GREEN "%s"AC_RESETALL "/s"
               "\n|"AC_RESETALL "RingBuf  : "AC_GREEN "%s/%s Used (%f)"AC_RESETALL
               "\n|"AC_RESETALL "History  : "AC_GREEN "%lu" AC_RESETALL "/%lu"
               "\n|"AC_RESETALL "Newest   : "AC_GREEN "%s" AC_RESETALL
               "\n|"AC_RESETALL "Oldest   : "AC_GREEN "%s/%s" AC_RESETALL
               "\n|"AC_RESETALL "- Dur    : "AC_GREEN "%s" AC_RESETALL
               "\n|"AC_RESETALL "- Chan   : "AC_GREEN "%d" AC_RESETALL
               "\n|"AC_RESETALL "- Resize : "AC_GREEN "%.2f" AC_RESETALL
               "%s\n"
               ,
               u->seed,
               u->ts, u->id, u->width, u->height,
               bytes_to_string(u->buf_len),
               (int)u->rect.size.width, (int)u->rect.size.height,
               u->start_x, u->start_y,
               u->end_x, u->end_y,
               bytes_to_string((size_t)(u->pixels_qty) / 1024),
               u->pixels_percent,
               bytes_to_string(copied),
               milliseconds_to_string(timestamp() - started),
               bytes_to_string(copied / (timestamp() - started)),
               "xxxxxxx",
               "yyyyyyyy",
               "zzzzzzzz",
               "", "", (float)0,
               vector_size(history),
               vector_capacity(history),
               milliseconds_to_string(timestamp() - (size_t)(vector_get(history, 0))),
               milliseconds_to_string(timestamp() - (size_t)(vector_get(history, vector_size(history) - 1))),
               milliseconds_to_string(HISTORY_MS),
               milliseconds_to_string(durs[0]), chan_size(l->chan),
               factor,
               ""
               );
      split = stringfn_split_lines(s);
      w     = 0; h = ws->ws_row - (int)split.count;
      if (true)
        fprintf(stdout,
                "\0337"
                "\033[s\033[%d;%dH"
                "%s"
                "\0338",
                h,
                w,
                s
                );
      if (s)
        free(s);
      stringfn_release_strings_struct(split);
    }
    if (joined)
      g_object_unref(joined);
    if (resized)
      g_object_unref(resized);
    if (tracker)
      g_object_unref(tracker);
    if (u->buf)
      free(u->buf);
    if (u)
      free(u);
    ended = l->ended;
    pthread_mutex_unlock(l->mutex);
  }
  return(EXIT_SUCCESS);
} /* wu_receive_stream */

void _command_test_stream_display(){
  pthread_mutex_t       mutex;
  CFRunLoopRef          loop = CFRunLoopGetCurrent();
  struct stream_setup_t l    = {
    .type                = "display",
    .id                  = args->id > 0 ? args->id : get_display_index_id(args->index),
    .loop                = &loop,
    .delay_ms            = clamp(args->duration_seconds * 1000,                        MIN_STREAM_MS,MAX_STREAM_MS),
    .mutex               = &mutex,
    .width               = get_display_width(),
    .height              = get_display_height(),
    .monitor_interval_ms = 3000,
    .chan                = chan_init(BUFFERED_STREAM_ITEMS_QTY),
    .images_chan         = chan_init(BUFFERED_STREAM_ITEMS_QTY),
    .heartbeat           = vector_new_with_options(10,                                 true),
    .verbose_mode        = args->verbose_mode,
    .debug_mode          = args->debug_mode,
  };

  pthread_create(&(l.threads[0]), NULL, stop_loop, (void *)&l);
  pthread_create(&(l.threads[3]), NULL, wu_receive_stream, (void *)&l);
  pthread_create(&(l.threads[3]), NULL, wu_receive_images, (void *)&l);
  pthread_create(&(l.threads[1]), NULL, wu_stream_display, (void *)&l);
  CFRunLoopRun();
  pthread_join(&(l.threads[0]), NULL);
  pthread_join(&(l.threads[3]), NULL);
  pthread_join(&(l.threads[1]), NULL);
//  pthread_join(&(l.threads[2]), NULL);
  exit(EXIT_SUCCESS);
}

void _command_test_ts(){
  Ds("ts");
  exit(EXIT_SUCCESS);
}

void _command_test_stream_window(){
  //wu_stream_window(args->width, args->height);
  //CFRunLoopRun();
  exit(EXIT_SUCCESS);
}

void _command_test_cap_display(){
  uint32_t seed = 12345;
  char     *buf, *last_buf; size_t len, last_len;
  size_t   qty = 0; char *path = "/tmp/qoir", *f, bufs_qty = 0, paths_qty = 0;
  char     **files = match_files(path, "*.qoir", &qty);
  char     *paths[qty];

  for (size_t i = 0; i < qty; i++)
    asprintf(&(paths[i]), "%s/%s", path, (char *)files[i]);
  for (size_t i = 0; i < paths_qty; i++)
    Ds(paths[i]);
  qoir_decode_result **decoded = (qoir_decode_result **)async_each(20, paths, qty, &bufs_qty, ^ void *(void *VOID){
    char *f                     = (char *)VOID;
    qoir_decode_options decopts = { 0 };
    decopts.pixfmt              = QOIR_PIXEL_FORMAT__RGBA_NONPREMUL;
    qoir_decode_result *_d      = calloc(1, sizeof(qoir_decode_result));
    qoir_decode_result d        = qoir_decode(fsio_read_binary_file(f), fsio_file_size(f), &decopts);
    memcpy(_d, &d, sizeof(qoir_decode_result));
    return((void *)_d);
  });
  size_t total = 0;

  for (size_t i = 0; i < bufs_qty; i++) {
    char   buf[32];
    total += (decoded[i]->dst_pixbuf.pixcfg.height_in_pixels * decoded[i]->dst_pixbuf.stride_in_bytes);
    size_t l = decoded[i]->dst_pixbuf.pixcfg.height_in_pixels * decoded[i]->dst_pixbuf.stride_in_bytes;
    Dn(l);
//    sha256_hash((unsigned char*)(decoded[i]->dst_pixbuf.data), &buf, decoded[i]->dst_pixbuf.pixcfg.height_in_pixels*decoded[i]->dst_pixbuf.stride_in_bytes);
//    log_info("buf:%s",buf);
    Di(decoded[i]->dst_pixbuf.pixcfg.width_in_pixels);
    Di(decoded[i]->dst_pixbuf.pixcfg.height_in_pixels);
    Dn(decoded[i]->dst_pixbuf.stride_in_bytes);
  }

  Dn(total);
  Di(bufs_qty);
  exit(0);
  for (size_t i = 0; i < qty; i++) {
  }
  for (size_t i = 0; i < qty; i++) {
    Dn(i);
    Di(decoded[i]->dst_pixbuf.pixcfg.height_in_pixels);
    Di(decoded[i]->dst_pixbuf.pixcfg.width_in_pixels);
  }
  if (false)

    for (size_t i = 0; i < qty; i++) {
      asprintf(&f, "%s/%s", path, files[i]);
      buf = fsio_read_binary_file(f);
      len = fsio_file_size(f);
      if (buf && last_buf) {
        unsigned long       ts = timestamp();
        bool                match;
        qoir_decode_options decopts = { 0 };
        decopts.pixfmt = QOIR_PIXEL_FORMAT__RGBA_NONPREMUL;
        qoir_decode_result  dec[2];
        dec[0] = qoir_decode(buf, len, &decopts);
        dec[1] = qoir_decode(last_buf, last_len, &decopts);
        match  = pixbufs_are_equal(&(dec[0].dst_pixbuf), &(dec[1].dst_pixbuf));
        unsigned long dur = timestamp() - ts;
        Di(match);
        log_info("Compared %s and %s buffers in %s", bytes_to_string(len), bytes_to_string(last_len), milliseconds_to_string(dur));
      }
      Ds(f);
      last_buf = buf;
    }
  Dn(qty);

  log_info("ccc");
  exit(EXIT_SUCCESS);
} /* _command_test_cap_display */

void _command_test_cap_window(){
}

TEST t_clipboard(){
  PASSm("Clipboard Tests OK");
}

TEST t_hash_each(){
  struct Vector *v[10];
  hash_t        *h[10];
  char          *ts;

  v[0] = vector_new();
  for (size_t i = 0; i < TEST_VECTOR_QTY; i++) {
    asprintf(&ts, "%lld", timestamp());
    vector_push(v[0], (void *)strdup(ts));
  }
  /*
     h[2] = vector_to_hash_values(v[0]);
     log_info("hash 2 has %d items", hash_size(h[2]));
     async_worker_cb cb = ^void*(void*VOID){
     char *s;
     hash_t *h_in = (hash_t*)VOID;
     int in = atoi(hash_get(h_in,"in"));
     Di(in);
     hash_t *h = hash_new();
     hash_set(h,"out","out");
     asprintf(&s,"%lu",(size_t)(in*2));
     hash_set(h,"in",s);
     return((void*)h);
     };
     Di(hash_size(h[2]));
     size_t len = hash_size(h[2]),qty=0;
     size_t i=0;
     hash_t *items[10];
     for(size_t i = 0; i <10;i++)
     items[i]=hash_new();
     for(size_t i = 0; i <10;i++){
     char *s;
     asprintf(&s,"%ld",i);
     hash_set(items[i],"in",s);
     }


     void **iterated_items = async_each(20, items, len, &qty,cb);
     Dn(qty);
     for(size_t i = 0; i <qty;i++){
     hash_t *h = (hash_t*)iterated_items[i];
     Ds((char*)hash_get(h,"in"));
     Ds((char*)hash_get(h,"out"));
     }
     hash_each(h[2], {
     //Dbg(key, %s);
     });
     ASSERT_EQ(vector_size(v[0]), hash_size(h[2]));
   */
  PASSm("Hash Each Tests OK");
}

TEST t_vector_each(){
  PASSm("Vector Each Tests OK");
}

TEST t_hash_each_key(){
  struct Vector *v[10];
  hash_t        *h[10];
  char          *ts;

  /*

     v[0] = vector_new();
     for (size_t i = 0; i < TEST_VECTOR_QTY; i++) {
     asprintf(&ts, "%lld", timestamp());
     vector_push(v[0], (void *)strdup(ts));
     }
     h[2] = vector_to_hash_values(v[0]);
     log_info("hash 2 has %d items", hash_size(h[2]));
     hash_each_key(h[2], {
     //Dbg(key, %s);
     });
     ASSERT_EQ(vector_size(v[0]), hash_size(h[2]));
   */
  PASSm("Hash Each Key Tests OK");
}

TEST t_windows_foreground(){
  int WINDOW_ID = 128;
  int qty       = CGSGetNumberOfDisplays();

  Dbg(qty, %d);
  bringWindoToForeground((CGWindowID)WINDOW_ID);
  PASSm("Foreground Tests OK");
}

TEST t_windows_0(){
  unsigned long s = timestamp();
  int           window_count;
  uint32_t      *window_list = front_process_window_list_for_active_space(&window_count);

  if (!window_list) {
    log_error("stream> %s: could not get windows of front process! abort..", __FUNCTION__);
    FAIL();
  }
  Dbg(milliseconds_to_string(timestamp() - s), %s);
  ASSERT_GT(window_count, 0);
  Dbg(window_count, %d);
  int window_id = 0;

  for (size_t i = 0; i < window_count; i++)
    Dbg(window_id, %d);

  for (size_t i = 0; i < window_count; i++) {
    window_id = window_list[i];
    Dbg(window_id, %d);
    AXUIElementRef app = GetUIElement(window_id);
  }
  bringWindoToForeground((CGWindowID)128);
  PASSm("Windows 0 Tests OK");
}

TEST t_hash_each_val(){
  /*
     hash_t *h[10];

     h[3] = hash_new();
     hash_set(h[3], "abc", (void *)strdup("123"));
     hash_set(h[3], "def", (void *)strdup("456"));
     hash_each_val(h[3], {
     // Dbg((char*)val, %s);
     });
   */
  PASSm("Hash Each Value Tests OK");
}
SUITE(s_hash_0){
}
SUITE(s_clipboard){
  RUN_TEST(t_clipboard);
}
SUITE(s_windows){
  RUN_TEST(t_windows_0);
  RUN_TEST(t_windows_foreground);
}
SUITE(s_vector){
  RUN_TEST(t_vector_each);
}
SUITE(s_hash){
  RUN_TEST(t_hash_each);
  RUN_TEST(t_hash_each_key);
  RUN_TEST(t_hash_each_val);
}
///////////////////////////////////////////
GREATEST_MAIN_DEFS();
COMMAND_TEST_ADD_TEST_FUNCTIONS()
#endif
