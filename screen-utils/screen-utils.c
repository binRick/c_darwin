#pragma once
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "core-utils/core-utils.h"
#include "core-utils/core-utils.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process/process.h"
#include "screen-utils/screen-utils.h"
#include "submodules/log.h/log.h"
#include "system-utils/system-utils.h"
#include "timestamp/timestamp.h"
/////////////////////////////////////////////////////

struct display_t *init_display(size_t DISPLAY_ID){
  struct display_t *D = calloc(1, sizeof(struct display_t));

  D->id   = DISPLAY_ID;
  D->rect = CGDisplayBounds(D->id);
  asprintf(&D->capture_file_name, "display-%lu-%lld.%s", D->id, timestamp(), SAVE_IMAGE_EXTENSION);

  log_debug("   [Init Display #%lu]    %fx%f|%fx%f",
            D->id,
            D->rect.size.width, D->rect.size.height,
            D->rect.origin.x, D->rect.origin.y
            );
  return(D);
}

bool save_captured_display(struct display_t *D){
  D->provider_ref    = CGDataProviderCreateWithData(NULL, D->buffer, D->rect.size.width * D->rect.size.height * 4, NULL);
  D->color_space_ref = CGColorSpaceCreateDeviceRGB();
  D->image_ref       = CGImageCreate(
    D->rect.size.width, D->rect.size.height, BITS_PER_COMPONENT, BITS_PER_PIXEL, D->rect.size.width * RGB_BYTES_PER_ROW,
    D->color_space_ref, kCGBitmapByteOrderDefault, D->provider_ref, DECODE_ARRAY, SHOULD_INTERPOLATE, kCGRenderingIntentDefault
    );
  D->url_ref         = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfstring_from_cstring(D->capture_file_name), kCFURLPOSIXPathStyle, false);
  D->destination_ref = CGImageDestinationCreateWithURL(D->url_ref, SAVE_IMAGE_TYPE, 1, NULL);
  CGImageDestinationAddImage(D->destination_ref, D->image_ref, NULL);
  D->captured_success = (CGImageDestinationFinalize(D->destination_ref));
  CFRelease(D->destination_ref);
  CFRelease(D->url_ref);
}

bool capture_display(struct display_t *D){
  D->capture_started_ms = timestamp();
  D->image_ref          = CGDisplayCreateImageForRect(D->id, D->rect);
  D->provider_ref       = CGImageGetDataProvider(D->image_ref);
  D->bits_per_pixel     = CGImageGetBitsPerPixel(D->image_ref) / 8;
  D->data_ref           = CGDataProviderCopyData(D->provider_ref);
  D->buffer_size        = D->bits_per_pixel * D->rect.size.width * D->rect.size.height;
  D->buffer             = calloc(1, D->buffer_size);
  memcpy(D->buffer, CFDataGetBytePtr(D->data_ref), D->buffer_size);

  CFRelease(D->data_ref);
  CGImageRelease(D->image_ref);
  D->capture_dur_ms = timestamp() - D->capture_started_ms;

  log_debug("   [Capture Display #%lu]    %fx%f|%fx%f with %s buffer.",
            D->id,
            D->rect.size.width, D->rect.size.height,
            D->rect.origin.x, D->rect.origin.y,
            bytes_to_string(D->buffer_size)
            );
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
      vector_push(C->displays_v, init_display(i));
    }
  }
  C->dur_ms = timestamp() - C->started_ms;
  log_debug("   [Init Screen Capture] with %lu displays in %s",
            C->displays_qty,
            milliseconds_to_string(C->dur_ms)
            );
  return(C);
}

bool save_captures(struct screen_capture_t *S){
  for (size_t i = 0; i < S->displays_qty; i++) {
    save_captured_display((struct display_t *)vector_get(S->displays_v, i));
  }
  return(true);
}

struct screen_capture_t *screen_capture(){
  struct screen_capture_t *C = init_screen_capture();

  C->success    = false;
  C->started_ms = timestamp();
  for (size_t i = 0; i < C->displays_qty; i++) {
    capture_display((struct display_t *)vector_get(C->displays_v, i));
  }
  C->dur_ms = timestamp() - C->started_ms;
  log_debug("   [Screen Capture] captured %lu displays in %s",
            C->displays_qty,
            milliseconds_to_string(C->dur_ms)
            );
  return(C);
}
/*
 * struct screen_capture_t *save_screen_buffer(struct screen_t *S){
 *  struct screen_capture_t *C = calloc(1,sizeof(struct screen_capture_t));
 * //    C->screen = S;
 *  C->success = false;
 *  C->started_ms = timestamp();
 *
 *  CGDataProviderRef provider_ref = CGDataProviderCreateWithData(NULL, S->buffer, S->width * S->height * 4, NULL);
 *  if (provider_ref != NULL) {
 *      CGColorSpaceRef cs_ref = CGColorSpaceCreateDeviceRGB();
 *      if (cs_ref != NULL) {
 *          CGImageRef img_ref = CGImageCreate(
 *              S->width,
 *              S->height,
 *              8,          // bitsPerComponent
 *              32,         // bitsPerPixel
 *              S->width * 4,  // bytesPerRow
 *              cs_ref,
 *              kCGBitmapByteOrderDefault,
 *              provider_ref,
 *              NULL,       // decode array
 *              false,      // shouldInterpolate
 *              kCGRenderingIntentDefault
 *          );
 *
 *          if (img_ref != NULL) {
 *              CFURLRef url_ref = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("screenshot.png"), kCFURLPOSIXPathStyle, false);
 *              if (url_ref != NULL) {
 *                  CGImageDestinationRef destination_ref = CGImageDestinationCreateWithURL(url_ref, kUTTypePNG, 1, NULL);
 *                  if (destination_ref != NULL) {
 *                      CGImageDestinationAddImage(destination_ref, img_ref, NULL);
 *                      if (CGImageDestinationFinalize(destination_ref)) {
 *                          C->success = true;
 *                      }
 *
 *                      CFRelease(destination_ref);
 *                  }
 *
 *                  CFRelease(url_ref);
 *              }
 *
 *              CGImageRelease(img_ref);
 *          }
 *
 *          CGColorSpaceRelease(cs_ref);
 *      }
 *
 *      CGDataProviderRelease(provider_ref);
 *  }
 *
 *  C->dur_ms = timestamp() - C->started_ms;
 *  return C;
 * }
 */
