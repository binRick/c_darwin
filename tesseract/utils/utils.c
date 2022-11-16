#pragma once
#ifndef TESSERACT_UTILS_C
#define TESSERACT_UTILS_C
#define CAPTURE_SCREEN_RECT             true
#define TESSERACT_EXTRACT_IMAGE_TYPE    IMAGE_TYPE_PNG
#include "allheaders.h"
#include "tesseract/capi.h"
////////////////////////////////////////////
#include "tesseract/utils/utils.h"
#include "timestamp/timestamp.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "ansi-utils/ansi-utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "c_workqueue/include/workqueue.h"
#include "capture/type/type.h"
#include "capture/utils/utils.h"
#include "chan/src/chan.h"
#include "core/image/image.h"
#include "core/utils/utils.h"
#include "frameworks/frameworks.h"
#include "image/utils/utils.h"
#include "keyboard/utils/utils.h"
#include "kitty/msg/msg.h"
#include "log/log.h"
#include "mouse/utils/utils.h"
#include "ms/ms.h"
#include "pasteboard/pasteboard.h"
#include "tempdir.c/tempdir.h"
#include "tesseract/utils/static.c"
#include "vips/vips.h"
#include "wildcardcmp/wildcardcmp.h"
#include "window/utils/utils.h"

////////////////////////////////////////////
#include "tesseract/utils/define.c"

struct Vector *get_security_words_v(){
  struct Vector *words = vector_new();

  vector_push(words, (void *)"click");
  vector_push(words, (void *)"lock");
  vector_push(words, (void *)"make");
  vector_push(words, (void *)"changes");
  return(words);
}

bool tesseract_security_preferences_logic(int space_id){
  struct tesseract_extract_result_t *r, *words;
  size_t                            focused_window_id, window_id;
  char                              *error = NULL;

  {
    focused_window_id = get_focused_window_id();
    assert(focused_window_id > 0);
    window_id = open_system_preferences_get_window_id();
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

  if (space_id > -1)
    if (!(set_window_id_to_space((size_t)(window_id), (int)(space_id))))
      log_error("Failed to move window %lu to space %d", window_id, space_id);
  {
    words = get_security_words_v();
    if (vector_size(words) <= 0)
      return(false);

    r = tesseract_find_item_matching_word_locations(window_id, words);
    if (TESSERACT_UTILS_DEBUG_MODE)
      log_debug("Found %s Bytes of text", bytes_to_string(strlen(r->text)));
  }
  {
    if (MINIMIZE_PREFERENCES && minimize_window_id(window_id) != true)
      return(false);
  }
  {
    if (TESSERACT_UTILS_DEBUG_MODE)
      log_debug("Parsing Extraction Results");
    if (parse_tesseract_extraction_results(r) != true)
      return(false);

    if (TESSERACT_UTILS_DEBUG_MODE)
      log_debug("Reporting");
    report_tesseract_extraction_results(r);
  }

  struct CGPoint *p;

  int            mouse_to[20];

  mouse_to[0] = (int)(r->window.rect.origin.x) + (int)(r->determined_area.x_min_offset_pixels) + r->x;
  mouse_to[1] = (int)(r->window.rect.origin.y) + (int)(r->determined_area.y_min_offset_pixels) + r->y;
  mouse_to[2] = mouse_to[0] + 260;
  mouse_to[3] = mouse_to[1] - 230;
  mouse_to[4] = mouse_to[2] + 20;
  mouse_to[5] = mouse_to[3] + 60;
  mouse_to[6] = mouse_to[4] + 170;
  mouse_to[7] = mouse_to[5] - 20;
  mouse_to[8] = mouse_to[6] - 190;
  mouse_to[9] = mouse_to[7] - 0;
  log_info("Moving mouse to %dx%d", mouse_to[0], mouse_to[1]);
  DEBUG_MOUSE_LOCATION();
  move_mouse(mouse_to[0], mouse_to[1]);
  DEBUG_MOUSE_LOCATION();
  usleep(1000 * 100);
  focus_window_id(window_id);
  usleep(1000 * 100);
  left_click_mouse();
  usleep(1000 * 100);
  move_mouse(mouse_to[2], mouse_to[3]);
  usleep(1000 * 500);
  right_click_mouse();
  usleep(1000 * 500);
  move_mouse(mouse_to[4], mouse_to[5]);
  usleep(1000 * 500);
  char *cb = read_clipboard();

  if (!getenv("PASS")) {
    log_error("PASS env var not set!");
    return(false);
  }
  char *pass = getenv("PASS");

  copy_clipboard(pass);
  left_click_mouse();
  usleep(1000 * 500);
  copy_clipboard(cb);
  move_mouse(mouse_to[6], mouse_to[7]);
  usleep(1000 * 500);
  left_click_mouse();
  usleep(1000 * 1000);
  CGImageRef    image_ref = capture_type_capture(CAPTURE_TYPE_WINDOW, window_id);
  int           w         = CGImageGetWidth(image_ref);
  int           h         = CGImageGetHeight(image_ref);
  size_t        img_len   = 0;
  unsigned char *img      = save_cgref_to_image_type_memory(IMAGE_TYPE_PNG, image_ref, &(img_len));

  log_info("captured %dx%d Window with %s Pixels", w, h, bytes_to_string(img_len));
  if (kitty_display_image_buffer(img, img_len))
    printf("\n");

  usleep(1000 * 500);
  move_mouse(mouse_to[8], mouse_to[9]);
  CGPoint *add_position = get_mouse_location();

  log_info("Add app button position: %dx%d",
           (int)(add_position->x),
           (int)(add_position->y)
           );
  char   *apps[] = {
//    "target/release/osx/Alacritty.app",
    // "kitty-alpha/alpha.app",
    //  "kitty-code/code.app",
    //   "kitty-compiler/compiler.app",
    //    "kitty-entr/entr.app",
    //     "kitty-left/left.app",
    "kitty-main/main.app",
    //     "kitty-primary/primary.app",
    //       "kitty-richard/richard.app",
    //     "kitty-right/right.app",
    //     "kitty-secondary/secondary.app",
  };
  size_t apps_qty = sizeof(apps) / sizeof(apps[0]);
  char   *osa, *app_path;

  for (size_t i = 0; i < apps_qty; i++) {
    focus_window_id(window_id);
    usleep(1000 * 100);
    move_mouse((int)(add_position->x), (int)(add_position->y));
    usleep(1000 * 100);
    left_click_mouse();
    usleep(1000 * 100);
    asprintf(&app_path, "/Users/rick/repos/%s", apps[i]);
    asprintf(&osa, SET_FILE, app_path);
    errno = 0;
    if (!run_osascript(osa)) {
      log_error("Failed to run set file osascript");
      return(false);
    }
    usleep(1000 * 500);
  }
  vector_clear(words);
  vector_push(words, (void *)(char *)"Accessibility");
  r = tesseract_find_item_matching_word_locations(window_id, words);
  log_info("Found  Results: %dx%d", (int)(r->window.rect.origin.x), (int)(r->window.rect.origin.y));

  if (TESSERACT_UTILS_DEBUG_MODE)
    log_debug("Closing System Preferences");
  assert(run_osascript(CLOSE_SYSTEM_PREFERENCES) == true);

  return(true);

log_error:
  if (error)
    log_error("%s", error);
  return(false);
} /* tesseract_security_preferences_logic */

void report_tesseract_extraction_results(struct tesseract_extract_result_t *r){
  struct  StringFNStrings lines = stringfn_split_lines_and_trim(r->text);
  char                    *msg;

  if (TESSERACT_UTILS_DEBUG_MODE)
    log_debug("Extracted Text: %s", r->text);
  int term_width = 0, term_height = 0;

  au_term_size(&term_width, &term_height);
  asprintf(&msg,
           "---------------------------------------------------"
           "\n|  %sCapture%s          :  |Duration:%s|" AC_RESETALL
           "\n|  %sWindow%s           :  |PID:%d|Size:%dx%d|Name:%s|"
           "\n|  %sLocation%s         :  |%dx%d|"
           "\n|  %sImage%s            :  |Size:%s|Type:%s"
           "\n|  %sTesseract%s        "
           "\n|                          |%s%s%s|"
           "\n|                          |Confidence:%d|Box:%d|"
           "\n|                          |Size:%dx%x|Width:%d|Height:%d|Mode:%ld|"
           "\n|  %sSpace%s            :  "AC_CYAN "%lu"AC_RESETALL
           "\n|  %sDisplay%s          :  |Index:%d|"
           "\n|  %sTerminal%s         :  |Width:%d|Height:%d|"
           "\n|  %sScreenshot%s       :  %dx%d %s %s"
           "\n|  %sDetermined Area%s  :  "
           "\n|      Max x             %d  "
           "\n|      Min x offset      %f  "
           "\n|      Max x offset      %f  "
           "\n|      Min y offset      %f  "
           "\n|      Max y offset      %f  "
           "\n|      Window min x offset pixels: %d"
           "\n|      Window max x offset pixels: %d"
           "\n|      Window min y offset pixels: %d"
           "\n|      Window max y offset pixels: %d"
           "\n"
           "---------------------------------------------------"
           "\n%s",
           AC_GREEN, AC_RESETALL, milliseconds_to_string(timestamp() - r->started),
           AC_RED, AC_RESETALL, r->window.pid, (int)(r->window.rect.size.width), (int)(r->window.rect.size.height), r->window.name,
           AC_RED, AC_RESETALL, (int)(r->window.rect.origin.x), (int)(r->window.rect.origin.y),
           AC_GREEN, AC_RESETALL, bytes_to_string(r->img_len), image_type_name(r->format),
           AC_MAGENTA, AC_RESETALL, AC_BRIGHT_YELLOW_BLACK AC_ITALIC AC_INVERSE, stringfn_trim(lines.strings[0]), AC_RESETALL,
           r->confidence, r->box, r->x, r->y, r->width, r->height, r->mode,
           AC_BLUE, AC_RESETALL, r->window.space_id,
           AC_YELLOW, AC_RESETALL, get_display_id_index(r->window.display_id),
           AC_MAGENTA, AC_RESETALL, term_width, term_height,
           AC_CYAN, AC_RESETALL, r->source_file.width, r->source_file.height, bytes_to_string(r->img_len), get_capture_type_name(r->type),
           AC_GREEN, AC_RESETALL, r->determined_area.max_x,
           r->determined_area.x_min_offset_perc,
           r->determined_area.x_max_offset_perc,
           r->determined_area.y_min_offset_perc,
           r->determined_area.y_max_offset_perc,
           r->determined_area.x_min_offset_pixels,
           r->determined_area.x_max_offset_pixels,
           r->determined_area.y_min_offset_pixels,
           r->determined_area.y_max_offset_pixels,
           ""
           );
  kitty_display_image_buffer(r->img, r->img_len);
  fprintf(stdout, "%s", msg);
  free(msg);
  stringfn_release_strings_struct(lines);
  return;
} /* report_tesseract_extraction_results */

bool parse_tesseract_extraction_results(struct tesseract_extract_result_t *r){
  r->determined_area.x_max_offset_perc   = (float)((float)((float)(r->x) / ((float)(r->source_file.width))));
  r->determined_area.x_min_offset_perc   = (r->determined_area.x_max_offset_perc) / 2;
  r->determined_area.y_min_offset_perc   = (float)((float)((float)(r->y) / ((float)(r->source_file.height))));
  r->determined_area.y_max_offset_perc   = 1;
  r->determined_area.x_max_offset_pixels = (int)((r->determined_area.x_max_offset_perc) * (float)(r->window.rect.size.width));
  r->determined_area.x_min_offset_pixels = (r->determined_area.x_max_offset_pixels) / 2;
  r->determined_area.y_min_offset_pixels = (int)((r->determined_area.y_min_offset_perc) * (float)(r->window.rect.size.height));
  r->determined_area.y_max_offset_pixels = (int)((r->determined_area.y_min_offset_perc) * (float)(r->window.rect.size.height));
  return(true);
}
struct tesseract_extract_result_t *tesseract_find_item_matching_word_locations(size_t window_id, struct Vector *words){
  struct Vector                     *extractions;
  struct tesseract_extract_result_t *r;
  CGRect                            rect, orig_rect;
  char                              *s;
  int                               w = 0, h = 0;
  CGImageRef                        image_refs[1];
  size_t                            img_len = 0;
  unsigned char                     *img    = NULL;
  enum image_type_id_t              format  = TESSERACT_EXTRACT_IMAGE_TYPE;
  {
    if (TESSERACT_UTILS_DEBUG_MODE)
      log_debug("Working with Window #%lu", window_id);
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
    }else
      image_refs[0] = capture_type_capture(CAPTURE_TYPE_WINDOW, window_id);
    w = CGImageGetWidth(image_refs[0]);
    h = CGImageGetHeight(image_refs[0]);
    assert(w > 10 && h > 10);
    img = save_cgref_to_image_type_memory(format, image_refs[0], &(img_len));
    assert(img_len > 0);
    assert(img);
  }

  if (TESSERACT_UTILS_DEBUG_MODE)
    log_debug("Captured %lux%lu %s Window Image", CGImageGetWidth(image_refs[0]), CGImageGetHeight(image_refs[0]), bytes_to_string(img_len));
  {
    unsigned long ts = timestamp();
    extractions = tesseract_extract_memory(img, img_len, RIL_TEXTLINE);
    if (TESSERACT_UTILS_DEBUG_MODE)
      log_info("ran extract tess in %s and received %lu Extractions", milliseconds_to_string(timestamp() - ts), vector_size(extractions));
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
          r->format             = format;
          assert(r->window.window_id == window_id);
          return(r);
        }
      }
    }
  }
} /* tesseract_find_item_matching_word_locations */

static void __tesseract_capture_item(void *ARGS){
  unsigned long starteds[3];

  starteds[0] = timestamp();
  struct TesseractArgs *r = (struct TesseractArgs *)ARGS;

  r->img_ref    = capture_type_capture(r->type, r->id);
  r->img_width  = CGImageGetWidth(r->img_ref);
  r->img_height = CGImageGetHeight(r->img_ref);
  assert(r->img_width > 10 && r->img_height > 10);
  r->img = NULL;
  r->img = save_cgref_to_image_type_memory(r->format, r->img_ref, &(r->img_len));
  assert(r->img_len > 0);
  assert(r->img != NULL);
  if (TESSERACT_UTILS_DEBUG_MODE)
    log_info("\tCaptured %lux%lu %s Image of Window ID %lu in %s", r->img_width, r->img_height, bytes_to_string(r->img_len), r->id, milliseconds_to_string(timestamp() - starteds[0]));
}

static void __tesseract_extract_item(void *ARGS){
  struct TesseractArgs *r = (struct TesseractArgs *)ARGS;
  unsigned long        starteds[3];

  starteds[0] = timestamp();
  starteds[1] = timestamp();
  starteds[2] = timestamp();
  if (TESSERACT_UTILS_DEBUG_MODE)
    log_info(AC_YELLOW "\tExtracting Item #%lu"AC_RESETALL, r->id);

  r->results_v = tesseract_extract_memory(r->img, r->img_len, RIL_TEXTLINE);
  if (TESSERACT_UTILS_DEBUG_MODE)
    log_info(AC_GREEN "\tExtracted %lu items from %s Image in %s"AC_RESETALL,
             vector_size(r->results_v),
             bytes_to_string(r->img_len),
             milliseconds_to_string(timestamp() - starteds[1])
             );
}
struct Vector *tesseract_extract_items(struct Vector *v, size_t concurrency){
  struct Vector    *Args = vector_new();
  unsigned long    started = timestamp();
  struct WorkQueue *extract_queues[concurrency], *capture_queues[concurrency];

  for (size_t i = 0; i < concurrency; i++) {
    capture_queues[i] = workqueue_new_with_options(vector_size(v), NULL);
    extract_queues[i] = workqueue_new_with_options(vector_size(v), NULL);
  }
  unsigned long cap_started = timestamp();

  for (size_t i = 0; i < vector_size(v); i++) {
    struct TesseractArgs *a = calloc(1, sizeof(struct TesseractArgs));
    a->id        = (size_t)vector_get(v, i);
    a->results_v = vector_new();
    a->format    = IMAGE_TYPE_PNG;
    vector_push(Args, (void *)a);
    if (!workqueue_push(capture_queues[i % concurrency], __tesseract_capture_item, a))
      log_error("Failed to push work function to queue\n");
  }
  for (size_t c = 0; c < concurrency; c++)
    workqueue_get_backlog_size(capture_queues[c]);
  for (size_t c = 0; c < concurrency; c++)
    workqueue_drain(capture_queues[c]);
  for (size_t c = 0; c < concurrency; c++)
    workqueue_release(capture_queues[c]);
  if (TESSERACT_UTILS_DEBUG_MODE)
    log_info("Captured in %s", milliseconds_to_string(timestamp() - cap_started));
  unsigned long extract_started = timestamp();

  for (size_t i = 0; i < vector_size(Args); i++)
    if (!workqueue_push(extract_queues[i % concurrency], __tesseract_extract_item, (struct TesseractArgs *)vector_get(Args, i)))
      log_error("Failed to push work function to queue\n");
  for (size_t c = 0; c < concurrency; c++)
    workqueue_get_backlog_size(extract_queues[c]);
  for (size_t c = 0; c < concurrency; c++)
    workqueue_drain(extract_queues[c]);
  for (size_t c = 0; c < concurrency; c++)
    workqueue_release(extract_queues[c]);
  if (TESSERACT_UTILS_DEBUG_MODE)
    log_info("Extraction finished in %s", milliseconds_to_string(timestamp() - extract_started));
  log_info("Finished in %s", milliseconds_to_string(timestamp() - started));
  for (size_t i = 0; i < vector_size(Args); i++)
    log_info("Item ID %lu has %lu results",
             ((struct TesseractArgs *)vector_get(Args, i))->id,
             vector_size(((struct TesseractArgs *)vector_get(Args, i))->results_v)
             );
  return(Args);
} /* tesseract_extract_items */
struct Vector *tesseract_extract_memory(unsigned char *img_data, size_t img_data_len, unsigned long MODE){
  unsigned long ts     = timestamp();
  struct Vector *words = vector_new();
  struct Boxa   *boxes;
  struct Pix    *img;
  BOX           *box;
  char          *c, *m;
  int           conf;
  TessBaseAPI   *api = NULL;
  {
    api = TessBaseAPICreate();
    assert(api != NULL);
    assert(TessBaseAPIInit3(api, NULL, tess_lang) == EXIT_SUCCESS);
    if(!(img = pixReadMem(img_data, img_data_len))){
    }
    assert(img != NULL);
    TessBaseAPISetImage2(api, img);
    assert(TessBaseAPIGetInputImage(api) != NULL);
    assert(TessBaseAPIRecognize(api, NULL) == EXIT_SUCCESS);
    boxes = TessBaseAPIGetComponentImages(api, MODE, true, NULL, NULL);
  }

  if (boxes != NULL)
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
        if (TESSERACT_UTILS_DEBUG_MODE)
          log_info("%s", m);
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
  pixDestroy(&img);
  TessBaseAPIEnd(api);
  TessBaseAPIDelete(api);
  if (TESSERACT_UTILS_DEBUG_MODE)
    log_info("Returning %lu words in %s", vector_size(words), milliseconds_to_string(timestamp() - ts));
  return(words);
} /* tesseract_extract_file_mode */

#endif
