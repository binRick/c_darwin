#pragma once
#ifndef TESSERACT_UTILS_STATIC_C
#define TESSERACT_UTILS_STATIC_C
#include "tesseract/utils/utils.h"
////////////////////////////////////////////
static bool       TESSERACT_UTILS_DEBUG_MODE = false;
static const char *tess_lang                 = "eng";
///////////////////////////////////////////////////////////////////////
static void __attribute__((destructor)) __destructor__tesseract_utils(void){
}
static void __attribute__((constructor)) __constructor__tesseract_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_tesseract_utils") != NULL) {
    log_debug("Enabling tesseract-utils Debug Mode");
    TESSERACT_UTILS_DEBUG_MODE = true;
  }
}

#endif
