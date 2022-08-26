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

  D->capture = calloc(1, sizeof(struct display_image_t));
  D->save    = calloc(1, sizeof(struct display_image_t));
  D->resize  = calloc(1, sizeof(struct display_image_t));

  D->id            = DISPLAY_ID;
  D->capture->rect = CGDisplayBounds(D->id);
  D->save->rect    = CGDisplayBounds(D->id);
  asprintf(&D->capture_file_name, "display-%lu-%lld.%s", D->id, timestamp(), SAVE_IMAGE_EXTENSION);
  asprintf(&D->capture_file_name, "display-%lu.%s", D->id, SAVE_IMAGE_EXTENSION);

  log_debug("   [Init Display #%lu]    %fx%f|%fx%f",
            D->id,
            D->capture->rect.size.width, D->capture->rect.size.height,
            D->capture->rect.origin.x, D->capture->rect.origin.y
            );
  return(D);
}

bool save_captured_display(struct display_t *D){
  {
    D->save->buffer_size = D->capture->buffer_size;
    D->save->buffer      = calloc(1, D->save->buffer_size);
    memcpy(D->save->buffer, D->capture->buffer, D->save->buffer_size);
  }
  D->save->provider_ref = CGDataProviderCreateWithData(
    NULL,
    D->save->buffer,
    D->save->rect.size.width * D->save->rect.size.height * 4,
    NULL
    );
  D->save->color_space_ref = CGColorSpaceCreateDeviceRGB();
  D->save->image_ref       = CGImageCreate(
    D->save->rect.size.width, D->save->rect.size.height,
    BITS_PER_COMPONENT,
    32,
    D->save->rect.size.width * 4,
    D->save->color_space_ref,
    kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little,
    D->save->provider_ref,
    NULL,
    false,
    kCGRenderingIntentDefault
    );
  D->save->url_ref         = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfstring_from_cstring(D->capture_file_name), kCFURLPOSIXPathStyle, false);
  D->save->destination_ref = CGImageDestinationCreateWithURL(D->save->url_ref, SAVE_IMAGE_TYPE, 1, NULL);
  CGImageDestinationAddImage(D->save->destination_ref, D->save->image_ref, NULL);
  D->save->success = (CGImageDestinationFinalize(D->save->destination_ref));
  {
    CFRelease(D->save->destination_ref);
    CFRelease(D->save->url_ref);
    CGImageRelease(D->save->image_ref);
    CGColorSpaceRelease(D->save->color_space_ref);
    CGDataProviderRelease(D->save->provider_ref);
  }
}

bool capture_display(struct display_t *D){
  D->capture->started_ms = timestamp();
  {
    D->capture->image_ref      = CGDisplayCreateImageForRect(D->id, D->capture->rect);
    D->capture->provider_ref   = CGImageGetDataProvider(D->capture->image_ref);
    D->capture->data_ref       = CGDataProviderCopyData(D->capture->provider_ref);
    D->capture->bits_per_pixel = CGImageGetBitsPerPixel(D->capture->image_ref) / 8;
    D->capture->buffer_size    = D->capture->bits_per_pixel * D->capture->rect.size.width * D->capture->rect.size.height;
    D->capture->buffer         = calloc(1, D->capture->buffer_size);
    memcpy(D->capture->buffer, CFDataGetBytePtr(D->capture->data_ref), D->capture->buffer_size);
  }
  D->capture->success = (D->capture->buffer_size > 0) ? true : false;

  {
    CFRelease(D->capture->data_ref);
    CGImageRelease(D->capture->image_ref);
  }
  D->capture->dur_ms = timestamp() - D->capture->started_ms;

  log_debug("   [Capture Display #%lu]    %fx%f|%fx%f with %s buffer|success:%s|",
            D->id,
            D->capture->rect.size.width, D->capture->rect.size.height,
            D->capture->rect.origin.x, D->capture->rect.origin.y,
            bytes_to_string(D->capture->buffer_size),
            D->capture->success == true ? "Yes" : "No"
            );
  return(D->capture->success);
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
    bool success = capture_display((struct display_t *)vector_get(C->displays_v, i));
    if (i == 0) {
      C->success = success;
    }else{
      C->success = (success == true && C->success == true);
    }
  }
  C->dur_ms = timestamp() - C->started_ms;
  log_debug("   [Screen Capture] captured %lu displays in %s",
            C->displays_qty,
            milliseconds_to_string(C->dur_ms)
            );
  return(C);
}
