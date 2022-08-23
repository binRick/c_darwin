#define DEBUG_MODE    false
#include "bench/bench.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "generic-print/print.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "wrec-capture-test.h"
static const char *tempdir = NULL;

TEST t_capture_images(void){
  struct Vector *window_ids = get_windows_ids();

  ASSERT_GTEm("Failed to get window ids", vector_size(window_ids), 1);
  unsigned long window_id;

  MEASURE(measurement_capture_window_ids)
  size_t qty = vector_size(window_ids);
  size_t        captured_images_bytes = 0;
  unsigned long started_ms            = timestamp();

  for (size_t i = 0; i < qty; i++) {
    window_id = (unsigned long)vector_get(window_ids, i);
    char *SCREENSHOT_FILE;
    asprintf(&SCREENSHOT_FILE, "%s/window-%lu.png", tempdir, window_id);
    log_info("screenshot file: %s\n", SCREENSHOT_FILE);
    bool ok = capture_to_file_image(window_id, SCREENSHOT_FILE);
    captured_images_bytes += fsio_file_size(SCREENSHOT_FILE);
    ASSERT_EQm("Failed to take screenshot", ok, true);
    free(SCREENSHOT_FILE);
  }
  unsigned long dur_ms = timestamp() - started_ms;

  log_info("Captured %lu Window ID Screenshots totaling %s in %s",
           qty,
           bytes_to_string(captured_images_bytes),
           milliseconds_to_string(dur_ms)
           );
  END_MEASURE(measurement_capture_window_ids)
  MEASURE_SUMMARY(measurement_capture_window_ids);
  PASS();
}

SUITE(s_capture){
  RUN_TEST(t_capture_images);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  asprintf(&tempdir, "/tmp/wrec-image-test/%d", getpid());
  if (fsio_dir_exists(tempdir) == false) {
    fsio_mkdirs(tempdir, 0700);
  }
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_capture);
  GREATEST_MAIN_END();
  return(0);
}
