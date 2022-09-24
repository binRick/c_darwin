#pragma once
#ifndef TESSERACT_C
#define TESSERACT_C
////////////////////////////////////////////
#include "tesseract/tesseract.h"
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
static bool TESSERACT_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__tesseract(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_tesseract") != NULL) {
    log_debug("Enabling tesseract Debug Mode");
    TESSERACT_DEBUG_MODE = true;
  }
}
static void tesseract_internal_fxn1(void);

////////////////////////////////////////////
static void tesseract_internal_fxn1(void){
  log_info("Called function tesseract_internal_fxn1");
  if (TESSERACT_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}

////////////////////////////////////////////
void tesseract_fxn1(void){
  tesseract_internal_fxn1();
  log_info("Called function tesseract_fxn1");
  if (TESSERACT_DEBUG_MODE == true) {
    log_info("Debug Mode Enabled");
  }
}
////////////////////////////////////////////
#endif