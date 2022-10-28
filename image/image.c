#pragma once
#ifndef IMAGE_C
#define IMAGE_C
////////////////////////////////////////////
#include "image/image.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

////////////////////////////////////////////
static bool IMAGE_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__image(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_image") != NULL) {
    log_debug("Enabling image Debug Mode");
    IMAGE_DEBUG_MODE = true;
  }
}
static void image_internal_fxn1(void);

////////////////////////////////////////////
static void image_internal_fxn1(void){
  log_info("Called function image_internal_fxn1");
  if (IMAGE_DEBUG_MODE == true)
    log_info("Debug Mode Enabled");
}

////////////////////////////////////////////
void image_fxn1(void){
  image_internal_fxn1();
  log_info("Called function image_fxn1");
  if (IMAGE_DEBUG_MODE == true)
    log_info("Debug Mode Enabled");
}
////////////////////////////////////////////
#endif
