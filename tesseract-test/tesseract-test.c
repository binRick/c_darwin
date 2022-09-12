#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
///////////////////////////////////////////////////////////
#include "allheaders.h"
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "capi.h"
#include "display-utils/display-utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "space-utils/space-utils.h"
#include "tesseract-test/tesseract-test.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"
///////////////////////////////////////////////////////////
static const char         *tess_lang = "eng";
static TessBaseAPI        *api;
static char               *tess_ver;
static char               *extracted_text = NULL;
static TessResultIterator *tess_iterator = NULL;
static bool               TESSERACT_TEST_DEBUG_MODE = false;
static struct Vector      *space_id_images, *window_id_images, *space_ids_v, *windows_v;
static const char         *TEST_IMAGES[] = {
  "tesseract-test/etc/ss1.png",
};
struct Pix                *img;
///////////////////////////////////////////////////////////

#define RUN_TESTS()    {                                       \
    RUN_TEST(t_tesseract_version);                             \
    RUN_TEST(t_tesseract_api);                                 \
    RUN_TEST(t_tesseract_api_init);                            \
    RUN_TESTp(t_tesseract_read_image, (void *)TEST_IMAGES[0]); \
    RUN_TEST(t_tesseract_set_image);                           \
    RUN_TEST(t_tesseract_api_recognize);                       \
    RUN_TESTp(t_tesseract_api_iterator, (void *)RIL_WORD);     \
    RUN_TESTp(t_tesseract_api_iterator, (void *)RIL_TEXTLINE); \
    RUN_TEST(t_tesseract_api_get_text);                        \
    RUN_TEST(t_tesseract_api_cleanup);                         \
}

TEST t_tesseract_capture_windows(){
  unsigned long captures_started = timestamp();

  windows_v = get_windows();
  char   *msg;
  char   *image_filename;
  size_t window_ids_bytes = 0;

  for (size_t i = 0; i < vector_size(windows_v); i++) {
    struct window_t *window = (struct window_t *)vector_get(windows_v, i);
    unsigned long   started = timestamp();
    log_info("capturing window %lu", window->window_id);
    CGImageRef      image_ref = window_capture(window);
    asprintf(&image_filename, "/tmp/window-%lu.png", window->window_id);
    save_window_cgref_to_png(image_ref, image_filename);
    vector_push(window_id_images, (void *)strdup(image_filename));
    window_ids_bytes += fsio_file_size(image_filename);
    asprintf(&msg, "Captured %s Window in %s",
             bytes_to_string(fsio_file_size(image_filename)),
             milliseconds_to_string(timestamp() - started)
             );
  }
  asprintf(&msg, "Captured %lu windows of %s in %s",
           vector_size(window_id_images),
           bytes_to_string(window_ids_bytes),
           milliseconds_to_string(timestamp() - captures_started)
           );
  PASSm(msg);
}

TEST t_tesseract_capture_spaces(){
  unsigned long captures_started = timestamp();

  space_ids_v = get_space_ids_v();
  char   *msg;
  char   *image_filename;
  size_t space_ids_bytes = 0;

  for (size_t i = 0; i < vector_size(space_ids_v); i++) {
    size_t        space_id = (size_t)vector_get(space_ids_v, i);
    unsigned long started  = timestamp();
    log_info("capturing space %lu", space_id);
    CGImageRef    image_ref = space_capture(space_id);
    asprintf(&image_filename, "/tmp/space-%lu.png", space_id);
    save_window_cgref_to_png(image_ref, image_filename);
    vector_push(space_id_images, (void *)strdup(image_filename));
    space_ids_bytes += fsio_file_size(image_filename);
    asprintf(&msg, "Captured %s Space in %s",
             bytes_to_string(fsio_file_size(image_filename)),
             milliseconds_to_string(timestamp() - started)
             );
  }
  asprintf(&msg, "Captured %lu spaces of %s in %s",
           vector_size(space_id_images),
           bytes_to_string(space_ids_bytes),
           milliseconds_to_string(timestamp() - captures_started)
           );
  PASSm(msg);
}

TEST t_tesseract_api_cleanup(){
  unsigned long started = timestamp();

  TessBaseAPIEnd(api);
  TessBaseAPIDelete(api);
  char *msg;

  asprintf(&msg, "Cleaned up in %s",
           milliseconds_to_string(timestamp() - started)
           );
  PASSm(msg);
}

TEST t_tesseract_api_get_text(){
  unsigned long started = timestamp();

  extracted_text = TessBaseAPIGetUTF8Text(api);
  ASSERT_NEQ(extracted_text, NULL);
  struct StringFNStrings lines = stringfn_split_lines_and_trim(extracted_text);

  TessDeleteText(extracted_text);
  char *msg;

  asprintf(&msg, "Extracted %s, %d Lines of text in %s",
           bytes_to_string(strlen(extracted_text)),
           lines.count,
           milliseconds_to_string(timestamp() - started)
           );
  if (TESSERACT_TEST_DEBUG_MODE == true) {
    log_debug("%s", extracted_text);
  }
  stringfn_release_strings_struct(lines);
  PASSm(msg);
}

TEST t_tesseract_api_iterator(void *ITERATOR_LEVEL){
  unsigned long started = timestamp();

  tess_iterator = TessBaseAPIGetIterator(api);
  TessPageIteratorLevel level = (int)(size_t)ITERATOR_LEVEL;
  size_t                i = 0;
  char                  *TS_WORD_TYPE = NULL; size_t TS_WORD_LEN = 0; int TS_VALID_WORD = -1;

  do{
    char  *ts_word = TessResultIteratorGetUTF8Text(tess_iterator, level);
    float conf     = TessResultIteratorConfidence(tess_iterator, level);
    TS_WORD_LEN   = strlen(ts_word);
    TS_VALID_WORD = TessBaseAPIIsValidWord(api, ts_word);
    if (conf < 0.1) {
      goto next_word;
    }
    if (stringfn_is_ascii(ts_word) == false) {
      if (TESSERACT_TEST_DEBUG_MODE == true) {
        log_debug("Skipping non ascii '%s'", ts_word);
      }
      goto next_word;
    }
    if (stringfn_is_digits(ts_word)) {
      TS_WORD_TYPE = AC_BLUE "digits" AC_RESETALL;
    }else{
      TS_WORD_TYPE = AC_GREEN "characters" AC_RESETALL;
    }

    if (TESSERACT_TEST_DEBUG_MODE == true) {
      log_debug("#%.4lu> [%.2f][%s][%d][" AC_CYAN "%.2lu" AC_RESETALL "] %s",
                i,
                conf, TS_WORD_TYPE, TS_VALID_WORD, TS_WORD_LEN,
                ts_word
                );
    }
next_word:
    TessDeleteText(ts_word);
    i++;
  } while (TessPageIteratorNext((TessPageIterator *)tess_iterator, level));

  char *msg;

  asprintf(&msg, "Processed %lu items in %s", i, milliseconds_to_string(timestamp() - started));
  PASSm(msg);
} /* t_tesseract_api_iterator */

TEST t_tesseract_api_recognize(){
  unsigned long started = timestamp();
  int           res     = TessBaseAPIRecognize(api, NULL);

  ASSERT_EQ(res, 0);
  char *msg;

  asprintf(&msg, "API Recognized in %s", milliseconds_to_string(timestamp() - started));
  PASSm(msg);
}

TEST t_tesseract_set_image(){
  unsigned long started = timestamp();

  TessBaseAPISetImage2(api, img);
  struct Pix *loaded_img = TessBaseAPIGetInputImage(api);

  ASSERT_NEQ(loaded_img, NULL);
  pixDestroy(&img);
  char *msg;

  asprintf(&msg, "Loaded Image in %s", milliseconds_to_string(timestamp() - started));
  PASSm(msg);
}

TEST t_tesseract_read_image(void *IMG_PATH){
  unsigned long started     = timestamp();
  char          *image_path = (char *)IMG_PATH;

  img = pixRead(image_path);
  ASSERT_NEQ(img, NULL);
  char *msg;

  asprintf(&msg, "Read %s Image %s in %s", bytes_to_string(fsio_file_size(image_path)), image_path, milliseconds_to_string(timestamp() - started));
  PASSm(msg);
}

TEST t_tesseract_api_init(){
  unsigned long started = timestamp();
  int           res     = TessBaseAPIInit3(api, NULL, tess_lang);

  ASSERT_EQ(res, 0);
  char *msg;

  asprintf(&msg, "Initialized Tesseract API v3 Language %s in %s", tess_lang, milliseconds_to_string(timestamp() - started));
  PASSm(msg);
}

TEST t_tesseract_api(){
  unsigned long started = timestamp();

  api = TessBaseAPICreate();
  ASSERT_NEQ(api, NULL);
  char *msg;

  asprintf(&msg, "Created Base Tesseract API in %s", milliseconds_to_string(timestamp() - started));
  PASSm(msg);
}

TEST t_tesseract_version(){
  tess_ver = TessVersion();
  char *msg;
  asprintf(&msg, "Initializing Tesseract Version %s", tess_ver);
  PASSm(msg);
}

SUITE(s_tesseract_windows) {
  unsigned long started = timestamp();

  window_id_images = vector_new();
  RUN_TEST(t_tesseract_capture_windows);
  RUN_TEST(t_tesseract_version);
  RUN_TEST(t_tesseract_api);
  RUN_TEST(t_tesseract_api_init);
  for (size_t i = 0; i < vector_size(window_id_images); i++) {
    char *image = (char *)vector_get(window_id_images, i);
    if (fsio_file_exists(image) == false) {
      continue;
    }
    log_info("Loading window image %s", image);
    RUN_TESTp(t_tesseract_read_image, (void *)image);
    RUN_TEST(t_tesseract_set_image);
    RUN_TEST(t_tesseract_api_recognize);
    RUN_TESTp(t_tesseract_api_iterator, (void *)RIL_TEXTLINE); \
    RUN_TEST(t_tesseract_api_get_text);
  }
  RUN_TEST(t_tesseract_api_cleanup);
  log_info("Completed Windows Tests in %s", milliseconds_to_string(timestamp() - started));
}

SUITE(s_tesseract_spaces) {
  unsigned long started = timestamp();

  space_id_images = vector_new();
  RUN_TEST(t_tesseract_capture_spaces);
  RUN_TEST(t_tesseract_version);
  RUN_TEST(t_tesseract_api);
  RUN_TEST(t_tesseract_api_init);
  for (size_t i = 0; i < vector_size(space_id_images); i++) {
    char *image = (char *)vector_get(space_id_images, i);
    if (fsio_file_exists(image) == false) {
      continue;
    }
    log_info("Loading space image %s", image);
    RUN_TESTp(t_tesseract_read_image, (void *)image);
    RUN_TEST(t_tesseract_set_image);
    RUN_TEST(t_tesseract_api_recognize);
    RUN_TESTp(t_tesseract_api_iterator, (void *)RIL_TEXTLINE); \
    RUN_TEST(t_tesseract_api_get_text);
  }
  RUN_TEST(t_tesseract_api_cleanup);
  log_info("Completed Spaces Tests in %s", milliseconds_to_string(timestamp() - started));
}

SUITE(s_tesseract_test) {
  unsigned long started = timestamp();

  RUN_TESTS()
  log_info("Completed Tesseract Tests in %s", milliseconds_to_string(timestamp() - started));
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  int verbosity = greatest_get_verbosity();
  TESSERACT_TEST_DEBUG_MODE = (getenv("DEBUG") != NULL || verbosity > 1) ? true : false;
  RUN_SUITE(s_tesseract_test);
  RUN_SUITE(s_tesseract_windows);
  RUN_SUITE(s_tesseract_spaces);
  GREATEST_MAIN_END();
}
