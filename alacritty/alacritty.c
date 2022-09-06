#pragma once
#ifndef ALACRITTY_C
#define ALACRITTY_C
////////////////////////////////////////////
///Applications/Alacritty.app/Contents/MacOS/alacritty msg config font.normal.family=InconsolataGo\ Nerd\ Font font.normal.style=Regular font.size=30
////////////////////////////////////////////
#include "alacritty/alacritty.h"
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
static bool ALACRITTY_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__alacritty(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_ALACRITTY") != NULL) {
    log_debug("Enabling alacritty Debug Mode");
    ALACRITTY_DEBUG_MODE = true;
  }
}
#endif
