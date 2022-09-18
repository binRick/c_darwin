#pragma once
#ifndef LS_WIN_HTTPSERVER_H
#define LS_WIN_HTTPSERVER_H
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////
struct incbin_response_t {
  size_t size;
  char   *data; char *content_type; char *request_path;
};
struct parsed_data_t {
  size_t window_id, space_id, display_id, screen_id, pid;
  size_t resize_factor;
  bool   grayscale_conversion;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
#define CONTENT_TYPE_PNG                               "image/png"
#define CONTENT_TYPE_TEXT                              "text/plain"
#define CONTENT_TYPE_FAVICON                           "image/x-icon"
#define CONTENT_TYPE_JS                                "application/javascript"
#define CONTENT_TYPE_JS_MAP                            "application/json"
#define CONTENT_TYPE_JSON                              "application/json"
#define CONTENT_TYPE_CSS                               "text/css"
#define CONTENT_TYPE_TTF                               "application/octet-stream"
#define CONTENT_TYPE_WOFF                              "application/x-font-woff"
#define CONTENT_TYPE_HTML                              "text/html; charset=UTF-8"
///////////////////////////////////////////////////////////////////////////////////////////////////////
#define INCLUDE_FAVICON_FILE                           "./.assets/images/favicon.ico"
#define INCLUDE_ERROR_IMAGE                            "./.assets/images/error.png"
///////////////////////////////////////////////////////////////////////////////////////////////////////
#define INCLUDE_REVEAL_MULTIPLE_PRESENTATIONS_HTML     "./.assets/reveal/index.html"
#define INCLUDE_REVEAL_SOURCE_SANS_PRO_CSS             "./.assets/reveal/fonts/source-sans-pro/source-sans-pro.css"
#define INCLUDE_REVEAL_SOURCE_SANS_PRO_REGULAR_TTF     "./.assets/reveal/fonts/source-sans-pro/source-sans-pro-regular.ttf"
#define INCLUDE_REVEAL_SOURCE_SANS_PRO_REGULAR_WOFF    "./.assets/reveal/fonts/source-sans-pro/source-sans-pro-regular.woff"
#define INCLUDE_REVEAL_SOURCE_SANS_PRO_ITALIC_WOFF     "./.assets/reveal/fonts/source-sans-pro/source-sans-pro-italic.woff"
#define INCLUDE_REVEAL_SOURCE_SANS_PRO_ITALIC_TTF      "./.assets/reveal/fonts/source-sans-pro/source-sans-pro-italic.ttf"
#define INCLUDE_REVEAL_DIST_REVEAL_JS                  "./.assets/reveal/dist/reveal.js"
#define INCLUDE_JQUERY_MIN_JS                          "./.assets/jquery/jquery.min.js"
#define INCLUDE_REVEAL_DIST_REVEAL_JS_MAP              "./.assets/reveal/dist/reveal.js.map"
#define INCLUDE_REVEAL_DIST_RESET_CSS                  "./.assets/reveal/dist/reset.css"
#define INCLUDE_REVEAL_DIST_REVEAL_CSS                 "./.assets/reveal/dist/reveal.css"
#define INCLUDE_REVEAL_DIST_THEME_WHITE_CSS            "./.assets/reveal/dist/theme/white.css"
#define INCLUDE_REVEAL_DIST_MONOKAI_CSS                "./.assets/reveal/plugin/highlight/monokai.css"
#define INCLUDE_REVEAL_DIST_HIGHLIGHT_JS               "./.assets/reveal/plugin/highlight/highlight.js"
#define INCLUDE_REVEAL_DIST_MARKDOWN_JS                "./.assets/reveal/plugin/markdown/markdown.js"
#define INCLUDE_REVEAL_DIST_MATH_JS                    "./.assets/reveal/plugin/math/math.js"
#define INCLUDE_MAIN_CSS                               "./.assets/main.css"
#define INCLUDE_MAIN_JS                                "./.assets/main.js"
///////////////////////////////////////////////////////////////////////////////////////////////////////
int httpserver_main();
#endif
