////////////////////////////////////////////
#include "wrec-capture/wrec-capture.h"
#include "wrec-common/wrec-common.h"
#include "wrec-image-test/wrec-image-test.h"
#include "wrec-image/wrec-image.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bench/bench.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
static const char *tempdir = NULL;

////////////////////////////////////////////
TEST t_wrec_image_test(void){
  MEASURE(measurement_capture_and_process_screenshot_rgb_data)

  struct Vector *window_ids = get_windows_ids();
  {
    ASSERT_GTEm("Failed to get window ids", vector_size(window_ids), 1);
  }
  unsigned long                       window_id;
  size_t                              windows_qty = vector_size(window_ids), captured_images_bytes = 0,
                                      captured_images = 0, captured_size, dur;
  unsigned long                       started_ms = timestamp();
  char                                *qoi_file_path, *SCREENSHOT_FILE, *tempdir;
  size_t                              capture_dur, capture_started;
  bool                                capture_ok = false;
  void                                *qoi_encoded_data;
  int                                 qoi_decode_file_result;
  struct qoi_encode_to_file_result_t  *qoi_encode_to_file_result;
  struct loaded_png_file_t            *loaded_png;
  struct qoi_encode_to_file_request_t *qoi_file_req;
  {
  }

  for (size_t i = 0; i < windows_qty; i++) {
    {
      window_id = (unsigned long)vector_get(window_ids, i);
      asprintf(&qoi_file_path, "%swindow-%lu-%lu-wrec-image-test-qoi-encode.qoi",
               tempdir, window_id, (size_t)timestamp()
               );
      asprintf(&SCREENSHOT_FILE, "%swindow-%lu-%lu-wrec-image-test.png",
               tempdir, window_id, (size_t)timestamp()
               );
    }
    {
      capture_started = (size_t)timestamp();
      capture_ok      = capture_to_file_image(window_id, SCREENSHOT_FILE);
      capture_dur     = timestamp() - (size_t)capture_started;
    }
    ASSERT_EQm("Failed to take screenshot", capture_ok, true);
    {
      captured_size = fsio_file_size(SCREENSHOT_FILE);
      captured_images++;
      captured_images_bytes += captured_size;
    }
    log_info("Captured %s Screenshot file %s in %s",
             bytes_to_string(captured_size), SCREENSHOT_FILE,
             milliseconds_to_string(capture_dur)
             );
    {
      loaded_png = load_png_file_pixels(SCREENSHOT_FILE);
    }
    ASSERT_GTEm("Unable to load png file with stb", loaded_png->image_dimensions.width, 10);
    {
      qoi_file_req                   = calloc(1, sizeof(struct qoi_encode_to_file_request_t));
      qoi_file_req->desc             = calloc(1, sizeof(struct qoi_desc));
      qoi_file_req->qoi_file_path    = qoi_file_path;
      qoi_file_req->rgb_pixels       = loaded_png->rgb_pixels;
      qoi_file_req->desc->width      = loaded_png->image_dimensions.width;
      qoi_file_req->desc->height     = loaded_png->image_dimensions.height;
      qoi_file_req->desc->channels   = 4;
      qoi_file_req->desc->colorspace = QOI_SRGB;
      qoi_encode_to_file_result      = qoi_encode_to_file(qoi_file_req);
    }
    ASSERT_EQm("Failed to encode qoi file", qoi_encode_to_file_result, 0);
    {
      qoi_decode_file_result = qoi_decode_file(qoi_file_path);
    }
    ASSERT_EQm("Failed to decode qoi file", qoi_decode_file_result, 0);
    {
      qoi_encoded_data = qoi_encode_memory(loaded_png->rgb_pixels, loaded_png->image_dimensions.width, loaded_png->image_dimensions.height);
    }
    ASSERT_NEQm("Failed to encode qoi to memory", qoi_encoded_data, NULL);
    {
//        decoded_rgb_from_qoi = qoi_decode_memory(qoi_encoded_,
    }
    {
      free(qoi_file_req->desc);
      free(qoi_file_req);
      free(loaded_png);
      free(SCREENSHOT_FILE);
    }
  }
  dur = timestamp() - (size_t)started_ms;
  log_info(AC_GREEN AC_BOLD "Captured %lu Images totaling %s from %lu windows in %s" AC_RESETALL,
           captured_images, bytes_to_string(captured_images_bytes),
           windows_qty,
           milliseconds_to_string(dur)
           );
  END_MEASURE(measurement_capture_and_process_screenshot_rgb_data)
  MEASURE_SUMMARY(measurement_capture_and_process_screenshot_rgb_data);
  PASS();
} /* t_wrec_image_test */

SUITE(s_wrec_image_test) {
  RUN_TEST(t_wrec_image_test);
}

GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  asprintf(&tempdir, "/tmp/wrec-image-test/%d", getpid());
  if (fsio_dir_exists(tempdir) == false) {
    fsio_mkdirs(tempdir, 0700);
  }
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_wrec_image_test);
  GREATEST_MAIN_END();
}
