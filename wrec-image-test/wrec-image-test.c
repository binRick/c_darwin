#include "wrec-image-test/wrec-image-test.h"
char *tempdir = NULL;

TEST t_wrec_image_test(void){
  MEASURE(measurement_capture_and_process_screenshot_rgb_data)

  struct Vector *window_ids = get_window_ids();
  {
    ASSERT_GTEm("Failed to get window ids", vector_size(window_ids), 1);
  }
  unsigned long                       window_id;
  size_t                              windows_qty = vector_size(window_ids), captured_images_bytes = 0,
                                      captured_images = 0, captured_size, dur;
  unsigned long                       started_ms = timestamp();
  char                                *qoi_file_path, *png_file_path;
  size_t                              capture_dur, capture_started;
  bool                                capture_ok = false;
  struct qoi_encode_to_file_result_t  *qoi_encode_to_file_result;
  struct loaded_png_file_t            *loaded_png;
  struct qoi_encode_to_file_request_t *qoi_file_req;
  struct qoi_decode_result_t          *qoi_decode_file_result, *qoi_decode_memory_result;
  struct qoi_encode_result_t          *qoi_encode_file_result, *qoi_encode_memory_result;
  {
  }

  for (size_t i = 0; i < windows_qty; i++) {
    {
      window_id = (unsigned long)vector_get(window_ids, i);
      asprintf(&qoi_file_path, "%s/qoi/%lu/%lu.qoi",
               tempdir, window_id, (size_t)timestamp()
               );
      asprintf(&png_file_path, "%s/png/%lu/%lu.png",
               tempdir, window_id, (size_t)timestamp()
               );
      fsio_mkdirs_parent(png_file_path, 0700);
      fsio_mkdirs_parent(qoi_file_path, 0700);
    }
    {
      capture_started = (size_t)timestamp();
      capture_ok      = capture_to_file_image(window_id, png_file_path);
      capture_dur     = timestamp() - (size_t)capture_started;
    }
    {
      ASSERT_EQm("Failed to take screenshot", capture_ok, true);
    }
    {
      captured_size = fsio_file_size(png_file_path);
      captured_images++;
      captured_images_bytes += captured_size;
    }
    {
      log_info("Captured %s Screenshot file %s in %s",
               bytes_to_string(captured_size), png_file_path,
               milliseconds_to_string(capture_dur)
               );
    }
    {
      loaded_png = load_png_file_pixels(png_file_path);
    }
    {
      ASSERT_GTEm("Unable to load png file with stb", loaded_png->image_dimensions.width, 10);
    }
    {
      qoi_file_req                   = calloc(1, sizeof(struct qoi_encode_to_file_request_t));
      qoi_file_req->desc             = calloc(1, sizeof(struct qoi_desc));
      qoi_file_req->qoi_file_path    = qoi_file_path;
      qoi_file_req->rgb_pixels       = loaded_png->rgb_pixels;
      qoi_file_req->desc->width      = loaded_png->image_dimensions.width;
      qoi_file_req->desc->height     = loaded_png->image_dimensions.height;
      qoi_file_req->desc->channels   = RGBA_CHANNELS;
      qoi_file_req->desc->colorspace = QOI_SRGB;
      qoi_encode_to_file_result      = qoi_encode_to_file(qoi_file_req);
    }
    ASSERT_EQm("Failed to encode qoi file", qoi_encode_to_file_result, 0);
    {
      qoi_decode_file_result = qoi_decode_file(qoi_file_path);
    }
    {
      ASSERT_NEQm("Failed to decode qoi file", qoi_decode_file_result, NULL);
    }
    {
      qoi_encode_memory_result = qoi_encode_memory(loaded_png->rgb_pixels, loaded_png->image_dimensions.width, loaded_png->image_dimensions.height);
    }
    {
      ASSERT_NEQm("Failed to encode qoi to memory", qoi_encode_memory_result, NULL);
      ASSERT_GTEm("Failed to encode qoi to memory", qoi_encode_memory_result->size, captured_size / 2);
      /*
       * ASSERT_NEQm("Failed to encode qoi to memory", qoi_encode_memory_result->desc, NULL);
       * ASSERT_EQm("Failed to encode qoi to memory", (int)qoi_encode_memory_result->desc->height, loaded_png->height);
       * ASSERT_EQm("Failed to encode qoi to memory", (int)qoi_encode_memory_result->desc->width, loaded_png->width);
       * ASSERT_EQm("Failed to encode qoi to memory", qoi_encode_memory_result->desc->channels, RGBA_CHANNELS);
       * ASSERT_EQm("Failed to encode qoi to memory", qoi_encode_memory_result->desc->colorspace, QOI_SRGB);
       */
    }
    {
      qoi_decode_memory_result = qoi_decode_memory(qoi_encode_memory_result->data, qoi_encode_memory_result->size);
    }
    {
      ASSERT_NEQm("Failed to decode qoi from memory", qoi_decode_memory_result, NULL);
      /*
       * ASSERT_EQm("Failed to decode qoi from memory", (int)qoi_decode_memory_result->desc->width, loaded_png->width);
       * ASSERT_NEQm("Failed to decode qoi from memory", qoi_decode_memory_result->desc, NULL);
       * ASSERT_EQm("Failed to decode qoi from memory", (int)qoi_decode_memory_result->desc->height, loaded_png->height);
       * ASSERT_EQm("Failed to decode qoi from memory", qoi_decode_memory_result->desc->channels, RGBA_CHANNELS);
       * ASSERT_EQm("Failed to decode qoi from memory", qoi_decode_memory_result->desc->colorspace, QOI_SRGB);
       */
    }
    {
      free(qoi_file_req->desc);
      free(qoi_file_req);
      free(loaded_png);
      free(qoi_decode_memory_result);
      free(qoi_encode_memory_result);
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
  if (argc > 1 && stringfn_starts_with(argv[1], "/") == true) {
    asprintf(&tempdir, "%s", argv[1]);
  }else{
    asprintf(&tempdir, "/tmp/wrec-image-test/%d", getpid());
  }
  if (fsio_dir_exists(tempdir) == false) {
    fsio_mkdirs(tempdir, 0700);
  }
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_wrec_image_test);
  GREATEST_MAIN_END();
}
