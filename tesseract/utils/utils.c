#pragma once
#ifndef TESSERACT_UTILS_C
#define TESSERACT_UTILS_C
#include "allheaders.h"
#include "tesseract/capi.h"
////////////////////////////////////////////
#include "tesseract/utils/utils.h"
#include "timestamp/timestamp.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "c_workqueue/include/workqueue.h"
#include "capture/utils/utils.h"
#include "capture/window/window.h"
#include "chan/src/chan.h"
#include "core/image/image.h"
#include "core/utils/utils.h"
#include "frameworks/frameworks.h"
#include "image/utils/utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
#include "tempdir.c/tempdir.h"
#include "timg/utils/utils.h"
#include "vips/vips.h"
#include "wildcardcmp/wildcardcmp.h"
////////////////////////////////////////////
static bool        TESSERACT_UTILS_DEBUG_MODE = false;
static const char  *tess_lang                 = "eng";
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__tesseract_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_tesseract_utils") != NULL) {
    log_debug("Enabling tesseract-utils Debug Mode");
    TESSERACT_UTILS_DEBUG_MODE = true;
  }
}
struct Vector *get_security_words_v(){
  struct Vector *words = vector_new();

  vector_push(words, (void *)"click");
  vector_push(words, (void *)"lock");
  vector_push(words, (void *)"make");
  vector_push(words, (void *)"changes");
  return(words);
}

////////////////////////////////////////////

bool tesseract_security_preferences_logic(){
  struct tesseract_extract_result_t *r, *words;
  size_t                            focused_window_id, window_id;
  char                              *error = NULL;

  {
    focused_window_id = get_focused_window_id();
    assert(focused_window_id > 0);
    window_id = run_osascript_system_prefs();
    if (window_id <= 0) {
      error = "failed to run osascript";
      goto log_error;
    }
    focus_window_id(focused_window_id);
    if ((size_t)get_focused_window_id() != focused_window_id) {
      error = "failed to focus window";
      goto log_error;
    }
  }
  {
    words = get_security_words_v();
    if (vector_size(words) <= 0) {
      return(false);
    }
    r = tesseract_find_window_matching_word_locations(window_id, words);
  }
  {
    if (minimize_window_id(window_id) != true) {
      return(false);
    }
  }
  {
    if (parse_tesseract_extraction_results(r) != true) {
      return(false);
    }
    report_tesseract_extraction_results(r);
  }

  assert(run_osascript(CLOSE_SYSTEM_PREFERENCES) == true);

  return(true);

log_error:
  if (error) {
    log_error("%s", error);
  }
  return(false);
} /* tesseract_security_preferences_logic */

void report_tesseract_extraction_results(struct tesseract_extract_result_t *r){
  struct  StringFNStrings lines = stringfn_split_lines_and_trim(r->text);
  asprintf(&r->file, "%stesseract-extract-results-%d-%lld.%s", gettempdir(), getpid(), timestamp(), stringfn_to_lowercase(image_type_name(r->image_type)));
  fsio_write_binary_file(r->file, r->img, r->img_len);
  char *msg;
  asprintf(&msg,
          "\n\t|  %sDuration         :      %s" AC_RESETALL
          "\n\t|  %sWindow%s           :      PID:%d|%dx%d|%s"
          "\n\t|  %sImage%s            :      |Size:%s|Type:%s|File:%s|"
          "\n\t|  %sTesseract%s        :      |%s%s%s|"
          "\n\t|                          Confidence:%d|Box:%d"
          "\n\t|                          %dx%x|Width:%d|Height:%d|Mode:%ld"
          "\n\t|  %sSpace%s            :      "AC_CYAN "%lu"AC_RESETALL
          "\n\t|  %sDisplay%s          :      %lu"
          "\n\t|  %sScreenshot%s       :      %dx%d"
          "\n\t|  %sDetermined Area%s  :  "
          "\n\t|             Max x             %d  |"
          "\n\t|             Min x offset      %f  |"
          "\n\t|             Max x offset      %f  |"
          "\n\t|             Min y offset      %f  |"
          "\n\t|             Max y offset      %f  |"
          "\n\t|             Window min x offset pixels: %d"
          "\n\t|             Window max x offset pixels: %d"
          "\n\t|             Window min y offset pixels: %d"
          "\n\t|             Window max y offset pixels: %d"
          "\n%s",
          AC_GREEN, milliseconds_to_string(timestamp() - r->started),
          AC_RED, AC_RESETALL, r->window.pid, (int)(r->window.rect.size.width), (int)(r->window.rect.size.height), r->window.name,
          AC_GREEN, AC_RESETALL, bytes_to_string(r->img_len), image_type_name(r->image_type), r->file,
          AC_MAGENTA, AC_RESETALL, AC_BRIGHT_YELLOW_BLACK AC_ITALIC, lines.strings[0], AC_RESETALL,
          r->confidence, r->box, r->x, r->y, r->width, r->height, r->mode,
          AC_BLUE, AC_RESETALL, r->window.space_id,
          AC_YELLOW, AC_RESETALL, r->window.display_id,
          AC_CYAN, AC_RESETALL, r->source_file.width, r->source_file.height,
          AC_GREEN, AC_RESETALL, r->determined_area.max_x,
          r->determined_area.x_min_offset_perc,
          r->determined_area.x_max_offset_perc,
          r->determined_area.y_min_offset_perc,
          r->determined_area.y_max_offset_perc,
          r->determined_area.x_min_offset_window_pixels,
          r->determined_area.x_max_offset_window_pixels,
          r->determined_area.y_min_offset_window_pixels,
          r->determined_area.y_max_offset_window_pixels,
          ""
          );
  timg_utils_image(r->file);
  fprintf(stdout,"%s",msg);
  free(msg);
  stringfn_release_strings_struct(lines);
  return;
} /* report_tesseract_extraction_results */

bool parse_tesseract_extraction_results(struct tesseract_extract_result_t *r){
  r->determined_area.x_max_offset_perc          = (float)((float)((float)(r->x) / ((float)(r->source_file.width))));
  r->determined_area.x_min_offset_perc          = (r->determined_area.x_max_offset_perc) / 2;
  r->determined_area.y_min_offset_perc          = (float)((float)((float)(r->y) / ((float)(r->source_file.height))));
  r->determined_area.y_max_offset_perc          = 1;
  r->determined_area.x_max_offset_window_pixels = (int)((r->determined_area.x_max_offset_perc) * (float)(r->window.rect.size.width));
  r->determined_area.x_min_offset_window_pixels = (r->determined_area.x_max_offset_window_pixels) / 2;
  r->determined_area.y_min_offset_window_pixels = (int)((r->determined_area.y_min_offset_perc) * (float)(r->window.rect.size.height));
  r->determined_area.y_max_offset_window_pixels = (int)((r->determined_area.y_min_offset_perc) * (float)(r->window.rect.size.height));
  return(true);
}
#define CAPTURE_SCREEN_RECT             true
#define TESSERACT_EXTRACT_IMAGE_TYPE    IMAGE_TYPE_GIF
struct tesseract_extract_result_t *tesseract_find_window_matching_word_locations(size_t window_id, struct Vector *words){
  struct Vector                     *extractions;
  struct tesseract_extract_result_t *r;
  CGRect                            rect, orig_rect;
  char                              *s;
  int                               w = 0, h = 0;
  CGImageRef                        image_refs[1];
  size_t                            img_len    = 0;
  unsigned char                     *img       = NULL;
  enum image_type_id_t              image_type = TESSERACT_EXTRACT_IMAGE_TYPE;
  {
    assert(get_window_id_display_id(window_id) > 0);
  }
  {
    if (CAPTURE_SCREEN_RECT == true) {
      SLSGetScreenRectForWindow(CGSMainConnectionID(), (uint32_t)window_id, &orig_rect);
      rect.size   = orig_rect.size;
      rect.origin = orig_rect.origin;
      float offsets[2];
      offsets[0] = 0;
      offsets[1] = (rect.size.height * .70);
      {
        rect.origin.x    += offsets[0];
        rect.size.width  -= (offsets[0]);
        rect.origin.y    += offsets[1];
        rect.size.height -= offsets[1];

        rect.origin.x    = (int)rect.origin.x;
        rect.origin.y    = (int)rect.origin.y;
        rect.size.width  = (int)rect.size.width;
        rect.size.height = (int)rect.size.height;
      }
      if (TESSERACT_UTILS_DEBUG_MODE) {
        log_info("X:       %d => %d", (int)orig_rect.origin.x, (int)rect.origin.x);
        log_info("Y:       %d => %d", (int)orig_rect.origin.y, (int)rect.origin.y);
        log_info("Width:   %d => %d", (int)orig_rect.size.width, (int)rect.size.width);
        log_info("Height:  %d => %d", (int)orig_rect.size.height, (int)rect.size.height);
      }
      image_refs[0] = capture_type_capture_rect(CAPTURE_TYPE_WINDOW, window_id, rect);
    }else{
      image_refs[0] = capture_type_capture(CAPTURE_TYPE_WINDOW, window_id);
    }
    w = CGImageGetWidth(image_refs[0]);
    h = CGImageGetHeight(image_refs[0]);
    assert(w > 10 && h > 10);
    img = save_cgref_to_image_type_memory(image_type, image_refs[0], &(img_len));
    assert(img_len > 0);
    assert(img);
  }
  {
    unsigned long ts = timestamp();
    extractions = tesseract_extract_memory(img, img_len, RIL_TEXTLINE);
    if (TESSERACT_UTILS_DEBUG_MODE) {
      log_info("ran extract tess in %s", milliseconds_to_string(timestamp() - ts));
    }
    for (size_t i = 0; i < vector_size(extractions); i++) {
      r = (struct tesseract_extract_result_t *)vector_get(extractions, i);
      for (size_t I = 0; I < vector_size(words); I++) {
        asprintf(&s, "*%s*", stringfn_to_lowercase((char *)vector_get(words, I)));
        if (wildcardcmp(s, stringfn_to_lowercase(r->text)) == 1) {
          r->source_file.width  = w;
          r->source_file.height = h;
          r->window             = *get_window_id_info(window_id);
          r->img                = img;
          r->img_len            = img_len;
          r->image_type         = image_type;
          assert(r->window.window_id == window_id);
          return(r);
        }
      }
    }
  }
} /* tesseract_find_window_matching_word_locations */
struct TesseractArgs {
  size_t        window_id;
  char          *file;
  size_t        index;
  struct Vector *results_v;
};

static void __tesseract_capture_window(void *ARGS){
  unsigned long starteds[3];

  starteds[0] = timestamp();
  struct TesseractArgs *args = (struct TesseractArgs *)ARGS;

  args->file = capture_type_capture_png_random_file(CAPTURE_TYPE_WINDOW, args->window_id);
  log_info("\tCaptured Window in %s", milliseconds_to_string(timestamp() - starteds[0]));
}

static void __tesseract_extract_window(void *ARGS){
  struct TesseractArgs *args = (struct TesseractArgs *)ARGS;
  unsigned long        starteds[3];

  starteds[0] = timestamp();
  starteds[1] = timestamp();
  starteds[2] = timestamp();
  log_info(AC_YELLOW "\tExtracting window #%lu"AC_RESETALL, args->window_id);
  log_info("%s", args->file);
  unsigned char *png    = fsio_read_binary_file(args->file);
  size_t        png_len = fsio_file_size(args->file);

  args->results_v = tesseract_extract_memory(png, png_len, RIL_TEXTLINE);
  log_info(AC_GREEN "\tExtracted %lu items from %s in %s"AC_RESETALL,
           vector_size(args->results_v),
           args->file,
           milliseconds_to_string(timestamp() - starteds[1])
           );
}
struct Vector *tesseract_extract_windows(struct Vector *v, size_t concurrency){
  struct Vector *Args   = vector_new();
  unsigned long started = timestamp();

  if (TESSERACT_UTILS_DEBUG_MODE) {
    log_info("%lu Windows", vector_size(v));
  }
  struct WorkQueue *extract_queues[concurrency], *capture_queues[concurrency];

  for (size_t i = 0; i < concurrency; i++) {
    extract_queues[i] = workqueue_new_with_options(vector_size(v), NULL);
    capture_queues[i] = workqueue_new_with_options(vector_size(v), NULL);
  }
  unsigned long cap_started = timestamp();

  for (size_t i = 0; i < vector_size(v); i++) {
    struct TesseractArgs *a = calloc(1, sizeof(struct TesseractArgs));
    a->window_id = (size_t)vector_get(v, i);
    a->results_v = vector_new();
    vector_push(Args, (void *)a);
    if (!workqueue_push(capture_queues[i % concurrency], __tesseract_capture_window, a)) {
      log_error("Failed to push work function to queue\n");
    }
  }
  for (size_t c = 0; c < concurrency; c++) {
    if (TESSERACT_UTILS_DEBUG_MODE) {
      printf("Queue #%lu Backlog Size: %zu\n", c, workqueue_get_backlog_size(capture_queues[c]));
    }
  }
  for (size_t c = 0; c < concurrency; c++) {
    workqueue_drain(capture_queues[c]);
  }
  log_info("Captured in %s", milliseconds_to_string(timestamp() - cap_started));
  unsigned long extract_started = timestamp();

  for (size_t i = 0; i < vector_size(Args); i++) {
    size_t Q = i % concurrency;
    if (TESSERACT_UTILS_DEBUG_MODE) {
      log_info("pushing %lu to queue %lu", i, Q);
    }
    if (!workqueue_push(extract_queues[Q], __tesseract_extract_window, (struct TesseractArgs *)vector_get(Args, i))) {
      log_error("Failed to push work function to queue\n");
    }
  }
  for (size_t c = 0; c < concurrency; c++) {
    if (TESSERACT_UTILS_DEBUG_MODE) {
      printf("Queue #%lu Backlog Size: %zu\n", c, workqueue_get_backlog_size(extract_queues[c]));
    }
  }
  for (size_t c = 0; c < concurrency; c++) {
    workqueue_drain(extract_queues[c]);
  }
  log_info("Extract finished in %s", milliseconds_to_string(timestamp() - extract_started));
  for (size_t c = 0; c < concurrency; c++) {
    if (TESSERACT_UTILS_DEBUG_MODE) {
      printf("Queue #%lu Backlog Size: %zu\n", c, workqueue_get_backlog_size(extract_queues[c]));
    }
  }
  for (size_t c = 0; c < concurrency; c++) {
    workqueue_release(extract_queues[c]);
  }
  log_info("Finished in %s", milliseconds_to_string(timestamp() - started));
  for (size_t i = 0; i < vector_size(Args); i++) {
    log_info("Window ID %lu has %lu results",
             ((struct TesseractArgs *)vector_get(Args, i))->window_id,
             vector_size(((struct TesseractArgs *)vector_get(Args, i))->results_v)
             );
  }
  return(Args);
} /* tesseract_extract_windows */
struct Vector *tesseract_extract_memory(unsigned char *img_data, size_t img_data_len, unsigned long MODE){
  unsigned long ts     = timestamp();
  struct Vector *words = vector_new();
  struct Boxa   *boxes;
  struct Pix    *img;
  BOX           *box;
  char          *c, *m;
  int           conf;
  TessBaseAPI *api;
  {
  api = TessBaseAPICreate();
  assert(api != NULL);
  assert(TessBaseAPIInit3(api, NULL, tess_lang) == EXIT_SUCCESS);
    img = pixReadMem(img_data, img_data_len);
    assert(img != NULL);
    TessBaseAPISetImage2(api, img);
    assert(TessBaseAPIGetInputImage(api) != NULL);
    assert(TessBaseAPIRecognize(api, NULL) == EXIT_SUCCESS);
    boxes = TessBaseAPIGetComponentImages(api, MODE, true, NULL, NULL);
  }

  if (boxes != NULL) {
    for (int i = 0; i < boxes->n; i++) {
      box = boxaGetBox(boxes, i, L_CLONE);
      TessBaseAPISetRectangle(api, box->x, box->y, box->w, box->h);
      c    = TessBaseAPIGetUTF8Text(api);
      conf = TessBaseAPIMeanTextConf(api);
      if ((c != NULL) && (c[0] != '\0')) {
        asprintf(&m,
                 "|size:%s|mode:%lu|box#:%d/%d"
                 "|x:%d|y:%d|w:%d|h:%d|confidence:%d"
                 "|text:" AC_GREEN "%s" AC_RESETALL "|",
                 bytes_to_string(img_data_len), MODE, box->refcount, boxes->n,
                 box->x, box->y, box->w, box->h, conf, c
                 );
        if (TESSERACT_UTILS_DEBUG_MODE) {
          log_info("%s", m);
        }
        struct tesseract_extract_result_t *r = calloc(1, sizeof(struct tesseract_extract_result_t));
        r->started = timestamp();
        r->x       = box->x;
        r->y       = box->y;
        r->width   = box->w;
        r->height  = box->h;
        asprintf(&(r->text), "%s", c);
        r->mode       = MODE;
        r->box        = box->refcount;
        r->confidence = conf;
        vector_push(words, (void *)r);
      }
    }
  }
  pixDestroy(&img);
  TessBaseAPIEnd(api);
  TessBaseAPIDelete(api);
  if (TESSERACT_UTILS_DEBUG_MODE) {
    log_info("Returning %lu words in %s", vector_size(words), milliseconds_to_string(timestamp() - ts));
  }
  return(words);
} /* tesseract_extract_file_mode */

#endif
