#pragma once
#ifndef CAPTURE_UTILS_C
#define CAPTURE_UTILS_C
////////////////////////////////////////////
#include "capture/utils/utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "b64.c/b64.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "c_workqueue/include/workqueue.h"
#include "cache-utils/cache-utils.h"
#include "display/utils/utils.h"
#include "fs.c/fs.h"
#include "image/utils/utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "screen/utils/utils.h"
#include "space/utils/utils.h"
#include "timestamp/timestamp.h"
#include "timg/utils/utils.h"
#include "vips/vips.h"
#include "window/utils/utils.h"
////////////////////////////////////////////
static bool                  CAPTURE_UTILS_DEBUG_MODE = false;
static const size_t          CAPTURE_TYPE_TTL         = 60;
#define CACHE_ENABLED    false
static const char            *capture_types_cache_dir = NULL;
static struct capture_type_t capture_types[]          = {
  [CAPTURE_TYPE_SPACE] =   {
    .name           = "space",
    .get_default_id = ^ size_t (void){ return(get_current_space_id());                                    },
    .validate_id    = ^ bool (size_t space_id){ return((space_id > 0 && space_id < 10000) ? true : false); },
    .capture        = ^ CGImageRef (size_t space_id){
      log_info("capturing space #%lu", space_id);
      return(capture_space_id((uint32_t)space_id));
    },
    .capture_rect   = ^ CGImageRef (size_t space_id,CGRect rect){
      return(capture_space_id_rect((uint32_t)space_id, rect));
    },
    .preview   = ^ CGImageRef (size_t space_id){ return(preview_space_id((uint32_t)space_id));  },
    .width     = ^ CGImageRef (size_t space_id, size_t width){ return(capture_space_id_width((uint32_t)space_id, width)); },
    .height    = ^ CGImageRef (size_t space_id, size_t height){ return(capture_space_id_height((uint32_t)space_id, height)); },
    .get_items = ^ struct Vector *(void){ return(get_window_infos_v());                         },
  },
  [CAPTURE_TYPE_DISPLAY] = {
    .name           = "display",
    .get_default_id = ^ size_t (void){ return(get_main_display_id());                                     },
    .validate_id    = ^ bool (size_t display_id){ return((display_id > 0 && display_id < 999999999) ? true : false); },
    .capture_rect   = ^ CGImageRef (size_t display_id, CGRect rect){
      return(capture_display_id_rect((uint32_t)display_id, rect));
    },
    .capture        = ^ CGImageRef (size_t display_id){
      return(capture_display_id((uint32_t)display_id));
    },
    .preview   = ^ CGImageRef (size_t display_id){ return(preview_display_id((uint32_t)display_id)); },
    .width     = ^ CGImageRef (size_t display_id, size_t width){ return(capture_display_id_width((uint32_t)display_id, width)); },
    .height    = ^ CGImageRef (size_t display_id, size_t height){ return(capture_display_id_height((uint32_t)display_id, height)); },
    .get_items = ^ struct Vector *(void){ return(get_displays_v());                             },
  },
  [CAPTURE_TYPE_WINDOW] =  {
    .name           = "window",
    .get_default_id = ^ size_t (void){ return(get_focused_window_id());                                   },
    .validate_id    = ^ bool (size_t window_id){ return((window_id > 0 && window_id < 99999) ? true : false); },
    .capture        = ^ CGImageRef (size_t window_id){
      return(capture_window_id((uint32_t)window_id));
    },
    .capture_rect   = ^ CGImageRef (size_t window_id,CGRect rect){
      return(capture_window_id_rect((uint32_t)window_id, rect));
    },
    .preview   = ^ CGImageRef (size_t window_id){ return(preview_window_id((uint32_t)window_id)); },
    .width     = ^ CGImageRef (size_t window_id, size_t width){ return(capture_window_id_width((uint32_t)window_id, width)); },
    .height    = ^ CGImageRef (size_t window_id, size_t height){ return(capture_window_id_height((uint32_t)window_id, height)); },
    .get_items = ^ struct Vector *(void){ return(get_spaces_v());                               },
  },
};
///////////////////////////////////////////////////////////////////////

size_t capture_type_validate_id_or_get_default_id(enum capture_type_id_t capture_type_id, size_t capture_id){
  return((capture_type_validate_id(capture_type_id, capture_id) == true) ? capture_id : capture_type_get_default_id(capture_type_id));
}

size_t capture_type_get_default_id(enum capture_type_id_t capture_type_id){
  return(capture_types[capture_type_id].get_default_id());
}

bool capture_type_validate_id(enum capture_type_id_t capture_type_id, size_t capture_id){
  return(capture_types[capture_type_id].validate_id(capture_id));
}

CGImageRef capture_type_id_or_default_capture_type_id(enum capture_type_id_t capture_type_id, size_t capture_id){
  return(capture_type_capture(capture_type_id, capture_type_validate_id_or_get_default_id(capture_type_id, capture_id)));
}

CGImageRef capture_type_height(enum capture_type_id_t capture_type_id, size_t capture_id, size_t height){
  return(capture_types[capture_type_id].height(capture_id, height));
}

CGImageRef capture_type_width(enum capture_type_id_t capture_type_id, size_t capture_id, size_t width){
  log_info("capture_type_width> %d|%lu|%lu|CAPTURE_TYPE_SPACE:%d", capture_type_id,capture_id,width,CAPTURE_TYPE_SPACE);
  return(capture_types[capture_type_id].width(capture_id, width));
}

CGImageRef capture_type_preview(enum capture_type_id_t capture_type_id, size_t capture_id){
  return(capture_types[capture_type_id].preview(capture_id));
}

char *capture_type_cache_file(enum capture_type_id_t capture_type_id, size_t capture_id){
  char *f;

  asprintf(&f, "%s/%lu.png", capture_types_cache_dir, capture_type_hash(capture_type_id, capture_id));
  return(f);
}

size_t capture_type_cache_file_age_seconds(enum capture_type_id_t capture_type_id, size_t capture_id){
  return((size_t)((long)(timestamp() / 1000) - (long)(fs_stat(capture_type_cache_file(capture_type_id, capture_id))->st_atimespec.tv_sec)));
}

bool capture_type_cache_file_is_expired(enum capture_type_id_t capture_type_id, size_t capture_id){
  return((fsio_file_exists(capture_type_cache_file(capture_type_id, capture_id)) == true && capture_type_cache_file_age_seconds(capture_type_id, capture_id) < CAPTURE_TYPE_TTL) ? false : true);
}

char *capture_type_pixels_hash_key(enum capture_type_id_t capture_type_id, size_t capture_id){
  char *s;

  asprintf(&s, "%d:%lu:pixels", capture_type_id, capture_id);
  return(s);
}

size_t capture_type_hash(enum capture_type_id_t capture_type_id, size_t capture_id){
  return(cache_utils_get_hash(capture_type_pixels_hash_key(capture_type_id, capture_id)));
}

unsigned char *capture_type_cache_png(enum capture_type_id_t capture_type_id, size_t capture_id, unsigned char *pixels, size_t size){
  fsio_write_binary_file(pixels, capture_type_cache_file(capture_type_id, capture_id), size);
  return(pixels);
}

CGImageRef capture_type_cache(enum capture_type_id_t capture_type_id, size_t capture_id, CGImageRef image_ref){
  save_cgref_to_png_file(image_ref, capture_type_cache_file(capture_type_id, capture_id));
  return(image_ref);
}

unsigned char *capture_type_get_cached_png(enum capture_type_id_t capture_type_id, size_t capture_id, size_t *size){
  *size = fsio_file_size(size);
  return((*size > 0) ? fsio_read_binary_file(capture_type_cache_file(capture_type_id, capture_id)) : NULL);
}

CGImageRef capture_type_get_cached_cgimage(enum capture_type_id_t capture_type_id, size_t capture_id){
  return(png_fp_to_cgimage(fopen(capture_type_cache_file(capture_type_id, capture_id), "rb")));
}

char *capture_type_capture_png_random_file(enum capture_type_id_t capture_type_id, size_t capture_id){
  char *f;

  asprintf(&f, "%s/%lu-%lld.png", gettempdir(), capture_id, timestamp());
  return(((capture_type_capture_png_file(capture_type_id, capture_id, f)) == true) ? f : NULL);
}

bool capture_type_capture_png_file(enum capture_type_id_t capture_type_id, size_t capture_id, char *file){
  return((CACHE_ENABLED && capture_type_cache_file_is_expired(capture_type_id, capture_id) == false)
    ? fsio_copy_file(capture_type_cache_file(capture_type_id, capture_id), file)
    : save_cgref_to_png_file(capture_type_capture(capture_type_id, capture_id), file));
}

unsigned char *capture_type_capture_png(enum capture_type_id_t capture_type_id, size_t capture_id, size_t *size){
  if (CACHE_ENABLED && capture_type_cache_file_is_expired(capture_type_id, capture_id) == false) {
    return(capture_type_get_cached_png(capture_type_id, capture_id, size));
  }
  unsigned char *pixels = save_cgref_to_png_memory(capture_types[capture_type_id].capture(capture_id), size);
  return(
    (size > 0) == true
        ? (size > 0 && CACHE_ENABLED)
            ? capture_type_cache_png(capture_type_id, capture_id, pixels, *size)
            : pixels
        : NULL
    );
}

CGImageRef capture_type_capture_rect(enum capture_type_id_t capture_type_id, size_t capture_id, CGRect rect){
  if (capture_types[capture_type_id].capture_rect == NULL) {
    log_error("Capture Type #%d has no capture rect handler", capture_type_id);
    return(NULL);
  }
  return(capture_types[capture_type_id].capture_rect(capture_id, rect));
}

CGImageRef capture_type_capture(enum capture_type_id_t capture_type_id, size_t capture_id){
  if (CACHE_ENABLED && capture_type_cache_file_is_expired(capture_type_id, capture_id) == false) {
    return(capture_type_get_cached_cgimage(capture_type_id, capture_id));
  }
  CGImageRef image_ref = capture_types[capture_type_id].capture(capture_id);
  if (CACHE_ENABLED) {
    return(capture_type_cache(capture_type_id, capture_id, image_ref));
  }else{
    return(image_ref);
  }
}

struct Vector *capture_type_get_items(enum capture_type_id_t capture_type_id){
  return(capture_types[capture_type_id].get_items());
}

static void __attribute__((constructor)) __constructor__capture_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_capture_utils") != NULL) {
    log_debug("Enabling capture-utils Debug Mode");
    CAPTURE_UTILS_DEBUG_MODE = true;
  }
  asprintf(&capture_types_cache_dir, "%s/%s", gettempdir(), "capture-utils");
  fsio_mkdirs(capture_types_cache_dir, 0700);
}
#endif
