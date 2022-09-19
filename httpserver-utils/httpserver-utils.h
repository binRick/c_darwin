#pragma once
#ifndef HTTPSERVER_UTILS_H
#define HTTPSERVER_UTILS_H
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
struct incbin_response_t {
  size_t size;
  char   *data; char *content_type; char *request_path;
};
struct parsed_data_t {
  size_t window_id, space_id, display_id, screen_id, pid;
  size_t resize_factor;
  bool   grayscale_conversion;
};
#define DARWIN_LS_HTTPSERVER_PORT          49225
#define DARWIN_LS_HTTPSERVER_HOST          "127.0.0.1"
#define DARWIN_LS_HTTPSERVER_PROTOCOL      "http"
#define UNHANDLED_REQUEST_RESPONSE_CODE    500
#define HANDLED_REQUEST_RESPONSE_CODE      200
#define DEFAULT_TEXT_RESPONSE              "404 Not Found"
#define DEFAULT_RESIZE_FACTOR              400
#undef HTTP_MAX_TOTAL_EST_MEM_USAGE
#undef HTTP_MAX_REQUEST_BUF_SIZE
#undef HTTP_REQUEST_TIMEOUT
#define HTTP_MAX_TOTAL_EST_MEM_USAGE    1024 * 1024 * 64
#define HTTP_MAX_REQUEST_BUF_SIZE       1024 * 1024
#define HTTP_REQUEST_TIMEOUT            3
#define CONTENT_TYPE_PNG                "image/png"
#define CONTENT_TYPE_TEXT               "text/plain"
#define CONTENT_TYPE_FAVICON            "image/x-icon"
#define CONTENT_TYPE_JS                 "application/javascript"
#define CONTENT_TYPE_JS_MAP             "application/json"
#define CONTENT_TYPE_JSON               "application/json"
#define CONTENT_TYPE_CSS                "text/css"
#define CONTENT_TYPE_TTF                "application/octet-stream"
#define CONTENT_TYPE_WOFF               "application/x-font-woff"
#define CONTENT_TYPE_HTML               "text/html; charset=UTF-8"
static const char *tess_lang = "eng";
struct http_request_s;
void handle_sigterm(int signum);
char * get_extracted_image_text(char *image_file);
void handle_request(struct http_request_s *request);
////////////////////////////////////////////////////////////
#endif
