#pragma once
#ifndef __HTTPSERVER_H
#define __HTTPSERVER_H
//////////////////////////////////////
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
  bool   preview_mode, thumbnail_mode, quant_mode;
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
  char                   *url;
  char                   *path;
  int                    response_status;
  struct http_response_s *response;
  struct http_request_s  *request;
  char                   *response_body;
  struct   http_string_s *target;
  url_data_t             *parsed;
};
struct ud_t {
  pid_t  pid;
  size_t requests_qty;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
int httpserver_main();
#endif
