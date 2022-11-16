#pragma once
#ifndef DLS_TEST_COMMANDS_C
#define DLS_TEST_COMMANDS_C
#define TEST_VECTOR_QTY    5
#define MIN_STREAM_MS      1000
#define MAX_STREAM_MS      600000
#include "capture/type/type.h"
#include "sha256.c/sha256.h"
#include "capture/utils/utils.h"
#include "color/color.h"
#include "core/core.h"
#include "dls/command-test.h"
#include "dls/dls.h"
#include "kitty/msg/msg.h"
#include "submodules/c_deps/submodules/c_greatest/greatest/greatest.h"
#include "window/utils/utils.h"
#include "match/match.h"
#include "qoir/src/qoir.h"
#include "qoir/util/pixbufs_are_equal.h"
extern int  DLS_EXECUTABLE_ARGC;
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
  struct Vector *colors_v = color_csv_load(color_csv_read(COLOR_TYPE_BEST), COLOR_FILTER_DARK);

  Dbg(vector_size(colors_v), %lu);
  colors_v = color_csv_load(color_csv_read(COLOR_TYPE_ALL), 0);
  Dbg(vector_size(colors_v), %lu);
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

bool wu_save_png_file(void *L){
  struct stream_setup_t *u = (struct stream_update_t *)L;

  if (vips_pngsave(u->png, u->png_file, NULL)) {
    log_error("Failed to save png file");
    return(false);
  }
  return(true);
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
#define WU_DISPLAY_TERMINAL_IMAGE                  true
#define        WU_DISPLAY_TERMINAL_SCALE_FACTOR    (0.15)
#define        WU_DISPLAY_TERMINAL_WIDTH           550
typedef void (^wu_cb)(void);
#define WU_SAVE_QOIR_FILE true
int wu_receive_stream(void *L){
  float factor = WU_DISPLAY_TERMINAL_SCALE_FACTOR;
  struct winsize        *ws;
  struct stream_setup_t *l = (struct stream_setup_t *)L;
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
  VipsImage *image;
  VipsImage *resized, *tracked;
  VipsImage *tracker = NULL, *out, *img_overlay, *img_show, *joined;

  while (!ended && chan_recv(l->chan, &msg) == 0) {
    unsigned long          ts = timestamp(), durs[10] = { 0 };
    pthread_mutex_lock(l->mutex);
    struct stream_update_t *u = (struct stream_update_t *)msg;
    pthread_mutex_unlock(l->mutex);
    if (!u) continue;
    if (chan_size(l->chan) > 10 && qty > 10) continue;
    if (u->seed < qty) continue;
    qty++;
    copied += u->buf_len;
    if((u->seed%10)==0)
        printf(AC_CLS);
    if(chan_size(l->chan)<5)
        factor = clamp((factor + 0.05), .10, 1.00);
    if(chan_size(l->chan)>30)
        factor = clamp((factor - 0.05), .10, 1.00);
    if (true || qty == 1 || qty % 2 == 0) {
      char  *show_file, *stats_file,*stats_text;

      int   ox     = (int)(factor * (float)(u->width - u->rect.size.width)),
            oy     = (int)(factor * (float)(u->height - u->rect.size.height));
      asprintf(&show_file, "/tmp/display-stream-%lu.qoir", qty);
      asprintf(&stats_file, "/tmp/display-stream-%lu.json", qty);
      asprintf(&stats_text, "{"
          "\"timestamp\":%lld"
          ",\"x\":%d"
          ",\"y\":%d"
          ",\"w\":%d"
          ",\"h\":%d"
          ",\"factor\":%.2f"
          ",\"len\":%lu"
          "}%s",
          timestamp(),
          (int)(u->rect.origin.x),
          (int)(u->rect.origin.y),
          (int)(u->rect.size.width),
          (int)(u->rect.size.height),
          factor,
          (size_t)(u->buf_len),
          "\n"
          );
      if (fsio_file_exists(show_file)) fsio_remove(show_file);
      if (fsio_file_exists(stats_file)) fsio_remove(stats_file);

      if(WU_SAVE_QOIR_FILE){
      if ((image = vips_image_new_from_memory(u->buf, u->buf_len, (int)(u->rect.size.width), (int)(u->rect.size.height), 4, VIPS_FORMAT_UCHAR)))
        if (
          (vips_resize(image, &resized, factor, NULL) || true && resized)
          && (tracker = (tracker) ? tracker : vips_image_copy_memory(resized))
          && (vips_insert(tracker, resized, &joined, ox, oy, NULL) == 0)
          && WU_DISPLAY_TERMINAL_IMAGE
          && (chan_size(l->chan) < 10)
          && (joined && vips_image_write_to_file(joined, show_file, NULL) || true)
          && (fsio_write_text_file(stats_file,stats_text))
          && fsio_file_exists(show_file) && fsio_file_size(show_file) > 1024
          ) {
          if(true)
            printf(AC_CLS);
          durs[0] = timestamp() - ts;
          if(false)
            if (kitty_display_image_path_resized_height(show_file, WU_DISPLAY_TERMINAL_WIDTH))
              printf("\n");
        }
      }
    }
    image   = NULL;
    resized = NULL;
    joined  = NULL;
    while (vector_size(history) > 0 && (size_t)vector_get(history, vector_size(history) - 1) < timestamp() - HISTORY_MS + 1000)
      vector_pop(history);
    while (vector_size(history) >= vector_capacity(history))
      vector_pop(history);
    vector_prepend(history, (void *)u->ts);
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
      if (s) free(s);
      stringfn_release_strings_struct(split);
    }
    pthread_mutex_lock(l->mutex);
    if (u->buf) free(u->buf);
    if (u) free(u);
    ended = l->ended;
    pthread_mutex_unlock(l->mutex);
  }
  return(EXIT_SUCCESS);
} /* wu_receive_stream */

int wu_monitor_stream(void *L){
  struct stream_setup_t *l = (struct stream_setup_t *)L;
  bool                  ended;
  size_t                interval, ts;

  pthread_mutex_lock(l->mutex);
  interval           = l->monitor_interval_ms;
  ended              = l->ended;
  l->last_monitor_ts = timestamp();
  pthread_mutex_unlock(l->mutex);
  while (!ended) {
    usleep(1000 * interval);
    pthread_mutex_lock(l->mutex);
    log_debug("%lu Updates in %s (%.2f/sec)",
              vector_size(l->heartbeat),
              milliseconds_to_string(timestamp() - l->last_monitor_ts),
              (float)((vector_size(l->heartbeat)) / ((float)(timestamp() - l->last_monitor_ts) / 1000))
              );
    for (size_t i = 0; i < vector_size(l->heartbeat); i++) {
      ts = (size_t)vector_pop(l->heartbeat);
      log_debug("Updates: %lu", ts);
    }
    l->last_monitor_ts = timestamp();
    ended              = l->ended;
    pthread_mutex_unlock(l->mutex);
  }
  return(EXIT_SUCCESS);
}

void _command_test_stream_display(){
  pthread_mutex_t       mutex;
  CFRunLoopRef          loop = CFRunLoopGetCurrent();
  struct stream_setup_t l    = {
    .loop                = &loop,
    .delay_ms            = clamp(args->duration_seconds * 1000,                        MIN_STREAM_MS,  MAX_STREAM_MS),
    .mutex               = &mutex,
    .width               = get_display_width(),
    .height              = get_display_height(),
    .monitor_interval_ms = 3000,
    .chan                = chan_init(100),
    .heartbeat           = vector_new_with_options(10,                                 true),
    .id                  = args->id > 0 ? args->id : get_display_index_id(args->index),
    .verbose_mode        = args->verbose_mode,
    .debug_mode          = args->debug_mode,
  };

  pthread_create(&(l.threads[0]), NULL, stop_loop, (void *)&l);
  pthread_create(&(l.threads[3]), NULL, wu_receive_stream, (void *)&l);
//  pthread_create(&(l.threads[2]), NULL, wu_monitor_stream, (void *)&l);
  pthread_create(&(l.threads[1]), NULL, wu_stream_display, (void *)&l);
  CFRunLoopRun();
  pthread_join(&(l.threads[0]), NULL);
  pthread_join(&(l.threads[3]), NULL);
  pthread_join(&(l.threads[1]), NULL);
//  pthread_join(&(l.threads[2]), NULL);
  exit(EXIT_SUCCESS);
}

void _command_test_stream_window(){
  //wu_stream_window(args->width, args->height);
  //CFRunLoopRun();
  exit(EXIT_SUCCESS);
}

void _command_test_cap_display(){
  uint32_t seed = 12345;
  char *buf,*last_buf; size_t len,last_len;
  size_t qty=0;char *path="/tmp/qoir",*f,bufs_qty=0,paths_qty=0;
  char **files = match_files(path,"*.qoir",&qty);
  char *paths[qty];
  for(size_t i = 0; i <qty;i++)
    asprintf(&(paths[i]),"%s/%s",path,(char*)files[i]);
  for(size_t i = 0; i <paths_qty;i++)
    Ds(paths[i]);
  qoir_decode_result **decoded = (qoir_decode_result**)async_each(20,paths,qty,&bufs_qty,^void*(void*VOID){
    char *f=(char*)VOID;
    qoir_decode_options decopts ={ 0 };
    decopts.pixfmt              = QOIR_PIXEL_FORMAT__RGBA_NONPREMUL;
    qoir_decode_result *_d=calloc(1,sizeof(qoir_decode_result));
    qoir_decode_result d = qoir_decode(fsio_read_binary_file(f),fsio_file_size(f),&decopts);
    memcpy(_d,&d,sizeof(qoir_decode_result));
    return((void*)_d);
  });
  size_t total=0;
  for(size_t i = 0; i <bufs_qty;i++){
    char buf[32];
    total+=(decoded[i]->dst_pixbuf.pixcfg.height_in_pixels*decoded[i]->dst_pixbuf.stride_in_bytes);
    size_t l = decoded[i]->dst_pixbuf.pixcfg.height_in_pixels*decoded[i]->dst_pixbuf.stride_in_bytes;
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
  for(size_t i=0;i<qty;i++){
  }
  for(size_t i=0;i<qty;i++){
    Dn(i);
    Di(decoded[i]->dst_pixbuf.pixcfg.height_in_pixels);
    Di(decoded[i]->dst_pixbuf.pixcfg.width_in_pixels);
  }
  if(false){

  for(size_t i=0;i<qty;i++){
    asprintf(&f,"%s/%s",path,files[i]);
    buf=fsio_read_binary_file(f);
    len=fsio_file_size(f);
    if(buf && last_buf){
      unsigned long ts=timestamp();
      bool match;
      qoir_decode_options decopts ={ 0 };
      decopts.pixfmt              = QOIR_PIXEL_FORMAT__RGBA_NONPREMUL;
      qoir_decode_result dec[2];
      dec[0] = qoir_decode(buf,len,&decopts);
      dec[1] = qoir_decode(last_buf,last_len,&decopts);
      match = pixbufs_are_equal(&(dec[0].dst_pixbuf),&(dec[1].dst_pixbuf));
      unsigned long dur=timestamp() - ts;
      Di(match);
      log_info("Compared %s and %s buffers in %s",bytes_to_string(len),bytes_to_string(last_len),milliseconds_to_string(dur));
    }
    Ds(f);
    last_buf=buf;
  }
  }
  Dn(qty);

  log_info("ccc");
  exit(EXIT_SUCCESS);
}

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
  PASSm("Hash Each Tests OK");
}

TEST t_vector_each(){
  PASSm("Vector Each Tests OK");
}

TEST t_hash_each_key(){
  struct Vector *v[10];
  hash_t        *h[10];
  char          *ts;

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
  hash_t *h[10];

  h[3] = hash_new();
  hash_set(h[3], "abc", (void *)strdup("123"));
  hash_set(h[3], "def", (void *)strdup("456"));
  hash_each_val(h[3], {
    // Dbg((char*)val, %s);
  });
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
