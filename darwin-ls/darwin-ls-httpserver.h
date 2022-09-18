#pragma once
#ifndef LS_WIN_HTTPSERVER_H
#define LS_WIN_HTTPSERVER_H
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <unistd.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////
struct incbin_response_t {
  size_t size;
  char   *data; char *content_type; char *request_path;
};
struct parsed_data_t {
  size_t window_id, space_id, display_id, screen_id, pid;
  bool   grayscale_conversion;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
#define CONTENT_TYPE_PNG                              "image/png"
#define CONTENT_TYPE_TEXT                             "text/plain"
#define CONTENT_TYPE_FAVICON                          "image/x-icon"
#define CONTENT_TYPE_JAVASCRIPT                       "application/javascript"
#define CONTENT_TYPE_JS_MAP                           "application/json"
#define CONTENT_TYPE_JSON                             "application/json"
#define CONTENT_TYPE_CSS                              "text/css"
#define CONTENT_TYPE_HTML                             "text/html; charset=UTF-8"
///////////////////////////////////////////////////////////////////////////////////////////////////////
#define INCLUDE_FAVICON_FILE                          "assets/images/favicon.ico"
#define INCLUDE_ERROR_IMAGE                           "assets/images/error.png"
#define INCLUDE_REVEAL_MULTIPLE_PRESENTATIONS_HTML    "assets/reveal/index.html"
#define INCLUDE_REVEAL_DIST_REVEAL_JS                 "assets/reveal/dist/reveal.js"
#define INCLUDE_REVEAL_DIST_REVEAL_JS_MAP             "assets/reveal/dist/reveal.js.map"
#define INCLUDE_REVEAL_DIST_RESET_CSS                 "assets/reveal/dist/reset.css"
#define INCLUDE_REVEAL_DIST_REVEAL_CSS                "assets/reveal/dist/reveal.css"
#define INCLUDE_REVEAL_DIST_THEME_WHITE_CSS           "assets/reveal/dist/theme/white.css"
#define INCLUDE_REVEAL_DIST_MONOKAI_CSS               "assets/reveal/plugin/highlight/monokai.css"
#define INCLUDE_REVEAL_DIST_HIGHLIGHT_JS              "assets/reveal/plugin/highlight/highlight.js"
#define INCLUDE_REVEAL_DIST_MARKDOWN_JS               "assets/reveal/plugin/markdown/markdown.js"
#define INCLUDE_REVEAL_DIST_MATH_JS                   "assets/reveal/plugin/math/math.js"
///////////////////////////////////////////////////////////////////////////////////////////////////////
int httpserver_main();
#endif
