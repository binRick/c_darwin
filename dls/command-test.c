#pragma once
#ifndef DLS_TEST_COMMANDS_C
#define DLS_TEST_COMMANDS_C
#define TEST_VECTOR_QTY    5
#define MIN_STREAM_MS      1000
#define MAX_STREAM_MS      600000
#include "core/core.h"
#include "dls/command-test.h"
#include "dls/dls.h"
#include "submodules/c_deps/submodules/c_greatest/greatest/greatest.h"
#include "window/utils/utils.h"
extern int  DLS_EXECUTABLE_ARGC;
extern char **DLS_EXECUTABLE_ARGV;
///////////////////////////////////////////

int stop_loop(void *L){
  struct stream_setup_t *l = (struct stream_setup_t *)L;

  pthread_mutex_lock(l->mutex);
  size_t delay_ms = l->delay_ms;

  pthread_mutex_unlock(l->mutex);
  usleep(1000 * delay_ms);
  pthread_mutex_lock(l->mutex);
  l->ended = true;
  CFRunLoopStop(l->loop);
  pthread_mutex_unlock(l->mutex);
  return(EXIT_SUCCESS);
}

#define DO_LOG true
#define LOG_INTERVAL 50
#define MAX_HISTORY_SIZE 500
#define HISTORY_MS 10000
int wu_receive_stream(void *L){
  struct stream_setup_t *l = (struct stream_setup_t *)L;
  bool                  ended;
  struct winsize *ws = get_terminal_size();
  void   **msg;
  CGRect rect;
  int width = ws->ws_col,height=ws->ws_row, w, h;
  size_t qty=0, copied=0;
  char *s;
  ringbuf_t rb = ringbuf_new(1024*1024*128);

  pthread_mutex_lock(l->mutex);
  ended = l->ended;
  pthread_mutex_unlock(l->mutex);
  unsigned long started = timestamp(), last_ts = timestamp();
  struct Vector *history = vector_new_with_options(MAX_HISTORY_SIZE, false);
  while (!ended && chan_recv(l->chan, &msg) == 0) {
    struct stream_update_t *u = (struct stream_update_t *)msg;
    if (!u) continue;
    qty++;
    copied += u->buf_len;
    while(vector_size(history)>0 && (size_t)vector_get(history,vector_size(history)-1) < timestamp()-HISTORY_MS + 1000)
      vector_pop(history);
    while(vector_size(history)>=vector_capacity(history))
      vector_pop(history);
    vector_prepend(history,(void*)u->ts);
    if(DO_LOG && (timestamp() - last_ts) > LOG_INTERVAL){
      last_ts = timestamp();
      struct StringFNStrings split;
      if(false)
        ringbuf_memcpy_into(rb,u->buf,u->buf_len);
      asprintf(&s,
       AC_YELLOW  "#%lu> " AC_RESETALL
        "\n|%lu|ID:%lu"
        "\n|%lux%lu Frame"
        "\n|"AC_YELLOW"%s"AC_RESETALL" Buffer"
        "\n|Update:"
        "\n|    Size  : %dx%d"
        "\n|    Range : %lux%lu - %lux%lu"
        "\n|            ("AC_RED"%s"AC_RESETALL" Pixels- "AC_RESETALL AC_BLUE"%.2f%%"AC_RESETALL")"
        "\n|"AC_RESETALL "Copied   : " AC_BLUE "%s" AC_RESETALL 
        "\n|"AC_RESETALL "Runtime  : "AC_MAGENTA"%s"AC_RESETALL
        "\n|"AC_RESETALL "Rate     : "AC_GREEN"%s"AC_RESETALL"/s" 
        "\n|"AC_RESETALL "RingBuf  : "AC_GREEN"%s/%s Used (%f)"AC_RESETALL 
        "\n|"AC_RESETALL "History  : "AC_GREEN"%lu" AC_RESETALL "/%lu"
        "\n|"AC_RESETALL "Newest   : "AC_GREEN"%s" AC_RESETALL
        "\n|"AC_RESETALL "Oldest   : "AC_GREEN"%s/%s" AC_RESETALL
        "%s\n"
        ,
        u->seed,
        u->ts, u->id, u->width, u->height,
        bytes_to_string(u->buf_len),
        (int)u->rect.size.width, (int)u->rect.size.height,
        u->start_x, u->start_y,
        u->end_x, u->end_y, 
        bytes_to_string(u->pixels_qty),
        u->pixels_percent,
        bytes_to_string(copied),
        milliseconds_to_string(timestamp() -started),
        bytes_to_string(copied / (timestamp()-started)),
        bytes_to_string(ringbuf_bytes_used(rb)),
        bytes_to_string(ringbuf_capacity(rb)),
        (float)ringbuf_bytes_used(rb)/(float)ringbuf_capacity(rb),
        vector_size(history),
        vector_capacity(history),
        milliseconds_to_string(timestamp() - (size_t)(vector_get(history,0))),
        milliseconds_to_string(timestamp() - (size_t)(vector_get(history,vector_size(history)-1))),
        milliseconds_to_string(HISTORY_MS),
        ""
        );
      split = stringfn_split_lines(s);
      w = 0; h = ws->ws_row - (int)split.count;
      fprintf(stdout,
           AC_CLS
           "\0337"
           "\033[s\033[%d;%dH"
           "%s"
           "\0338",
           h,
           w,
           s
      );
      if(s)free(s);
      stringfn_release_strings_struct(split);
    }
    if (u->buf) free(u->buf);
    if (u) free(u);
    pthread_mutex_lock(l->mutex);
    ended = l->ended;
    pthread_mutex_unlock(l->mutex);
  }
  return(EXIT_SUCCESS);
}

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

  pthread_create(&(l.threads[3]), NULL, wu_receive_stream, (void *)&l);
//  pthread_create(&(l.threads[2]), NULL, wu_monitor_stream, (void *)&l);
  pthread_create(&(l.threads[0]), NULL, stop_loop, (void *)&l);
  pthread_create(&(l.threads[1]), NULL, wu_stream_active_display, (void *)&l);
  CFRunLoopRun();
  pthread_join(&(l.threads[0]), NULL);
  pthread_join(&(l.threads[1]), NULL);
//  pthread_join(&(l.threads[2]), NULL);
  pthread_join(&(l.threads[3]), NULL);
  exit(EXIT_SUCCESS);
}

void _command_test_stream_window(){
  wu_stream_active_window(args->width, args->height);
  CFRunLoopRun();
  exit(EXIT_SUCCESS);
}

void _command_test_cap_display(){
}

void _command_test_cap_window(){
}

TEST t_clipboard(){
   struct vn_init vn; /* FIRST OF ALL DEFINE WINDOW NAME */
   vn.width = 20; 
   vn.height = 2; 
   vn.pos_x = 2; 
   vn.pos_y = 2;

   struct vnc_color white;
   white.is_fore = 0;
   white.color = vn_hex_color("ffffff", white.is_fore);

   struct vnc_color black;
   black.is_fore = 1; /* FOR BACKGROUND */
   black.color = vn_rgb_color(0, 0, 0, black.is_fore);

//   vn_clear(); /* CLEAR THE TERMINAL SCREEN */
   vn_gotoxy(vn.pos_x, vn.pos_y); /* GO TO CERTAIN POSITION */
   vn_print("Hey is this text with color?", white.color, black.color, text_bold);
   vn_end(vn);  
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
