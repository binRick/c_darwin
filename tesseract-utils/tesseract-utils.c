#pragma once
#ifndef TESSERACT_UTILS_C
#define TESSERACT_UTILS_C
#include "allheaders.h"
#include "tesseract/capi.h"
////////////////////////////////////////////
#include "tesseract-utils/tesseract-utils.h"
#include "timestamp/timestamp.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "capture-utils/capture-utils.h"
#include "image-utils/image-utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
#include "wildcardcmp/wildcardcmp.h"
////////////////////////////////////////////
static bool       TESSERACT_UTILS_DEBUG_MODE = false;
static const char *tess_lang                 = "eng";
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__tesseract_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_tesseract_utils") != NULL) {
    log_debug("Enabling tesseract-utils Debug Mode");
    TESSERACT_UTILS_DEBUG_MODE = true;
  }
}
static const long symbol_modes[] = {
  RIL_SYMBOL,
};

static const long text_modes[] = {
  RIL_WORD,
  RIL_TEXTLINE,
};
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

  {
    focused_window_id = get_focused_window_id();
    assert(focused_window_id > 0);
    window_id = run_osascript_system_prefs();
    assert(window_id > 0);
    focus_window_id(focused_window_id);
    assert((size_t)get_focused_window_id() == focused_window_id);
  }
  {
    words = get_security_words_v();
    assert(vector_size(words) > 0);
    r = tesseract_find_window_matching_word_locations(window_id, words);
  }
  {
    assert(minimize_window_id(window_id) == true);
    assert(stbi_info(r->source_file.file, &(r->source_file.width), &(r->source_file.height), &(r->source_file.stbi_format)) == 1);
  }
  {
    assert(parse_tesseract_extraction_results(r) == true);
    report_tesseract_extraction_results(r);
  }
  return(true);
}
void report_tesseract_extraction_results(struct tesseract_extract_result_t *r){
  fprintf(stdout,
          "|file:" AC_YELLOW "%s" AC_RESETALL "|mode:%lu|box#:%d"
          "|x:%d|y:%d|w:%d|h:%d|confidence:%d"
          "\n\t|text:" AC_GREEN "%s" AC_RESETALL "|"
          "\n\t|window pid:%d|w:%d|h:%d|name:%s|space:%lu|display:%lu|"
          "\n\t|file:%s|w:%d|h:%d|"
          "\n\t|"AC_GREEN "Determined Area:  max x:%d|"
          "\n                      min x offset perc: %f|"
          "\n                      max x offset perc: %f|"
          "\n                      min y offset perc: %f|"
          "\n                      max y offset perc: %f|"
          "\n             window min x offset pixels: %d|"
          "\n             window max x offset pixels: %d|"
          "\n             window min y offset pixels: %d|"
          "\n             window max y offset pixels: %d|"
          "%s",
          r->file, r->mode, r->box, r->x, r->y, r->width, r->height, r->confidence, r->text,
          r->window.pid,
          (int)(r->window.rect.size.width),
          (int)(r->window.rect.size.height), r->window.name, r->window.space_id, r->window.display_id,
          r->source_file.file,
          r->source_file.width,
          r->source_file.height,
          r->determined_area.max_x,
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
  return;
}
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
struct tesseract_extract_result_t *tesseract_find_window_matching_word_locations(size_t window_id, struct Vector *words){
  struct Vector                     *extractions;
  struct tesseract_extract_result_t *r;
  char                              *s, *extract_file, *files[2];
  int                               w, h, f;
  CGImageRef                        image_refs[2];
  {
    asprintf(&files[0], "%s/window-%lu.png", "/tmp", window_id);
    extract_file = files[0];
    if (fsio_file_exists(extract_file) == true) {
      assert(fsio_remove(extract_file) == true);
    }
  }
  {
    image_refs[0] = capture_type_capture(CAPTURE_TYPE_WINDOW, window_id);
    assert(save_cgref_to_png_file(image_refs[0], extract_file) == true);
    assert(fsio_file_size(extract_file) > 1024);
    assert(stbi_info(extract_file, &w, &h, &f) == 1);
    assert(w > 10 && h > 10 && f > 0);
  }
  {
    extractions = tesseract_extract_file_mode(extract_file, RIL_TEXTLINE);
    for (size_t i = 0; i < vector_size(extractions); i++) {
      r = (struct tesseract_extract_result_t *)vector_get(extractions, i);
      for (size_t I = 0; I < vector_size(words); I++) {
        asprintf(&s, "*%s*", stringfn_to_lowercase((char *)vector_get(words, I)));
        if (wildcardcmp(s, stringfn_to_lowercase(r->text)) == 1) {
          r->source_file.file   = extract_file;
          r->source_file.width  = w;
          r->source_file.height = h;
          r->window             = *get_window_id_info(window_id);
          assert(r->window.window_id == window_id);
          return(r);
        }
      }
    }
  }
}
struct Vector *tesseract_extract_file_mode(char *image_file, unsigned long MODE){
  struct Vector *words = vector_new();
  struct Boxa   *boxes;
  struct Pix    *img;
  BOX           *box;
  char          *c, *m;
  int           conf;
  TessBaseAPI   *api;
  {
    api = TessBaseAPICreate();
    assert(api != NULL);
    assert(TessBaseAPIInit3(api, NULL, tess_lang) == EXIT_SUCCESS);
    img = pixRead(image_file);
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
                 "|file:" AC_YELLOW "%s" AC_RESETALL "|mode:%lu|box#:%d/%d"
                 "|x:%d|y:%d|w:%d|h:%d|confidence:%d"
                 "|text:" AC_GREEN "%s" AC_RESETALL "|",
                 image_file, MODE, box->refcount, boxes->n, box->x, box->y, box->w, box->h, conf, c
                 );
        if (TESSERACT_UTILS_DEBUG_MODE) {
          log_info("%s", m);
        }
        struct tesseract_extract_result_t *r = calloc(1, sizeof(struct tesseract_extract_result_t));
        r->x      = box->x;
        r->y      = box->y;
        r->width  = box->w;
        r->height = box->h;
        asprintf(&(r->text), "%s", c);
        r->mode       = MODE;
        r->box        = box->refcount;
        r->file       = strdup(image_file);
        r->confidence = conf;
        vector_push(words, (void *)r);
      }
    }
  }
  pixDestroy(&img);
  TessBaseAPIEnd(api);
  TessBaseAPIDelete(api);
  return(words);
} /* tesseract_extract_file_mode */

#endif
