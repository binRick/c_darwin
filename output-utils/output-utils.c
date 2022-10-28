#pragma once
#ifndef OUTPUT_UTILS_C
#define OUTPUT_UTILS_C
////////////////////////////////////////////

////////////////////////////////////////////
#include "output-utils/output-utils.h"
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
static bool output_utils_DEBUG_MODE             = false;
const char  *output_modes[OUTPUT_MODES_QTY + 1] = {
  [OUTPUT_MODE_TEXT]  = "text",
  [OUTPUT_MODE_JSON]  = "json",
  [OUTPUT_MODE_TABLE] = "table",
  [OUTPUT_MODES_QTY]  = NULL,
};
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__output_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_output_utils") != NULL) {
    log_debug("Enabling output-utils Debug Mode");
    output_utils_DEBUG_MODE = true;
  }
}
static void output_utils_internal_fxn1(void);

////////////////////////////////////////////
static void output_utils_internal_fxn1(void){
  log_info("Called function output_utils_internal_fxn1");
  if (output_utils_DEBUG_MODE == true)
    log_info("Debug Mode Enabled");
}

////////////////////////////////////////////
void output_utils_fxn1(void){
  output_utils_internal_fxn1();
  log_info("Called function output_utils_fxn1");
  if (output_utils_DEBUG_MODE == true)
    log_info("Debug Mode Enabled");
}
////////////////////////////////////////////
#endif
