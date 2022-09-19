#pragma once
#ifndef LS_WIN_HTTPSERVER_C
#define LS_WIN_HTTPSERVER_C
#include <sys/types.h>
#include <sys/utsname.h>
///////////////////////////////////////////////
#include "c_fsio/include/fsio.h"
#include "c_img/src/img.h"
#include "c_stringfn/include/stringfn.h"
#include "httpserver.h/httpserver.h"
#include "image-utils/image-utils.h"
#include "log/log.h"
#include "parson/parson.h"
#include "querystring.c/querystring.h"
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
#include "submodules/b64.c/b64.h"
#include "timestamp/timestamp.h"
#include "url.h/url.h"
///////////////////////////////////////////////
#include "app-utils/app-utils.h"
#include "capture-utils/capture-utils.h"
#include "config-utils/config-utils.h"
#include "font-utils/font-utils.h"
#include "hotkey-utils/hotkey-utils.h"
#include "monitor-utils/monitor-utils.h"
#include "screen-utils/screen-utils.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
#include "window-utils/window-utils.h"
///////////////////////////////////////////////
//#include "darwin-ls/darwin-ls-httpserver-incbin.c"
//#include "darwin-ls/darwin-ls-httpserver.h"
#include "httpserver-utils/httpserver-utils.h"
#include "httpserver/httpserver.h"
///////////////////////////////////////////////
static bool DARWIN_LS_HTTPSERVER_DEBUG_MODE = false;

static void __attribute__((constructor)) __constructor__darwin_ls_httpserver(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_DARWIN_LS_HTTPSERVEr") != NULL) {
    log_debug("Enabling darwin-ls httpserver Debug Mode");
    DARWIN_LS_HTTPSERVER_DEBUG_MODE = true;
  }
}

#endif
