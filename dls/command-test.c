#pragma once
#ifndef DLS_TEST_COMMANDS_C
#define DLS_TEST_COMMANDS_C
#define TEST_VECTOR_QTY    5
#define MIN_STREAM_MS      1000
#define MAX_STREAM_MS      60000
#include "core/core.h"
#include "dls/command-test.h"
#include "dls/dls.h"
#include "submodules/c_deps/submodules/c_greatest/greatest/greatest.h"
#include "window/utils/utils.h"
extern int  DLS_EXECUTABLE_ARGC;
extern char **DLS_EXECUTABLE_ARGV;
///////////////////////////////////////////
struct stop_loop_t {
  CFRunLoopRef *loop;
  size_t       delay_ms;
};

int stop_loop(void *SL){
  usleep(1000 * (size_t)((struct stop_loop_t *)SL)->delay_ms);
  CFRunLoopStop(*(CFRunLoopRef *)((struct stop_loop_t *)SL)->loop);
  return(EXIT_SUCCESS);
}

void _command_test_stream_display(){
  pthread_t    th;
  CFRunLoopRef loop = CFRunLoopGetCurrent();

  pthread_create(&th, NULL, stop_loop, (void *)&(struct stop_loop_t){
    .loop     = &loop,
    .delay_ms = clamp(args->duration_seconds * 1000, MIN_STREAM_MS, MAX_STREAM_MS),
  });
  wu_stream_active_display(
    args->width,
    args->height,
    args->id > 0 ? args->id : get_display_index_id(args->index)
    );
  CFRunLoopRun();
  pthread_join(&th, NULL);
  CFRelease(loop);
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
