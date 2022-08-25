#define DEBUG_MODE    false
#include "timestamp/timestamp.h"
#include "wrec-capture-test.h"
static const char *tempdir = NULL;
static char *msg;

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

char *generate_window_id_screenshot_file_path(unsigned long WINDOW_ID){
  char *SCREENSHOT_FILE = NULL;

  if (getenv("SCREENSHOT") != NULL) {
    asprintf(&SCREENSHOT_FILE, "%s-%lu.png", getenv("SCREENSHOT"), WINDOW_ID);
  }else{
    asprintf(&SCREENSHOT_FILE, "%s/%lu/%ld.png", tempdir, WINDOW_ID, (size_t)timestamp());
  }
  if (fsio_dir_exists(dirname(SCREENSHOT_FILE)) == false) {
    fsio_mkdirs(dirname(SCREENSHOT_FILE), 0700);
  }
  return(SCREENSHOT_FILE);
}

TEST t_capture_all_windows_to_png_files(void *WINDOWS){
  struct window_t *w;
  unsigned long   started_ms = timestamp(), dur_ms;
  size_t          qty, captured_images_bytes = 0;
  char            *SCREENSHOT_FILE;
  bool            ok = false;

  struct Vector   *windows = (struct Vector *)WINDOWS;

  ASSERT_GTEm("Failed to receive valid window ids vector", vector_size(windows), 1);
  qty = vector_size(windows);
  for (size_t i = 0; i < qty; i++) {
    w = (struct window_t *)vector_get(windows, i);
    if (!w || w->window_id < 1 || w->layer >= 24) {
      continue;
    }
    SCREENSHOT_FILE = generate_window_id_screenshot_file_path(w->window_id);
    ok              = capture_to_file_image(w->window_id, SCREENSHOT_FILE);
    if (ok == false) {
      struct Vector *tmp_window_ids = get_window_ids();
      bool          tmp_found       = false;
      for (size_t _i = 0; _i < vector_size(tmp_window_ids); _i++) {
        if ((size_t)w->window_id == (size_t)vector_get(tmp_window_ids, _i)) {
          tmp_found = true;
        }
      }
      vector_release(tmp_window_ids);
      if (tmp_found == false) {
        continue;
      }
    }
    ASSERT_EQm("Failed to take screenshot", ok, true);
    ok = fsio_file_exists(SCREENSHOT_FILE);
    ASSERT_EQm("Failed to locate screenshot file", ok, true);
    captured_images_bytes += fsio_file_size(SCREENSHOT_FILE);
    ASSERT_GTEm("Failed to locate valid screenshot file", captured_images_bytes, 1);
    printf("%s\n", SCREENSHOT_FILE);
  }
  if (SCREENSHOT_FILE) {
    free(SCREENSHOT_FILE);
  }
  dur_ms = timestamp() - started_ms;

  asprintf(&msg, "Captured %lu Window ID Screenshots totaling %s in %s",
           qty,
           bytes_to_string(captured_images_bytes),
           milliseconds_to_string(dur_ms)
           );
  PASSm(msg);
} /* t_capture_all_windows_to_png_files */

SUITE(s_droid){
  RUN_TEST(t_droid);
}
SUITE(s_shapes){
  RUN_TEST(t_shapes);
}
SUITE(s_frog){
  RUN_TEST(t_frog);
}
SUITE(s_capture_all_windows_to_png_files){
  struct Vector *windows = get_windows();

  RUN_TESTp(t_capture_all_windows_to_png_files, (void *)windows);
  vector_release(windows);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  tempdir = NULL;
  if (getenv("STORAGE_DIR") != NULL) {
    asprintf(&tempdir, "%s", getenv("STORAGE_DIR"));
  }
  if (tempdir == NULL) {
    asprintf(&tempdir, "/tmp/wrec-image-test/%d", getpid());
  }
  if (fsio_dir_exists(tempdir) == false) {
    fsio_mkdirs(tempdir, 0700);
  }
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_capture_all_windows_to_png_files);
  /*
   * RUN_SUITE(s_frog);
   * RUN_SUITE(s_droid);
   * RUN_SUITE(s_shapes);
   */
  GREATEST_MAIN_END();
  return(0);
}
