#pragma once
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "core-utils/core-utils.h"
#include "core-utils/core-utils.h"
#include "image-utils/image-utils.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process/process.h"
#include "screen-utils/screen-utils.h"
#include "submodules/log.h/log.h"
#include "system-utils/system-utils.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "wrec-capture/wrec-capture.h"
#include "wrec-common/image.h"
static bool SCREEN_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__screen_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_SCREEN_UTILS") != NULL) {
    log_debug("Enabling Screen Utils Debug Mode");
    SCREEN_UTILS_DEBUG_MODE = true;
  }
}
/////////////////////////////////////////////////////
static bool save_captured_display(struct screen_t *D);
static bool capture_display(struct screen_t *D);
static bool save_to_file_qoi(struct screen_t *D);
bool save_captures(struct screen_capture_t *S);
struct screen_capture_t *init_screen_capture();
struct screen_t *init_display(size_t DISPLAY_ID);

///////////////////////////////////////////////////////////////////
static bool save_to_file_qoi(struct screen_t *D){
  struct qoi_encode_to_file_request_t *qoi_file_req = calloc(1, sizeof(struct qoi_encode_to_file_request_t));

  qoi_file_req->desc = calloc(1, sizeof(struct qoi_desc));
  asprintf(&qoi_file_req->qoi_file_path, "display-%lu.qoi", D->id);
  qoi_file_req->rgb_pixels       = D->capture->buffer;
  qoi_file_req->desc->width      = D->capture->rect.size.width;
  qoi_file_req->desc->height     = D->capture->rect.size.height;
  qoi_file_req->desc->channels   = RGBA_CHANNELS;
  qoi_file_req->desc->colorspace = QOI_SRGB;
  struct qoi_encode_to_file_result_t *qoi_encode_to_file_result = qoi_encode_to_file(qoi_file_req);
}

static bool save_captured_display(struct screen_t *D){
  D->save->started_ms = timestamp();

  D->save->url_ref         = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfstring_from_cstring(D->save_file_name), kCFURLPOSIXPathStyle, false);
  D->save->destination_ref = CGImageDestinationCreateWithURL(D->save->url_ref, SAVE_IMAGE_TYPE, 1, NULL);
  CGImageDestinationAddImage(D->save->destination_ref, D->capture->image_ref, NULL);
  D->save->success = (CGImageDestinationFinalize(D->save->destination_ref));
  {
    CGImageRelease(D->capture->image_ref);
  }
  D->save->dur_ms = timestamp() - D->save->started_ms;
  if (SCREEN_UTILS_DEBUG_MODE == true) {
    log_debug("   [Save Display #%lu]  File %s |success:%s| in %s | %s | %fx%f | ",
              D->id,
              D->save_file_name,
              D->save->success == true ? "Yes" : "No",
              milliseconds_to_string(D->capture->dur_ms),
              bytes_to_string(D->capture->buffer_size),
              D->capture->rect.size.width, D->capture->rect.size.height
              );
  }
} /* save_captured_display */

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

CGImageRef capture_display_id(size_t display_id){
  char *display_uuid = get_display_uuid(display_id);

  if (!display_uuid) {
    log_error("Cannot find display #%lu", display_id);
    return(NULL);
  }
  if (SCREEN_UTILS_DEBUG_MODE) {
    log_debug("Capturing Display #%lu |uuid:%s|",
              display_id, display_uuid
              );
  }
  CGImageRef img_ref = CGDisplayCreateImage(display_id);

  return(img_ref);
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

bool save_captures(struct screen_capture_t *S){
  for (size_t i = 0; i < vector_size(S->displays_v); i++) {
    struct display_image_t *D = (struct screen_t *)vector_get(S->displays_v, i);
    D->resize_factor = S->resize_factor;
    if (SCREEN_UTILS_DEBUG_MODE == true) {
      log_info("resize %f", D->resize_factor);
    }
    if (S->save_png_file == true) {
      save_captured_display(D);
    }
    if (S->save_qoi_file == true) {
      save_to_file_qoi(D);
    }
  }
  return(true);
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
