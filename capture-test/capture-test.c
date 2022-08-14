#define DEBUG_MODE    false
#include "dbg.h"
//#include "debug-memory/debug_memory.h"
#include "bench/bench.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "capture-test.h"
#include "generic-print/print.h"
#include "ms/ms.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
static const char *tempdir_path = NULL;

/*
 * ////////////////////////////////////////
 *  BENCHMARK_QTY(benchmark_name, 3)
 *  //DO_WORK
 *  END_BENCHMARK(benchmark_name)
 *  BENCHMARK_SUMMARY(benchmark_name);
 * ////////////////////////////////////////
 *  MEASURE(measurement_name)
 *  //DO_WORK
 *  END_MEASURE(measurement_name)
 *  MEASURE_SUMMARY(measurement_name);
 * ////////////////////////////////////////
 */

TEST t_capture_images(void){
  MEASURE(measurement_window_ids)
  struct Vector *window_ids = get_windows_ids();
  END_MEASURE(measurement_window_ids)
  MEASURE_SUMMARY(measurement_window_ids);

  dbg(vector_size(window_ids), %u);
  ASSERT_GTEm("Failed to get window ids", vector_size(window_ids), 1);
  unsigned long window_id;
  MEASURE(measurement_capture_window_ids)
  size_t qty = vector_size(window_ids);
  size_t        captured_images_bytes = 0;
  unsigned long started_ms            = timestamp();
  for (int i = 0; i < qty; i++) {
    window_id = (unsigned long)vector_get(window_ids, i);
    char *SCREENSHOT_FILE;
    asprintf(&SCREENSHOT_FILE, "%s/window-%lu.png", tempdir_path, window_id);
    dbg(SCREENSHOT_FILE, %s);
    bool ok = capture_to_file_image(window_id, SCREENSHOT_FILE);
    captured_images_bytes += fsio_file_size(SCREENSHOT_FILE);
    ASSERT_EQm("Failed to take screenshot", ok, true);
    free(SCREENSHOT_FILE);
  }
  unsigned long dur_ms = timestamp() - started_ms;
  char          *msg;
  asprintf(&msg, "Captured %lu Window ID Screenshots totaling %s in %s",
           qty,
           bytes_to_string(captured_images_bytes),
           milliseconds_to_string(dur_ms)
           );
  fprintf(stdout, AC_RESETALL AC_YELLOW AC_REVERSED "%s" AC_RESETALL "\n", msg);
  free(msg);
  END_MEASURE(measurement_capture_window_ids)
  MEASURE_SUMMARY(measurement_capture_window_ids);
  PASS();
}

SUITE(s_capture){
  RUN_TEST(t_capture_images);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  if (tempdir_path == NULL) {
    tempdir_path = gettempdir();
  }
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_capture);
  GREATEST_MAIN_END();
  return(0);
}
