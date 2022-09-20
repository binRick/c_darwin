#pragma once
#ifndef __HTTPSERVER_H
#define __HTTPSERVER_H
//////////////////////////////////////
#include "chan/src/chan.h"
#include "httpserver.h/httpserver.h"
#include "querystring.c/querystring.h"
#include "url.h/url.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define DARWIN_LS_HTTPSERVER_PORT        49225
#define DARWIN_LS_HTTPSERVER_HOST        "127.0.0.1"
#define DARWIN_LS_HTTPSERVER_PROTOCOL    "http"
//////////////////////////////////////
struct http_server_s *server;
struct parsed_data_t {
  size_t window_id, space_id, display_id, screen_id, pid;
  size_t resize_factor;
  bool   grayscale_conversion;
  bool   preview_mode, thumbnail_mode, quant_mode, ocr_mode;
  int    min_quant_qual, max_quant_qual;
  char   *type; int type_id;
  size_t id;
};
struct rq_t {
  bool                   ok;
  pid_t                  pid;
  unsigned long          dur, started;
  size_t                 size;
  struct parsed_data_t   *pd;
  char                   *query;
  char                   *method;
  char                   *parse_url;
  char                   *uri;
  char                   *path;
  int                    response_status;
  struct http_response_s *response;
  struct http_request_s  *request;
  char                   *response_body;
  struct   http_string_s *target;
  struct http_string_s   *url;
  url_data_t             *parsed;
};
struct websocket_config_t {
  int    port;
  char   *socket;
  chan_t *chan;
};
struct ud_t {
  pid_t  pid;
  size_t requests_qty;
};

enum httpserver_chan_type_id_t {
  HTTPSERVER_CHAN_REQUEST,
  HTTPSERVER_CHAN_RESPONSE,
  HTTPSERVER_CHAN_CAPTURE,
  HTTPSERVER_CHAN_OCR,
  HTTPSERVER_CHANS_QTY,
};
struct httpserver_chan_t {
  chan_t *chan;
  size_t buffer_qty;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
int httpserver_main();
#endif
