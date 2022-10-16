#pragma once
#include "active-app/active-app.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "core/utils/utils.h"
#include "image/utils/utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "screen/utils/utils.h"
#include "submodules/log/log.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
static bool SCREEN_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__screen_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_SCREEN_UTILS") != NULL) {
    log_debug("Enabling Screen Utils Debug Mode");
    SCREEN_UTILS_DEBUG_MODE = true;
  }
}
/////////////////////////////////////////////////////
bool save_captures(struct screen_capture_t *S);
struct screen_capture_t *init_screen_capture();
struct screen_t *init_display(size_t DISPLAY_ID);

///////////////////////////////////////////////////////////////////

struct screen_t *init_display(size_t DISPLAY_ID){
  struct screen_t *D = calloc(1, sizeof(struct screen_t));
  {
    D->capture = calloc(1, sizeof(struct display_image_t));
    D->save    = calloc(1, sizeof(struct display_image_t));
  }
  {
    D->id            = DISPLAY_ID;
    D->capture->rect = CGDisplayBounds(D->id);
    D->save->rect    = CGDisplayBounds(D->id);
  }

  if (D->debug_mode == true) {
    asprintf(&D->save_file_name, "display-%lu.%s", D->id, SAVE_IMAGE_EXTENSION);
  }else{
    asprintf(&D->save_file_name, "display-%lu-%lld.%s", D->id, timestamp(), SAVE_IMAGE_EXTENSION);
  }

  if (SCREEN_UTILS_DEBUG_MODE == true) {
    log_debug("   [Init Display #%lu]    %fx%f|%fx%f",
              D->id,
              D->capture->rect.size.width, D->capture->rect.size.height,
              D->capture->rect.origin.x, D->capture->rect.origin.y
              );
  }
  return(D);
}

CGImageRef preview_display_id(size_t display_id){
  CGImageRef img_ref = capture_display_id(display_id);

  return(resize_cgimage(img_ref, CGImageGetWidth(img_ref) / PREVIEW_FACTOR, CGImageGetHeight(img_ref) / PREVIEW_FACTOR));
}

  CGImageRef capture_display_id_height(size_t display_id, size_t height){
    CGImageRef img_ref = capture_display_id(display_id);
    int        w[2], h[2];

    w[0] = CGImageGetWidth(img_ref);
    h[0] = CGImageGetHeight(img_ref);
    h[1] = height;
    float factor = 1;
    if(h[0] > 100){
      factor = (float)(h[0]) / (float)(h[1]);
    }

    w[1] = (int)((float)w[0] / factor);
    return(resize_cgimage(img_ref, w[1], h[1]));
  }

  CGImageRef capture_display_id_width(size_t display_id, size_t width){
    CGImageRef img_ref = capture_display_id(display_id);
    int        w[2], h[2];

    w[0] = CGImageGetWidth(img_ref);
    h[0] = CGImageGetHeight(img_ref);
    w[1] = width;
    float factor = (float)(w[0]) / (float)(w[1]);

    h[1] = (int)((float)h[0] / factor);
    return(resize_cgimage(img_ref, w[1], h[1]));
  }

CGImageRef capture_display_id_rect(size_t display_id, CGRect rect){
  return(CGDisplayCreateImageForRect(display_id, rect));
}

CGImageRef capture_display_id(size_t display_id){
  return(CGDisplayCreateImage(display_id));
}

static bool capture_display(struct screen_t *D){
  float resize_factor = 0.3;

  D->capture->started_ms = timestamp();
  CGImageRef image_ref;
  {
    image_ref                    = CGDisplayCreateImageForRect(D->id, D->capture->rect);
    D->capture->rect.size.width  = D->capture->rect.size.width * resize_factor;
    D->capture->rect.size.height = D->capture->rect.size.height * resize_factor;
    D->save->rect.size.width     = D->capture->rect.size.width;
    D->save->rect.size.height    = D->capture->rect.size.height;
    D->capture->image_ref        = resize_cgimage(image_ref, D->capture->rect.size.width, D->capture->rect.size.height);
    D->capture->provider_ref     = CGImageGetDataProvider(D->capture->image_ref);
    D->capture->data_ref         = CGDataProviderCopyData(D->capture->provider_ref);
    D->capture->buffer_size      = ((int)(CGImageGetBitsPerPixel(D->capture->image_ref) / 8)) * D->capture->rect.size.width * D->capture->rect.size.height;
    D->capture->buffer           = calloc(1, D->capture->buffer_size);
    memcpy(D->capture->buffer, CFDataGetBytePtr(D->capture->data_ref), D->capture->buffer_size);
  }
  {
    D->capture->success = (D->capture->buffer_size > 0 && D->capture->buffer != NULL) ? true : false;
  }
  {
    CFRelease(D->capture->data_ref);
    CGImageRelease(image_ref);
  }

  D->capture->dur_ms = timestamp() - D->capture->started_ms;

  if (SCREEN_UTILS_DEBUG_MODE == true) {
    log_debug("   [Capture Display #%lu] %fx%f | with %s buffer|success:%s|%s|%s|",
              D->id,
              D->capture->rect.size.width, D->capture->rect.size.height,
              bytes_to_string(D->capture->buffer_size),
              D->capture->success == true ? "Yes" : "No",
              milliseconds_to_string(D->capture->dur_ms),
              bytes_to_string(D->capture->buffer_size)
              );
  }
  return(D->capture->success);
}

struct screen_capture_t *screen_capture(){
  struct screen_capture_t *C = init_screen_capture();

  C->success    = false;
  C->started_ms = timestamp();
  for (size_t i = 0; i < vector_size(C->displays_v); i++) {
    bool success = capture_display((struct screen_t *)vector_get(C->displays_v, i));
    if (i == 0) {
      C->success = success;
    }else{
      C->success = (success == true && C->success == true);
    }
  }
  C->dur_ms = timestamp() - C->started_ms;
  if (SCREEN_UTILS_DEBUG_MODE == true) {
    log_debug("   [Screen Capture] captured %lu displays in %s",
              vector_size(C->displays_v),
              milliseconds_to_string(C->dur_ms)
              );
  }
  return(C);
}

struct screen_capture_t *init_screen_capture(){
  struct screen_capture_t *C = calloc(1, sizeof(struct screen_capture_t));

  C->started_ms  = timestamp();
  C->display_ids = calloc(MAX_DISPLAYS, sizeof(CGDirectDisplayID));
  C->displays_v  = vector_new();
  CGError get_displays_result = CGGetActiveDisplayList(UCHAR_MAX, C->display_ids, &(C->displays_qty));

  if (get_displays_result == kCGErrorSuccess) {
    for (size_t i = 0; i < C->displays_qty && i < MAX_DISPLAYS; i++) {
      struct display_image_t *D = init_display(i);
      vector_push(C->displays_v, (void *)D);
    }
  }
  C->dur_ms = timestamp() - C->started_ms;
  if (SCREEN_UTILS_DEBUG_MODE == true) {
    log_debug("   [Init Screen Capture] with %lu displays in %s",
              vector_size(C->displays_v),
              milliseconds_to_string(C->dur_ms)
              );
  }
  return(C);
}
