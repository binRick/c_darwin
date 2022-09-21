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
static struct Vector *tesseract_extract(size_t window_id);
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

////////////////////////////////////////////
struct tesseract_extract_result_t *tesseract_find_window_matching_word_locations(size_t window_id, struct Vector *words){
  struct Vector *extractions;
  struct tesseract_extract_result_t *r;
  char *s, *extract_file, *files[2];
  int w, h, f;
  CGImageRef    image_refs[2];
  {
    asprintf(&files[0], "%s/window-%lu.png", "/tmp", window_id);
    extract_file = files[0];
    if(fsio_file_exists(extract_file)==true) 
      assert(fsio_remove(extract_file)==true);
  }
  {
    image_refs[0] = capture_type_capture(CAPTURE_TYPE_WINDOW, window_id);
    assert(save_cgref_to_png_file(image_refs[0], extract_file) == true);
    assert(fsio_file_size(extract_file) > 1024);
    assert(stbi_info(extract_file, &w, &h, &f)==1);
    assert(w > 10 && h > 10 && f > 0);
  }
  {
    extractions = tesseract_extract_file_mode(extract_file, RIL_TEXTLINE);
    for (size_t i = 0; i < vector_size(extractions); i++) {
      r = (struct tesseract_extract_result_t *)vector_get(extractions, i);
      for (size_t I = 0; I < vector_size(words); I++) {
        asprintf(&s, "*%s*", stringfn_to_lowercase((char *)vector_get(words, I)));
        if(wildcardcmp(s, stringfn_to_lowercase(r->text)) == 1){
          r->source_file.file = extract_file;
          r->source_file.width = w;
          r->source_file.height = h;
          r->window = *get_window_id_info(window_id);
          assert(r->window.window_id == window_id);
          return(r);
      }
    }
  }
}
}
      /*
void tesseract_extract_symbols(size_t window_id){
  tesseract_extract(window_id);
}
  stbir_uint8 *rgb_pix;
  asprintf(&files[1], "%s/window-%lu-600-percent.png", "/tmp", window_id);
    stbir_uint8 *rgb50 = calloc(w /2 * h /2 * f, sizeof(stbir_uint8));
    stbir_resize_uint8(rgb_pix, w, h, 0, rgb50, w /2, h /2, 0, 4);
    stbi_write_png(files[1], w /2, h /2, 4, rgb50, 0);
    assert(fsio_file_size(files[1]) > 1024);
    rgb_pix = stbi_load(files[1], &w, &h, &f, 0);
    assert(fsio_file_size(files[1]) > 1024);
    assert(w > 10);
    assert(h > 10);
    assert(f > 0);
      extractions = tesseract_extract_file_mode(files[1], RIL_TEXTLINE);
    stbir_uint8 *rgb600 = calloc(w * 6 * h * 6 * f, sizeof(stbir_uint8));
    stbir_resize_uint8(rgb_pix, w, h, 0, rgb600, w * 6, h * 6, 0, 4);
    stbi_write_png(files[1], w * 6, h * 6, 4, rgb600, 0);
    assert(fsio_file_size(files[1]) > 1024);
    */
/*
struct Vector *tesseract_extract_text(size_t window_id){
  return(tesseract_extract(window_id));
}
#define FILES_QTY    20
static struct Vector *tesseract_extract(size_t window_id){
  struct Vector *words = vector_new();
  char          *files[FILES_QTY];
  stbir_uint8 *rgb_pix;
  asprintf(&files[4], "%s/%lu-500.png", "/tmp", window_id);
  asprintf(&files[0], "%s/%lu.png", "/tmp", window_id);
  asprintf(&files[5], "%s/%lu.gif", "/tmp", window_id);
  asprintf(&files[7], "%s/%lu.jpeg", "/tmp", window_id);
  asprintf(&files[8], "%s/%lu.tiff", "/tmp", window_id);
  asprintf(&files[9], "%s/%lu-400.png", "/tmp", window_id);
  asprintf(&files[10], "%s/%lu-600.png", "/tmp", window_id);
  int         w, h, f;
  size_t        rgb_len;
  CGImageRef    i1;
  {
    i1 = capture_type_capture(CAPTURE_TYPE_WINDOW, window_id);
    assert(save_cgref_to_png_file(i1, files[0]) == true);
    assert(fsio_file_size(files[0]) > 1024);
    rgb_pix = stbi_load(files[0], &w, &h, &f, 0);
    assert(w > 10);
    assert(h > 10);
    assert(f > 0);
    stbir_uint8 *rgb600 = calloc(w * 6 * h * 6 * f, sizeof(stbir_uint8));
    stbir_resize_uint8(rgb_pix, w, h, 0, rgb600, w * 6, h * 6, 0, 4);
    stbi_write_png(files[10], w * 6, h * 6, 4, rgb600, 0);
    assert(fsio_file_size(files[10]) > 1024);
    return(tesseract_extract_file_mode(files[10], RIL_TEXTLINE));
  }
  unsigned char *rgb;
  {
    rgb = cgimage_ref_to_rgb_pixels(i1, &rgb_len);
    assert(rgb_len > 1024);
  }
  {
    assert(stbi_info(files[0], &w, &h, &f) == 1);
    assert(w > 10);
    assert(h > 10);
    assert(f > 0);
  }
  {
  }
  {
    stbir_uint8 *rgb400 = calloc(w * 4 * h * 4 * f, sizeof(stbir_uint8));
    stbir_resize_uint8(rgb_pix, w, h, 0, rgb400, w * 4, h * 4, 0, 4);
    stbi_write_png(files[9], w * 4, h * 4, 4, rgb400, 0);
    assert(fsio_file_size(files[9]) > 1024);
  }

  {
    assert(save_cgref_to_gif_file(i1, files[5]) == true);
    assert(fsio_file_size(files[5]) > 1024);
  }
  {
    assert(save_cgref_to_jpeg_file(i1, files[7]) == true);
    assert(save_cgref_to_tiff_file(i1, files[8]) == true);
    files[2] = convert_png_to_grayscale(files[0], 500);
    files[3] = convert_png_to_grayscale(files[0], 1000);
  }

  for (size_t m = 0; m < 3; m++) {
    for (size_t i = 10; i < 11; i++) {
      if (!files[i] || (fsio_file_exists(files[i]) == false)) {
        continue;
      }
      struct Vector *I = tesseract_extract_file_mode(files[i], RIL_TEXTLINE);
      for (size_t x = 0; x < vector_size(I); x++) {
        vector_push(words, (void *)vector_get(I, x));
      }
      struct Vector *I1 = tesseract_extract_file_mode(files[i], RIL_WORD);
      for (size_t x = 0; x < vector_size(I1); x++) {
        vector_push(words, (void *)vector_get(I1, x));
      }
    }
  }
  return(words);
}
*/
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
struct Vector *get_security_words_v(){
    struct Vector *words = vector_new();
    vector_push(words, (void *)"click");
    vector_push(words, (void *)"lock");
    vector_push(words, (void *)"make");
    vector_push(words, (void *)"changes");
    return(words);
}


#endif
