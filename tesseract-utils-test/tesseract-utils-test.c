#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
///////////////////////////////////////////////////////////
#include "allheaders.h"
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_img/src/img.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "capture-utils/capture-utils.h"
#include "display-utils/display-utils.h"
#include "image-utils/image-utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "space-utils/space-utils.h"
#include "tesseract-utils-test/tesseract-utils-test.h"
#include "tesseract-utils/tesseract-utils.h"
#include "tesseract/capi.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"
///////////////////////////////////////////////////////////
#define MIN_PNG_SIZE      1024 * 32
#define MIN_PNG_WIDTH     100
#define MIN_PNG_HEIGHT    50
static size_t MAX_WINDOWS = 100;
static char *image_grayscale_name(char *image_path);
static char *image_results_file_name(char *image_path);
static const char         *tess_lang = "eng";
static TessBaseAPI        *api;
static char               *tess_ver;
static char               *extracted_text = NULL;
static TessResultIterator *tess_iterator = NULL;
static bool               TESSERACT_TEST_DEBUG_MODE = false;
static struct Vector      *space_id_images, *window_id_images, *space_ids_v, *windows_v;
static const char         *TEST_IMAGES[] = {
  "tesseract-utils-test/etc/ss1.png",
};
struct Pix                *img;
///////////////////////////////////////////////////////////

#define RUN_TESTS()    {                                                                                         \
    RUN_TEST(t_tesseract_version);                                                                               \
    RUN_TEST(t_tesseract_api);                                                                                   \
    RUN_TEST(t_tesseract_api_init);                                                                              \
    RUN_TESTp(t_tesseract_read_image, (void *)TEST_IMAGES[0]);                                                   \
    RUN_TEST(t_tesseract_api_recognize);                                                                         \
    RUN_TESTp(t_tesseract_api_iterator, (void *)RIL_TEXTLINE, (void *)image_iterator_file_name(TEST_IMAGES[0])); \
    RUN_TESTp(t_tesseract_api_get_text, (void *)image_results_file_name(TEST_IMAGES[0]));                        \
    RUN_TEST(t_tesseract_api_cleanup);                                                                           \
}

static char *image_stats_file_name(char *image_path){
  char *image_ext       = fsio_file_extension(image_path);
  char *_image_filename = stringfn_substring(image_path, 0, strlen(image_path) - strlen(image_ext));
  char *image_filename;

  asprintf(&image_filename, "%s-stats.txt", _image_filename);
  return(image_filename);
}

static char *image_iterator_file_name(char *image_path){
  char *image_ext       = fsio_file_extension(image_path);
  char *_image_filename = stringfn_substring(image_path, 0, strlen(image_path) - strlen(image_ext));
  char *image_filename;

  asprintf(&image_filename, "%s-iterator.txt", _image_filename);
  return(image_filename);
}

static char *image_results_file_name(char *image_path){
  char *image_ext       = fsio_file_extension(image_path);
  char *_image_filename = stringfn_substring(image_path, 0, strlen(image_path) - strlen(image_ext));
  char *image_filename;

  asprintf(&image_filename, "%s-results.txt", _image_filename);
  return(image_filename);
}

static char *image_grayscale_name(char *image_path){
  char *image_ext           = fsio_file_extension(image_path);
  char *_tif_image_filename = stringfn_substring(image_path, 0, strlen(image_path) - strlen(image_ext));
  char *tif_image_filename;

  asprintf(&tif_image_filename, "%s-grayscale.tif", _tif_image_filename);
  return(tif_image_filename);
}

TEST t_tesseract_capture_windows(){
  unsigned long captures_started = timestamp();

  windows_v = get_windows();
  char   *msg;
  char   *image_filename;
  size_t window_ids_bytes = 0;

  for (size_t i = 0; i < vector_size(windows_v) && vector_size(window_id_images) < MAX_WINDOWS; i++) {
    struct window_t *window = (struct window_t *)vector_get(windows_v, i);
    unsigned long   started = timestamp();
    log_info("capturing window %lu", window->window_id);
    asprintf(&image_filename, "/tmp/window-%lu.png", window->window_id);
    bool ok = capture_type_capture_png_file(CAPTURE_TYPE_WINDOW, window->window_id, image_filename);
    if (ok == false || fsio_file_size(image_filename) < MIN_PNG_SIZE) {
      if (TESSERACT_TEST_DEBUG_MODE) {
        log_info("Skipping %s png file %s (size too small)",
                 bytes_to_string(fsio_file_size(image_filename)),
                 image_filename
                 );
      }
      continue;
    }
    struct image_dimensions_t *png_size = get_png_dimensions(image_filename);
    if (TESSERACT_TEST_DEBUG_MODE) {
      log_debug("PNG Size: %dx%d", png_size->width, png_size->height);
    }
    if (png_size->width < MIN_PNG_WIDTH || png_size->height < MIN_PNG_HEIGHT) {
      if (TESSERACT_TEST_DEBUG_MODE) {
        log_info("Skipping %dx%d png file %s (dimensions too small)", png_size->width, png_size->height, image_filename);
      }
      continue;
    }

    vector_push(window_id_images, (void *)strdup(image_filename));
    window_ids_bytes += fsio_file_size(image_filename);
    asprintf(&msg, "Captured %s %dx%d Window in %s",
             bytes_to_string(fsio_file_size(image_filename)),
             png_size->width, png_size->height,
             milliseconds_to_string(timestamp() - started)
             );
  }
  asprintf(&msg, "Captured %lu windows of %s in %s",
           vector_size(window_id_images),
           bytes_to_string(window_ids_bytes),
           milliseconds_to_string(timestamp() - captures_started)
           );
  PASSm(msg);
} /* t_tesseract_capture_windows */

TEST t_tesseract_capture_spaces(){
  unsigned long captures_started = timestamp();

  space_ids_v = get_space_ids_v();
  char   *msg;
  size_t space_ids_bytes = 0;

  for (size_t i = 0; i < vector_size(space_ids_v); i++) {
    unsigned long started  = timestamp();
    size_t        space_id = (size_t)vector_get(space_ids_v, i);
    char          *image_filename;
    asprintf(&image_filename, "/tmp/space-%lu.png", space_id);
    bool          ok = capture_type_capture_png_file(CAPTURE_TYPE_SPACE, space_id, image_filename);
    log_info("capturing space %lu to %s", space_id, image_filename);
    if (ok == false || fsio_file_size(image_filename) < MIN_PNG_SIZE) {
      if (TESSERACT_TEST_DEBUG_MODE) {
        log_info("Skipping %s png file %s (size too small)",
                 bytes_to_string(fsio_file_size(image_filename)),
                 image_filename
                 );
      }
      continue;
    }
    struct image_dimensions_t *png_size = get_png_dimensions(image_filename);
    if (TESSERACT_TEST_DEBUG_MODE) {
      log_debug("PNG Size: %dx%d", png_size->width, png_size->height);
    }
    if (png_size->width < MIN_PNG_WIDTH || png_size->height < MIN_PNG_HEIGHT) {
      if (TESSERACT_TEST_DEBUG_MODE) {
        log_info("Skipping %dx%d png file %s (dimensions too small)", png_size->width, png_size->height, image_filename);
      }
      continue;
    }
    vector_push(space_id_images, (void *)strdup(image_filename));
    space_ids_bytes += fsio_file_size(image_filename);
    asprintf(&msg, "Captured %s %dx%d Space in %s",
             bytes_to_string(fsio_file_size(image_filename)),
             png_size->width, png_size->height,
             milliseconds_to_string(timestamp() - started)
             );
  }
  asprintf(&msg, "Captured %lu spaces of %s in %s",
           vector_size(space_id_images),
           bytes_to_string(space_ids_bytes),
           milliseconds_to_string(timestamp() - captures_started)
           );
  PASSm(msg);
} /* t_tesseract_capture_spaces */

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

TEST t_tesseract_api_get_text(void *RESULTS_FILE){
  unsigned long started = timestamp();

  extracted_text = TessBaseAPIGetUTF8Text(api);
  ASSERT_NEQ(extracted_text, NULL);
  fsio_write_text_file((char *)RESULTS_FILE, extracted_text);
  struct StringFNStrings lines = stringfn_split_lines_and_trim(extracted_text);

  TessDeleteText(extracted_text);
  char *msg;

  asprintf(&msg, "Extracted %s, %d Lines of text to %s in %s",
           bytes_to_string(strlen(extracted_text)),
           lines.count,
           (char *)RESULTS_FILE,
           milliseconds_to_string(timestamp() - started)
           );
  if (TESSERACT_TEST_DEBUG_MODE == true) {
    log_debug("%s", extracted_text);
  }
  stringfn_release_strings_struct(lines);
  PASSm(msg);
}

TEST t_tesseract_api_iterator(void *ITERATOR_LEVEL, void *ITERATOR_RESULTS_FILE){
  unsigned long started = timestamp();

  char          *results_file = (char *)ITERATOR_RESULTS_FILE;

  if (fsio_file_exists(results_file)) {
    fsio_remove(results_file);
  }
  char        *m;
  size_t      i      = 0;
  struct Boxa *boxes = TessBaseAPIGetComponentImages(api, RIL_TEXTLINE, true, NULL, NULL);

  if (boxes != NULL) {
    log_info("%d", boxes->n);
    for (int i = 0; i < boxes->n; i++) {
      BOX *box = boxaGetBox(boxes, i, L_CLONE);
      TessBaseAPISetRectangle(api, box->x, box->y, box->w, box->h);

      const char *c   = TessBaseAPIGetUTF8Text(api);
      int        conf = TessBaseAPIMeanTextConf(api);
      if ((c != NULL) && (c[0] != '\0')) {
        asprintf(&m, "Box[%d]: x=%d, y=%d, w=%d, h=%d, confidence: %d, text: %s",
                 i, box->x, box->y, box->w, box->h, conf, c);
        log_info("%s", m);
        fsio_append_text_file(results_file, m);
        i++;
      }
      boxDestroy(&box);
    }
  }

/*
 * tess_iterator = TessBaseAPIGetIterator(api);
 * TessPageIteratorLevel level = (int)(size_t)ITERATOR_LEVEL;
 * char                  *TS_WORD_TYPE = NULL; size_t TS_WORD_LEN = 0; int TS_VALID_WORD = -1;
 * do{
 *  char  *ts_word = TessResultIteratorGetUTF8Text(tess_iterator, level);
 *  float conf     = TessResultIteratorConfidence(tess_iterator, level);
 *
 *
 *
 *
 *  TS_WORD_LEN   = strlen(ts_word);
 *  TS_VALID_WORD = TessBaseAPIIsValidWord(api, ts_word);
 *  if (conf < 0.1) {
 *    goto next_word;
 *  }
 *  if (stringfn_is_ascii(ts_word) == false) {
 *    if (TESSERACT_TEST_DEBUG_MODE == true) {
 *      log_debug("Skipping non ascii '%s'", ts_word);
 *    }
 *    goto next_word;
 *  }
 *  if (stringfn_is_digits(ts_word)) {
 *    TS_WORD_TYPE = AC_BLUE "digits" AC_RESETALL;
 *  }else{
 *    TS_WORD_TYPE = AC_GREEN "characters" AC_RESETALL;
 *  }
 *  fsio_append_text_file(results_file, ts_word);
 * //    fsio_append_text_file(results_file,"\n");
 *
 *  if (TESSERACT_TEST_DEBUG_MODE == true) {
 *    log_debug("#%.4lu> [%.2f][%s][%d][" AC_CYAN "%.2lu" AC_RESETALL "] %s",
 *              i,
 *              conf, TS_WORD_TYPE, TS_VALID_WORD, TS_WORD_LEN,
 *              ts_word
 *              );
 *  }
 * next_word:
 *  TessDeleteText(ts_word);
 *  i++;
 * } while (TessPageIteratorNext((TessPageIterator *)tess_iterator, level));
 *
 *
 */

  char *msg;

  asprintf(&msg, "Processed %lu items and wrote %s to %s in %s",
           i,
           bytes_to_string(fsio_file_size(results_file)),
           results_file,
           milliseconds_to_string(timestamp() - started)
           );
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

TEST t_tesseract_convert_image(void *IMG_PATH){
  unsigned long started             = timestamp();
  char          *image_path         = (char *)IMG_PATH;
  char          *tif_image_filename = image_grayscale_name(image_path);

  log_info("Converting %s to %s", image_path, tif_image_filename);

  FILE       *input_png_file = fopen(image_path, "rb");
  CGImageRef png_gs          = png_file_to_grayscale_cgimage_ref_resized(input_png_file, 400);
  bool       ok              = write_cgimage_ref_to_tif_file_path(png_gs, tif_image_filename);

  ASSERT_EQ(ok, true);

  char *msg;

  asprintf(&msg, "Converted %s %s to %s %s in %s",
           bytes_to_string(fsio_file_size(image_path)), image_path,
           bytes_to_string(fsio_file_size(tif_image_filename)), tif_image_filename,
           milliseconds_to_string(timestamp() - started)
           );
  PASSm(msg);
}

TEST t_tesseract_read_image(void *IMG_PATH){
  unsigned long started     = timestamp();
  char          *image_path = (char *)IMG_PATH;

  log_info("Reading %s", image_path);
  img = pixRead(image_path);
  ASSERT_NEQ(img, NULL);
  char *msg;

  log_info("Read %s Image %s in %s",
           bytes_to_string(fsio_file_size(image_path)), image_path, milliseconds_to_string(timestamp() - started)
           );
  started = timestamp();
  TessBaseAPISetImage2(api, img);
  struct Pix *loaded_img = TessBaseAPIGetInputImage(api);

  ASSERT_NEQ(loaded_img, NULL);
  pixDestroy(&img);

  log_debug("Loaded Image in %s",
            milliseconds_to_string(timestamp() - started)
            );
  PASS();
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
  unsigned long pngs_started = timestamp();

  for (size_t i = 0; i < vector_size(window_id_images); i++) {
    char *image = (char *)vector_get(window_id_images, i);
    if (fsio_file_exists(image) == false) {
      continue;
    }
    log_info("Loading window png image %s", image);
    RUN_TESTp(t_tesseract_read_image, (void *)image);
    RUN_TEST(t_tesseract_api_recognize);
    RUN_TESTp(t_tesseract_api_iterator, (void *)RIL_TEXTLINE, (void *)image_iterator_file_name(image));
    RUN_TESTp(t_tesseract_api_get_text, (void *)image_results_file_name(image));
  }
  log_info("Completed PNGs in %s", milliseconds_to_string(timestamp() - pngs_started));
  unsigned long tifs_started = timestamp();

  for (size_t i = 0; i < vector_size(window_id_images); i++) {
    char *image = (char *)vector_get(window_id_images, i);
    if (fsio_file_exists(image) == false) {
      continue;
    }
    char *tif = image_grayscale_name(image);
    RUN_TESTp(t_tesseract_convert_image, (void *)image);
    log_info("Loading window tif image %s", tif);
    RUN_TESTp(t_tesseract_read_image, (void *)tif);
    RUN_TEST(t_tesseract_api_recognize);
    RUN_TESTp(t_tesseract_api_iterator, (void *)RIL_TEXTLINE, (void *)image_iterator_file_name(image));
    RUN_TESTp(t_tesseract_api_get_text, (void *)image_results_file_name(tif));
  }
  log_info("Completed TIFs in %s", milliseconds_to_string(timestamp() - tifs_started));
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
  unsigned long pngs_started = timestamp();

  for (size_t i = 0; i < vector_size(space_id_images); i++) {
    char *image = (char *)vector_get(space_id_images, i);
    if (fsio_file_exists(image) == false) {
      continue;
    }
    log_info("Loading space png image %s", image);
    RUN_TESTp(t_tesseract_read_image, (void *)image);
    RUN_TEST(t_tesseract_api_recognize);
    RUN_TESTp(t_tesseract_api_iterator, (void *)RIL_TEXTLINE, (void *)image_iterator_file_name(image));
    RUN_TESTp(t_tesseract_api_get_text, (void *)image_results_file_name(image));
  }
  log_info("Completed PNGs in %s", milliseconds_to_string(timestamp() - pngs_started));
  unsigned long tifs_started = timestamp();

  for (size_t i = 0; i < vector_size(space_id_images); i++) {
    char *image = (char *)vector_get(space_id_images, i);
    if (fsio_file_exists(image) == false) {
      continue;
    }
    char *tif = image_grayscale_name(image);
    RUN_TESTp(t_tesseract_convert_image, (void *)image);
    log_info("Loading space tif image %s", tif);
    RUN_TESTp(t_tesseract_read_image, (void *)tif);
    RUN_TEST(t_tesseract_api_recognize);
    RUN_TESTp(t_tesseract_api_iterator, (void *)RIL_TEXTLINE, (void *)image_iterator_file_name(tif));
    RUN_TESTp(t_tesseract_api_get_text, (void *)image_results_file_name(tif));
  }
  log_info("Completed TIFs in %s", milliseconds_to_string(timestamp() - tifs_started));
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
  RUN_SUITE(s_tesseract_spaces);
  RUN_SUITE(s_tesseract_windows);
  GREATEST_MAIN_END();
}
