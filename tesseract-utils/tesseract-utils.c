#pragma once
#ifndef TESSERACT_UTILS_C
#define TESSERACT_UTILS_C
////////////////////////////////////////////
#include "tesseract-utils/tesseract-utils.h"
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
static bool TESSERACT_UTILS_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__tesseract_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_tesseract_utils") != NULL) {
    log_debug("Enabling tesseract-utils Debug Mode");
    TESSERACT_UTILS_DEBUG_MODE = true;
  }
}
static void tesseract_utils_internal_fxn1(void);

////////////////////////////////////////////
static void tesseract_utils_internal_fxn1(void){
  log_info("Called function tesseract_utils_internal_fxn1");
  if (TESSERACT_UTILS_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}

////////////////////////////////////////////
void tesseract_utils_fxn1(void){
  tesseract_utils_internal_fxn1();
  log_info("Called function tesseract_utils_fxn1");
  if (TESSERACT_UTILS_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}
////////////////////////////////////////////
#endif
