#define DEBUG_MODE    false
#include "bench/bench.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "wrec-capture-test.h"
#include "wrec-capture/croak.h"
#include "wrec-capture/droid.h"
#include "wrec-capture/frog.h"
#include "wrec-capture/shape.h"
static const char *tempdir = NULL;

TEST t_frog(void){
  Frog *paul  = Frog_new("Paul");
  Frog *steve = Frog_new("Steve");

  VCALL(DYN(Frog, Croak, paul), croak);
  VCALL(DYN(Frog, Croak, steve), croak);
  VCALL(DYN(Frog, Croak, steve), croak);
  VCALL(DYN(Frog, Croak, steve), croak);
  VCALL(DYN(Frog, Croak, steve), croak);
  VCALL(DYN(Frog, Croak, paul), croak);

  Frog_free(paul);
  Frog_free(steve);

  PASS();
}

TEST t_shapes(void){
  Shape r = DYN_LIT(Rectangle, Shape, { 5, 7 });
  Shape t = DYN_LIT(Triangle, Shape, { 10, 20, 30 });

  test(r);
  test(t);

  PASS();
}

TEST t_droid(void){
  Droid c_3po = DYN_LIT(C_3PO, Droid, { 0 });
  Droid r2_d2 = DYN_LIT(R2_D2, Droid, { 0 });

  VCALL_OBJ(c_3po, turn_on);
  VCALL_OBJ(r2_d2, turn_on);

  PASS();
}

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

SUITE(s_droid){
  RUN_TEST(t_droid);
}
SUITE(s_shapes){
  RUN_TEST(t_shapes);
}
SUITE(s_frog){
  RUN_TEST(t_frog);
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
  RUN_SUITE(s_frog);
  RUN_SUITE(s_droid);
  RUN_SUITE(s_shapes);
  GREATEST_MAIN_END();
  return(0);
}
