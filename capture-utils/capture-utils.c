#pragma once
#ifndef CAPTURE_UTILS_C
#define CAPTURE_UTILS_C
////////////////////////////////////////////
#include "capture-utils/capture-utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "display-utils/display-utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "screen-utils/screen-utils.h"
#include "space-utils/space-utils.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"
////////////////////////////////////////////
static bool           CAPTURE_UTILS_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
struct capture_type_t capture_types[CAPTURE_TYPES_QTY + 1] = {
  [CAPTURE_TYPE_SPACE] =   {
    .name           = "space",
    .get_default_id = ^ size_t (void){ return(get_space_id());                                            },
    .validate_id    = ^ bool (size_t space_id){ return((space_id > 0 && space_id < 10000) ? true : false); },
    .capture        = ^ CGImageRef (size_t space_id){ return(capture_space_id((uint32_t)space_id)); },
    .get_items      = ^ struct Vector *(void){ return(get_window_infos_v());                    },
  },
  [CAPTURE_TYPE_DISPLAY] = {
    .name           = "display",
    .get_default_id = ^ size_t (void){ return(get_main_display_id());                                     },
    .validate_id    = ^ bool (size_t display_id){ return((display_id > 0 && display_id < 999999999) ? true : false); },
    .capture        = ^ CGImageRef (size_t display_id){ return(capture_display_id((uint32_t)display_id)); },
    .get_items      = ^ struct Vector *(void){ return(get_displays_v());                        },
  },
  [CAPTURE_TYPE_WINDOW] =  {
    .name           = "window",
    .get_default_id = ^ size_t (void){ return(get_focused_window_id());                                   },
    .validate_id    = ^ bool (size_t window_id){ return((window_id > 0 && window_id < 99999) ? true : false); },
    .capture        = ^ CGImageRef (size_t window_id){ return(capture_window_id((uint32_t)window_id)); },
    .get_items      = ^ struct Vector *(void){ return(get_spaces_v());                          },
  },
};

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

CGImageRef capture_type_capture(enum capture_type_id_t capture_type_id, size_t capture_id){
  return(capture_types[capture_type_id].capture(capture_id));
}
struct Vector *capture_type_get_items(enum capture_type_id_t capture_type_id){
  return(capture_types[capture_type_id].get_items());
}

static void __attribute__((constructor)) __constructor__capture_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_capture_utils") != NULL) {
    log_debug("Enabling capture-utils Debug Mode");
    CAPTURE_UTILS_DEBUG_MODE = true;
  }
}
#endif
