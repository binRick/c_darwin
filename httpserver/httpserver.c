#pragma once
#ifndef __HTTPSERVER_C
#define __HTTPSERVER_C
////////////////////////////////////////////
#include "httpserver-utils/httpserver-utils.h"
#include "httpserver.h/httpserver.h"
#include "httpserver/httpserver.h"
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
static bool HTTPSERVER_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__httpserver(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_httpserver") != NULL) {
    log_debug("Enabling httpserver Debug Mode");
    HTTPSERVER_DEBUG_MODE = true;
  }
}

////////////////////////////////////////////
int httpserver_main() {
  signal(SIGTERM, handle_sigterm);
  server = http_server_init(DARWIN_LS_HTTPSERVER_PORT, handle_request);
  http_server_listen_addr(server, DARWIN_LS_HTTPSERVER_HOST);
}
////////////////////////////////////////////
#endif
