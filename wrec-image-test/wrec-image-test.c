////////////////////////////////////////////
#include "wrec-capture/wrec-capture.h"
#include "wrec-common/wrec-common.h"
#include "wrec-image-test/wrec-image-test.h"
#include "wrec-image/wrec-image.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log.h/log.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"

////////////////////////////////////////////
TEST t_wrec_image_test(void){
  struct Vector *window_ids = get_windows_ids();

  ASSERT_GTEm("Failed to get window ids", vector_size(window_ids), 1);
  unsigned long window_id;
  size_t        qty                   = vector_size(window_ids);
  size_t        captured_images_bytes = 0;
  unsigned long started_ms            = timestamp();

  for (size_t i = 0; i < qty; i++) {
    window_id = (unsigned long)vector_get(window_ids, i);
    char *SCREENSHOT_FILE;
    asprintf(&SCREENSHOT_FILE, "%swindow-%lu-%lu-wrec-image-test.png", gettempdir(), window_id, (size_t)timestamp());
    printf("%s\n", SCREENSHOT_FILE);
    bool ok = capture_to_file_image(window_id, SCREENSHOT_FILE);
    captured_images_bytes += fsio_file_size(SCREENSHOT_FILE);
    ASSERT_EQm("Failed to take screenshot", ok, true);
    struct loaded_png_file_t *png = load_png_file_pixels(SCREENSHOT_FILE);

    ASSERT_GTEm("unable to load png file with stb", png->image_dimensions.width, 10);
    char *qoi_file;
    asprintf(&qoi_file, "%swindow-%lu-%lu-wrec-image-test-qoi-encode.qoi", gettempdir(), window_id, (size_t)timestamp());

    int  qoi_encode_file_result = qoi_encode_file(qoi_file, png->pixels, png->image_dimensions.width, png->image_dimensions.height);
    ASSERT_EQm("Failed to encode qoi file", qoi_encode_file_result, 0);
    int  qoi_decode_file_result = qoi_decode_file(qoi_file);
    ASSERT_EQm("Failed to decode qoi file", qoi_decode_file_result, 0);
    void *qoi_data = qoi_encode_to_memory(png->pixels, png->image_dimensions.width, png->image_dimensions.height);
    ASSERT_NEQm("failed to encode qoi to memory", qoi_data, NULL);
//void *decoded_data = qoi_decode_from_memory(qoi_data,

    free(SCREENSHOT_FILE);
  }
  PASS();
}

SUITE(s_wrec_image_test) {
  RUN_TEST(t_wrec_image_test);
}

GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_wrec_image_test);
  GREATEST_MAIN_END();
}
