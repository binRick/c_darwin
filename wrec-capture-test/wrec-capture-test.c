#define DEBUG_MODE    false
#include "bench/bench.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "generic-print/print.h"
#include "ms/ms.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "wrec-capture-test.h"
static const char *tempdir_path = NULL;

TEST t_capture_images(void){
  MEASURE(measurement_window_ids)
  struct Vector *window_ids = get_windows_ids();
  END_MEASURE(measurement_window_ids)
  MEASURE_SUMMARY(measurement_window_ids);

  ASSERT_GTEm("Failed to get window ids", vector_size(window_ids), 1);
  unsigned long window_id;
  MEASURE(measurement_capture_window_ids)
  size_t qty = vector_size(window_ids);
  size_t        captured_images_bytes = 0;
  unsigned long started_ms            = timestamp();
  for (size_t i = 0; i < qty; i++) {
    window_id = (unsigned long)vector_get(window_ids, i);
    char *SCREENSHOT_FILE;
    asprintf(&SCREENSHOT_FILE, "%s/window-%lu.png", tempdir_path, window_id);
    printf("%s\n", SCREENSHOT_FILE);
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
