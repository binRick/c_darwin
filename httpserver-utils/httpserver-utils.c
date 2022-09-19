#pragma once
#ifndef HTTPSERVER_UTILS_C
#define HTTPSERVER_UTILS_C
#define HTTPSERVER_IMPL
#include "httpserver-utils/httpserver-utils.h"
#include "httpserver.h/httpserver.h"
#include "httpserver/httpserver.h"
#define INCBIN_SILENCE_BITCODE_WARNING
#include "allheaders.h"
#include "bytes/bytes.h"
#include "capture-utils/capture-utils.h"
#include "config-utils/config-utils.h"
#include "display-utils/display-utils.h"
#include "hotkey-utils/hotkey-utils.h"
#include "incbin/incbin.h"
#include "ms/ms.h"
#include "querystring.c/querystring.h"
#include "querystring.c/querystring.h"
#include "screen-utils/screen-utils.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
#include "tempdir.c/tempdir.h"
#include "tesseract/capi.h"
#include "timestamp/timestamp.h"
#include "url.h/url.h"
/////////////////////////////////////////////////////////
#include "c_fsio/include/fsio.h"
#include "c_img/src/img.h"
#include "c_stringfn/include/stringfn.h"
#include "image-utils/image-utils.h"
#include "log/log.h"
#include "parson/parson.h"
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
#include "submodules/b64.c/b64.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
static struct parsed_data_t *parse_request(const struct http_request_s *request);
static int iterate_incbin_responses(struct http_request_s *request, struct http_response_s *response);
static int request_target_is(const struct http_request_s *request, const char *target);
static void request_params_parser(void *data, char *fst, char *snd);
static bool HTTPSERVER_UTILS_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////////////////////////////////////
#define INCLUDE_FAVICON_FILE                           "./.assets/images/favicon.ico"
#define INCLUDE_ERROR_IMAGE                            "./.assets/images/error.png"
///////////////////////////////////////////////////////////////////////////////////////////////////////
#define INCLUDE_SOCKETIO_JS                            "./.assets/socket.io/socket.io.min.js"
#define INCLUDE_SOCKETIO_JS_MAP                        "./.assets/socket.io/socket.io.min.js.map"
#define INCLUDE_CONTEXT_CSS                            "./.assets/context/jquery.contextMenu.min.css"
#define INCLUDE_CONTEXT_CSS_MAP                        "./.assets/context/jquery.contextMenu.min.css.map"
#define INCLUDE_CONTEXT_JS                             "./.assets/context/jquery.contextMenu.min.js"
#define INCLUDE_CONTEXT_JS_MAP                         "./.assets/context/jquery.contextMenu.min.js.map"
#define INCLUDE_CONTEXT_FONT_ICONS                     "./.assets/context/font/context-menu-icons.woff2"
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
#define INCLUDE_REVEAL_DIST_THEME_SOLARIZED_CSS        "./.assets/reveal/dist/theme/solarized.css"
#define INCLUDE_REVEAL_DIST_MONOKAI_CSS                "./.assets/reveal/plugin/highlight/monokai.css"
#define INCLUDE_REVEAL_DIST_HIGHLIGHT_JS               "./.assets/reveal/plugin/highlight/highlight.js"
#define INCLUDE_REVEAL_DIST_MARKDOWN_JS                "./.assets/reveal/plugin/markdown/markdown.js"
#define INCLUDE_REVEAL_DIST_MATH_JS                    "./.assets/reveal/plugin/math/math.js"
#define INCLUDE_MAIN_CSS                               "./.assets/main.css"
#define INCLUDE_MAIN_JS                                "./.assets/main.js"
#define RETURN_ERROR_PNG(CAPTURE_TYPE, CAPTURE_TYPE_ID)                          \
  http_response_status(response, UNHANDLED_REQUEST_RESPONSE_CODE);               \
  log_error("Failed to capture %s #%lu", CAPTURE_TYPE, (size_t)CAPTURE_TYPE_ID); \
  http_response_header(response, "Content-Type", CONTENT_TYPE_PNG);              \
  http_response_body(response, gerrorpngData, gerrorpngSize);
#define ADD_REQUEST_HANDLER(URI, TYPE, INC)                                \
  if (request_target_is(request, URI)) {                                   \
    http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);         \
    http_response_header(response, "Content-Type", CONTENT_TYPE_ ## TYPE); \
    http_response_body(response, g ## INC ## Data, g ## INC ## Size);      \
    http_respond(request, response);                                       \
    return(EXIT_SUCCESS);                                                  \
  }
INCBIN(favicon, INCLUDE_FAVICON_FILE);
INCBIN(errorpng, INCLUDE_ERROR_IMAGE);
INCBIN(main_css, INCLUDE_MAIN_CSS);
INCBIN(main_js, INCLUDE_MAIN_JS);
INCBIN(jquery_min_js, INCLUDE_JQUERY_MIN_JS);
INCBIN(reveal_js, INCLUDE_REVEAL_DIST_REVEAL_JS);
INCBIN(reveal_js_map, INCLUDE_REVEAL_DIST_REVEAL_JS_MAP);
INCBIN(reset_css, INCLUDE_REVEAL_DIST_RESET_CSS);
INCBIN(reveal_css, INCLUDE_REVEAL_DIST_REVEAL_CSS);
INCBIN(reveal_theme_white_css, INCLUDE_REVEAL_DIST_THEME_WHITE_CSS);
INCBIN(reveal_monokai_css, INCLUDE_REVEAL_DIST_MONOKAI_CSS);
INCBIN(reveal_highlight_js, INCLUDE_REVEAL_DIST_HIGHLIGHT_JS);
INCBIN(reveal_markdown_js, INCLUDE_REVEAL_DIST_MARKDOWN_JS);
INCBIN(reveal_math_js, INCLUDE_REVEAL_DIST_MATH_JS);
INCBIN(reveal_multiple_presentations_html, INCLUDE_REVEAL_MULTIPLE_PRESENTATIONS_HTML);
INCBIN(reveal_reveal_source_space_pro_css, INCLUDE_REVEAL_SOURCE_SANS_PRO_CSS);
INCBIN(reveal_reveal_source_space_pro_regular_ttf, INCLUDE_REVEAL_SOURCE_SANS_PRO_REGULAR_TTF);
INCBIN(reveal_reveal_source_space_pro_regular_woff, INCLUDE_REVEAL_SOURCE_SANS_PRO_REGULAR_WOFF);
INCBIN(reveal_reveal_source_space_pro_italic_ttf, INCLUDE_REVEAL_SOURCE_SANS_PRO_ITALIC_TTF);
INCBIN(reveal_reveal_source_space_pro_italic_woff, INCLUDE_REVEAL_SOURCE_SANS_PRO_ITALIC_WOFF);
INCBIN(context_js_map, INCLUDE_CONTEXT_JS_MAP);
INCBIN(context_js, INCLUDE_CONTEXT_JS);
INCBIN(context_css, INCLUDE_CONTEXT_CSS);
INCBIN(context_css_map, INCLUDE_CONTEXT_CSS_MAP);
INCBIN(context_font_icons, INCLUDE_CONTEXT_FONT_ICONS);
INCBIN(socketio_js, INCLUDE_SOCKETIO_JS);
INCBIN(socketio_js_map, INCLUDE_SOCKETIO_JS_MAP);
/////////////////////////////////////////////////////////

//    http_response_header(response, "Cache-Control", "max-age:3600, immutable");
static int iterate_incbin_responses(struct http_request_s *request, struct http_response_s *response){
  ADD_REQUEST_HANDLER("/dist/socket.io.js", JS, socketio_js)
  ADD_REQUEST_HANDLER("/dist/socket.io.js.map", JS_MAP, socketio_js_map)
  ADD_REQUEST_HANDLER("/dist/font/context-menu-icons.woff2", WOFF, context_font_icons)
  ADD_REQUEST_HANDLER("/dist/jquery.contextMenu.min.js", JS_MAP, context_js)
  ADD_REQUEST_HANDLER("/dist/jquery.contextMenu.min.js.map", JS_MAP, context_js_map)
  ADD_REQUEST_HANDLER("/dist/jquery.contextMenu.min.css", CSS, context_css)
  ADD_REQUEST_HANDLER("/dist/jquery.contextMenu.min.css.map", JS_MAP, context_css_map)
  ADD_REQUEST_HANDLER("/dist/reveal.js", JS, reveal_js)
  ADD_REQUEST_HANDLER("/dist/reveal.js.map", JS_MAP, reveal_js_map)
  ADD_REQUEST_HANDLER("/reveal.html", HTML, reveal_multiple_presentations_html)
  ADD_REQUEST_HANDLER("/main.js", JS, main_js)
  ADD_REQUEST_HANDLER("/favicon.ico", FAVICON, favicon)
  ADD_REQUEST_HANDLER("/main.css", CSS, main_css)
  ADD_REQUEST_HANDLER("/dist/main.css", CSS, reset_css)
  ADD_REQUEST_HANDLER("/dist/reveal.css", CSS, reveal_css)
  ADD_REQUEST_HANDLER("/dist/jquery.min.js", JS, jquery_min_js)
  ADD_REQUEST_HANDLER("/dist/theme/white.css", CSS, reveal_theme_white_css)
  ADD_REQUEST_HANDLER("/plugin/markdown/markdown.js", JS, reveal_markdown_js)
  ADD_REQUEST_HANDLER("/plugin/highlight/highlight.js", JS, reveal_highlight_js)
  ADD_REQUEST_HANDLER("/plugin/highlight/monokai.css", CSS, reveal_monokai_css)
  ADD_REQUEST_HANDLER("/dist/theme/fonts/source-sans-pro/source-sans-pro.css", CSS, reveal_reveal_source_space_pro_css)
  ADD_REQUEST_HANDLER("/dist/theme/fonts/source-sans-pro/source-sans-pro-regular.ttf", TTF, reveal_reveal_source_space_pro_regular_ttf)
  ADD_REQUEST_HANDLER("/dist/theme/fonts/source-sans-pro/source-sans-pro-regular.woff", WOFF, reveal_reveal_source_space_pro_regular_woff)
  ADD_REQUEST_HANDLER("/dist/theme/fonts/source-sans-pro/source-sans-pro-italic.ttf", TTF, reveal_reveal_source_space_pro_italic_ttf)
  ADD_REQUEST_HANDLER("/dist/theme/fonts/source-sans-pro/source-sans-pro-italic.woff", WOFF, reveal_reveal_source_space_pro_italic_woff)
  return(EXIT_FAILURE);
}   /* iterate_incbin_responses */
#undef ADD_REQUEST_HANDLER

static void request_params_parser(void *data, char *fst, char *snd) {
  struct parsed_data_t *parsed_data = (struct parsed_data_t *)data;

  if (strcmp(fst, "window_id") == 0) {
    parsed_data->window_id = string_size_to_size_t(snd);
  }else if (strcmp(fst, "pid") == 0) {
    parsed_data->pid = string_size_to_size_t(snd);
  }else if (strcmp(fst, "screen_id") == 0) {
    parsed_data->screen_id = string_size_to_size_t(snd);
  }else if (strcmp(fst, "display_id") == 0) {
    parsed_data->display_id = string_size_to_size_t(snd);
  }else if (strcmp(fst, "resize_factor") == 0) {
    parsed_data->resize_factor = string_size_to_size_t(snd);
  }else if (strcmp(fst, "grayscale") == 0) {
    parsed_data->grayscale_conversion = (snd && strlen(snd) > 0 && strcmp(snd, "1") == 0) ? true : false;
  }else if (strcmp(fst, "space_id") == 0) {
    parsed_data->space_id = string_size_to_size_t(snd);
  }
}
static struct parsed_data_t *parse_request(const struct http_request_s *request){
  http_string_t url = http_request_target(request);
  http_string_t _method = http_request_method(request);
  char          *method = stringfn_substring(_method.buf, 0, _method.len), *purl;

  if (HTTPSERVER_UTILS_DEBUG_MODE) {
    log_info("method:%s", method);
  }

  asprintf(&purl, "%s://%s:%d/%s", DARWIN_LS_HTTPSERVER_PROTOCOL, DARWIN_LS_HTTPSERVER_HOST, DARWIN_LS_HTTPSERVER_PORT, url.buf);
  url_data_t           *parsed = url_parse(purl);
  char                 *query  = strdup(parsed->query);
  struct parsed_data_t *data   = calloc(1, sizeof(struct parsed_data_t));

  data->resize_factor        = DEFAULT_RESIZE_FACTOR;
  data->grayscale_conversion = false;
  data->window_id            = 0;
  data->space_id             = 0;
  data->display_id           = 0;
  data->screen_id            = 0;
  data->pid                  = 0;
  parse_querystring(query, (void *)data, request_params_parser);
  return(data);
}

static int request_target_is(const struct http_request_s *request, const char *target) {
  http_string_t url;
  int           res = EXIT_FAILURE;

  if (request && target) {
    url = http_request_target(request);
    if (url.buf && url.len > 0) {
      char *purl = NULL, *url_buf = stringfn_substring(url.buf, 1, url.len - 1), *pathname;
      if (url_buf) {
        asprintf(&purl, "%s://%s:%d/%s", DARWIN_LS_HTTPSERVER_PROTOCOL, DARWIN_LS_HTTPSERVER_HOST, DARWIN_LS_HTTPSERVER_PORT, url_buf);
        if (purl) {
          url_data_t *parsed = url_parse(purl);
          pathname = strdup(parsed->pathname);
          if (pathname) {
            res = (url.buf && target && pathname && (memcmp(pathname, target, strlen(pathname))) == 0);
            if (res == EXIT_SUCCESS && HTTPSERVER_UTILS_DEBUG_MODE) {
              log_debug("%s", url_buf);
              log_debug("%s", purl);
              if (parsed) {
                url_data_inspect(parsed);
              }
            }
          }
          if (parsed) {
            url_free(parsed);
          }
          if (pathname) {
            free(pathname);
          }
          if (purl) {
            free(purl);
          }
          if (url_buf) {
            free(url_buf);
          }
        }
      }
    }
  }
  return(res);
}   /* request_target_is */

void handle_sigterm(int signum) {
  (void)signum;
  if (server) {
    free(server);
  }
  exit(EXIT_SUCCESS);
}

void handle_request(struct http_request_s *request) {
  char *response_body;

  asprintf(&response_body, "{\"response_code\":%d,\"response_data\":null}", UNHANDLED_REQUEST_RESPONSE_CODE);

  http_request_connection(request, HTTP_AUTOMATIC);
  struct http_response_s *response = http_response_init();

  http_response_status(response, UNHANDLED_REQUEST_RESPONSE_CODE);
  if (iterate_incbin_responses(request, response) == EXIT_SUCCESS) {
    return;
  }else if (request_target_is(request, "/enable_configured_key")) {
    http_string_t body = http_request_body(request);
    if (body.len > 1) {
      JSON_Value              *schema   = json_parse_string(body.buf);
      char                    *key      = json_object_get_string(json_object(schema), "key");
      char                    *cfg_path = get_homedir_yaml_config_file_path();
      struct hotkeys_config_t *cfg      = load_yaml_config_file_path(cfg_path);
      struct key_t            *k        = get_hotkey_config_key(cfg, key);
      if (key && strlen(key) > 0 && k) {
        log_info("enabling key %s", key);
        bool ok = enable_hotkey_config_key(k->key);
        cfg = load_yaml_config_file_path(cfg_path);
        k   = get_hotkey_config_key(cfg, key);
        ok  = (ok == true && k->enabled == true) ? true : false;
        asprintf(&response_body,
                 "{\"response_code\":%d,\"response_data\":\"%s\",\"ts\":%lu"
                 "}"
                 "%s",
                 (ok == true) ? HANDLED_REQUEST_RESPONSE_CODE : UNHANDLED_REQUEST_RESPONSE_CODE, (ok == true) ? "key_enabled" : "failed", (size_t)timestamp(),
                 ""
                 );
      }
    }
    http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);
    http_response_header(response, "Content-Type", CONTENT_TYPE_JSON);
    http_response_body(response, response_body, strlen(response_body));
  } else if (request_target_is(request, "/capture/display")) {
    struct parsed_data_t *data      = parse_request(request);
    size_t               display_id = capture_type_validate_id_or_get_default_id(CAPTURE_TYPE_DISPLAY, data->display_id);
    char                 *output_file;
    asprintf(&output_file, "%sdisplay-%lu-%lld.png", gettempdir(), display_id, timestamp());
    CGImageRef           img_ref     = capture_type_capture(CAPTURE_TYPE_DISPLAY, display_id);
    size_t               rgb_len     = 0;
    unsigned char        *rgb_pixels = cgimage_ref_to_rgb_pixels(img_ref, &rgb_len);
    log_debug("Got %lu rgb pixels", rgb_len);
    fsio_write_binary_file("/tmp/a.rgb", rgb_pixels, rgb_len);

    save_cgref_to_qoi_file(img_ref, "/tmp/a.qoi");

    size_t        qoi_len     = 0;
    unsigned char *qoi_pixels = cgref_to_qoi_memory(img_ref, &qoi_len);
    log_debug("Got %lu qoi pixels", qoi_len);

    int qoi_width = 0, qoi_height = 0;
    image_types[IMAGE_TYPE_QOI].get_dimensions_from_buffer(qoi_pixels, qoi_len, &qoi_width, &qoi_height);
    log_debug("Got qoi %dx%d", qoi_width, qoi_height);

    size_t len = 0; int width = 0, height = 0; bool ok = false; unsigned char *pixels = NULL; unsigned char *header = NULL; char *file = NULL;
    int    compression = 0;

    {
      if (pixels) {
        free(pixels);
      }
      if (header) {
        free(header);
      }
      len    = 0; width = 0; height = 0; ok = false; pixels = NULL; header = NULL;
      file   = "/tmp/a.png";
      pixels = save_cgref_to_png_memory(img_ref, &len);
      fsio_write_binary_file(file, pixels, len);
      log_debug("PNG OK: %s %s", file, bytes_to_string(fsio_file_size(file)));
      header = image_types[IMAGE_TYPE_PNG].read_file_header(file);
      image_types[IMAGE_TYPE_PNG].get_dimensions_from_header(header, &width, &height);
      log_debug("PNG Dimensions: %dx%d", width, height);
    }

    {
      if (pixels) {
        free(pixels);
      }
      if (header) {
        free(header);
      }
      len    = 0; width = 0; height = 0; ok = false; pixels = NULL; header = NULL;
      file   = "/tmp/a.gif";
      pixels = save_cgref_to_gif_memory(img_ref, &len);
      fsio_write_binary_file(file, pixels, len);
      log_debug("GIF OK: %s %s", file, bytes_to_string(fsio_file_size(file)));
      header = image_types[IMAGE_TYPE_GIF].read_file_header(file);
      image_types[IMAGE_TYPE_GIF].get_dimensions_from_header(header, &width, &height);
      log_debug("GIF Dimensions: %dx%d", width, height);
    }

    {
      if (pixels) {
        free(pixels);
      }
      if (header) {
        free(header);
      }
      len  = 0; width = 0; height = 0; ok = false; pixels = NULL; header = NULL;
      file = "/tmp/a.tiff";
      save_cgref_to_tiff_file(img_ref, "/tmp/a1.tiff");
      pixels = save_cgref_to_tiff_memory(img_ref, &len);
      fsio_write_binary_file(file, pixels, len);
      image_types[IMAGE_TYPE_TIFF].get_dimensions_from_buffer(pixels, len, &width, &height);
      log_debug("TIFF Dimensions: %dx%d", width, height);
      stbi_load_from_memory(pixels, len, &width, &height, &compression, 0);
      log_debug("%d,%d,%d", width, height, compression);
    }
    {
      if (pixels) {
        free(pixels);
      }
      if (header) {
        free(header);
      }
      len    = 0; width = 0; height = 0; ok = false; pixels = NULL; header = NULL;
      file   = "/tmp/a.bmp";
      pixels = save_cgref_to_bmp_memory(img_ref, &len);
      fsio_write_binary_file(file, pixels, len);
      image_types[IMAGE_TYPE_BMP].get_dimensions_from_buffer(pixels, len, &width, &height);
      log_debug("BMP Dimensions: %dx%d", width, height);
    }

    {
      if (pixels) {
        free(pixels);
      }
      if (header) {
        free(header);
      }
      len    = 0; width = 0; height = 0; ok = false; pixels = NULL; header = NULL;
      file   = "/tmp/a.jpeg";
      pixels = save_cgref_to_jpeg_memory(img_ref, &len);
      fsio_write_binary_file(file, pixels, len);
      image_types[IMAGE_TYPE_JPEG].get_dimensions_from_buffer(pixels, len, &width, &height);
      log_debug("JPEG Dimensions: %dx%d", width, height);
    }

    if (save_cgref_to_png_file(img_ref, output_file) == true) {
      size_t        png_len   = fsio_file_size(output_file);
      unsigned char *png_data = fsio_read_binary_file(output_file);
      fsio_remove(output_file);
      http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);
      http_response_header(response, "Content-Type", CONTENT_TYPE_PNG);
      http_response_body(response, png_data, png_len);
    }else{
      RETURN_ERROR_PNG("Display", display_id);
    }
  } else if (request_target_is(request, "/capture/space")) {
    struct parsed_data_t *data    = parse_request(request);
    size_t               space_id = capture_type_validate_id_or_get_default_id(CAPTURE_TYPE_SPACE, data->space_id);
    char                 *output_file;
    asprintf(&output_file, "%sspace-%lu-%lld.png", gettempdir(), space_id, timestamp());
    CGImageRef           img_ref = capture_type_capture(CAPTURE_TYPE_SPACE, space_id);
    if (save_cgref_to_png_file(img_ref, output_file) == true) {
      size_t        png_len   = fsio_file_size(output_file);
      unsigned char *png_data = fsio_read_binary_file(output_file);
      fsio_remove(output_file);
      http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);
      http_response_header(response, "Content-Type", CONTENT_TYPE_PNG);
      http_response_body(response, png_data, png_len);
    }else{
      RETURN_ERROR_PNG("Space", space_id);
    }
  } else if (request_target_is(request, "/extract/display")) {
    struct parsed_data_t *data      = parse_request(request);
    size_t               display_id = (data->display_id > 0) ? (size_t)(data->display_id) : (size_t)(get_main_display_id());
    char                 *output_file;
    asprintf(&output_file, "%sdisplay-%lu-%lld.png", gettempdir(), display_id, timestamp());
    CGImageRef           img_ref = capture_display_id(display_id);
    if (save_cgref_to_png_file(img_ref, output_file) == true && fsio_file_exists(output_file) && fsio_file_size(output_file) > 4096) {
      if (data->grayscale_conversion == true) {
        if (HTTPSERVER_UTILS_DEBUG_MODE) {
          log_info("Converting to grayscale");
        }
        output_file = convert_png_to_grayscale(output_file, data->resize_factor);
      }else{
        if (HTTPSERVER_UTILS_DEBUG_MODE) {
          log_info("Not converting to grayscale");
        }
      }
      char *extracted_text = get_extracted_image_text(output_file);
      http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);
      http_response_header(response, "Content-Type", CONTENT_TYPE_TEXT);
      http_response_body(response, extracted_text, strlen(extracted_text));
      http_respond(request, response);
      TessDeleteText(extracted_text);
      return;
    }else{
      RETURN_ERROR_PNG("Display", display_id);
    }
  } else if (request_target_is(request, "/extract/space")) {
    struct parsed_data_t *data = parse_request(request);
    size_t               space_id = (data && data->space_id && data->space_id > 0) ? (size_t)(data->space_id) : (size_t)(get_space_id());
    char                 *output_file = NULL, *extracted_text = NULL;
    asprintf(&output_file, "%sspace-%lu-%lld.png", gettempdir(), space_id, timestamp());
    CGImageRef           img_ref = capture_space_id((uint32_t)space_id);
    if (save_cgref_to_png_file(img_ref, output_file) == true && fsio_file_exists(output_file) && fsio_file_size(output_file) > 4096) {
      if (data->grayscale_conversion == true) {
        output_file = convert_png_to_grayscale(output_file, data->resize_factor);
      }
      extracted_text = get_extracted_image_text(output_file);
      fsio_remove(output_file);
      http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);
      http_response_header(response, "Content-Type", CONTENT_TYPE_TEXT);
      http_response_body(response, extracted_text, strlen(extracted_text));
      http_respond(request, response);
      TessDeleteText(extracted_text);
      return;
    }else{
      RETURN_ERROR_PNG("Space", space_id);
    }
  } else if (request_target_is(request, "/extract/window")) {
    struct parsed_data_t *data = parse_request(request);
    size_t               window_id = capture_type_validate_id_or_get_default_id(CAPTURE_TYPE_WINDOW, data->window_id);
    char                 *output_file = NULL, *extracted_text = NULL;
    asprintf(&output_file, "%swindow-%lu-%lld.png", gettempdir(), window_id, timestamp());
    CGImageRef           img_ref = capture_type_capture(CAPTURE_TYPE_WINDOW, window_id);
    if (save_cgref_to_png_file(img_ref, output_file) == true && fsio_file_exists(output_file) && fsio_file_size(output_file) > 4096) {
      if (data->grayscale_conversion == true) {
        if (HTTPSERVER_UTILS_DEBUG_MODE) {
          log_info("Converting to grayscale");
        }
        output_file = convert_png_to_grayscale(output_file, data->resize_factor);
      }else{
        if (HTTPSERVER_UTILS_DEBUG_MODE) {
          log_info("Not converting to grayscale");
        }
      }
      extracted_text = get_extracted_image_text(output_file);
      fsio_remove(output_file);
      http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);
      http_response_header(response, "Content-Type", CONTENT_TYPE_TEXT);
      http_response_body(response, extracted_text, strlen(extracted_text));
      http_respond(request, response);
      TessDeleteText(extracted_text);
      return;
    }else{
      RETURN_ERROR_PNG("Window", window_id);
    }
  } else if (request_target_is(request, "/capture/window")) {
    struct parsed_data_t *data        = parse_request(request);
    size_t               window_id    = capture_type_validate_id_or_get_default_id(CAPTURE_TYPE_WINDOW, data->window_id);
    char                 *output_file = NULL;
    unsigned char        *png_data    = NULL;
    size_t               png_len      = 0;
    asprintf(&output_file, "%swindow-%lu-%lld.png", gettempdir(), window_id, timestamp());
    CGImageRef           img_ref = capture_type_capture(CAPTURE_TYPE_WINDOW, window_id);
    if (save_cgref_to_png_file(img_ref, output_file) == true) {
      png_len  = fsio_file_size(output_file);
      png_data = fsio_read_binary_file(output_file);
      fsio_remove(output_file);
      http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);
      http_response_header(response, "Content-Type", CONTENT_TYPE_PNG);
      http_response_body(response, png_data, png_len);
    }else{
      RETURN_ERROR_PNG("Window", window_id);
    }
  } else if (request_target_is(request, "/disable_configured_key")) {
    http_string_t body = http_request_body(request);
    if (body.len > 1) {
      bool                    ok;
      JSON_Value              *schema   = json_parse_string(body.buf);
      char                    *key      = json_object_get_string(json_object(schema), "key");
      char                    *cfg_path = get_homedir_yaml_config_file_path();
      struct hotkeys_config_t *cfg      = load_yaml_config_file_path(cfg_path);
      struct key_t            *k        = get_hotkey_config_key(cfg, key);
      if (key && strlen(key) > 0 && k) {
        log_info("disabling key %s", key);
        ok  = disable_hotkey_config_key(k->key);
        cfg = load_yaml_config_file_path(cfg_path);
        k   = get_hotkey_config_key(cfg, key);
        ok  = (ok == true && k->enabled == false) ? true : false;
        asprintf(&response_body,
                 "{\"response_code\":%d,\"response_data\":\"%s\",\"ts\":%lu"
                 "}"
                 "%s",
                 (ok == true) ? HANDLED_REQUEST_RESPONSE_CODE : UNHANDLED_REQUEST_RESPONSE_CODE, (ok == true) ? "key_disabled" : "failed", (size_t)timestamp(),
                 ""
                 );
      }
    }
    http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);
    http_response_header(response, "Content-Type", CONTENT_TYPE_JSON);
    http_response_body(response, response_body, strlen(response_body));
  } else if (request_target_is(request, "/config")) {
    char *response_body = get_config_response_body();
    http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);
    http_response_header(response, "Content-Type", CONTENT_TYPE_JSON);
    http_response_body(response, response_body, strlen(response_body));
    http_respond(request, response);
    free(response_body);
    return;
  }
  http_respond(request, response);
} /* handle_request */

char * get_extracted_image_text(char *image_file){
  unsigned long             extract_started = timestamp(), started;
  char                      *tess_ver = TessVersion(), *extracted_text = NULL;
  struct Pix                *img, *loaded_img;
  struct image_dimensions_t *png_size;

  if (HTTPSERVER_UTILS_DEBUG_MODE) {
    log_debug("Initializing Tesseract Version %s", tess_ver);
  }

  TessBaseAPI *api = TessBaseAPICreate();

  assert(api != NULL);
  assert(TessBaseAPIInit3(api, NULL, tess_lang) == EXIT_SUCCESS);
  started = timestamp();

  if (HTTPSERVER_UTILS_DEBUG_MODE) {
    log_debug("Reading file %s", image_file);
  }
  img = pixRead(image_file);

  assert(img != NULL);
  if (HTTPSERVER_UTILS_DEBUG_MODE) {
    log_info("Read %s Image %s in %s",
             bytes_to_string(fsio_file_size(image_file)), image_file, milliseconds_to_string(timestamp() - started)
             );
  }
  started = timestamp();
  TessBaseAPISetImage2(api, img);
  loaded_img = TessBaseAPIGetInputImage(api);

  assert(loaded_img != NULL);

  if (HTTPSERVER_UTILS_DEBUG_MODE) {
    log_info("Loaded %s Image %s in %s",
             bytes_to_string(fsio_file_size(image_file)), image_file, milliseconds_to_string(timestamp() - started)
             );
  }
  pixDestroy(&img);
  started = timestamp();
  assert(TessBaseAPIRecognize(api, NULL) == EXIT_SUCCESS);
  if (HTTPSERVER_UTILS_DEBUG_MODE) {
    log_debug("API Recognized in %s", milliseconds_to_string(timestamp() - started));
  }
  extracted_text = TessBaseAPIGetUTF8Text(api);
  png_size       = get_png_dimensions(image_file);

  log_debug("[TESSERACT] Extracted %s of Text from %s %dx%d %s in %s",
            bytes_to_string(strlen(extracted_text)),
            bytes_to_string(strlen(image_file)),
            png_size->width, png_size->height,
            image_file,
            milliseconds_to_string(timestamp() - extract_started)
            );
  fsio_remove(image_file);
  assert(extracted_text != NULL);
  return(extracted_text);
} /* get_extracted_image_text */
static void __attribute__((constructor)) __constructor__darwin_ls_httpserver(void){
  if (getenv("DEBUG") != NULL || getenv("HTTPSERVER_UTILS_DEBUG_MODE") != NULL) {
    log_debug("Enabling httpserver utils Debug Mode");
    HTTPSERVER_UTILS_DEBUG_MODE = true;
  }
}
/////////////////////////////////////////////////////////
#endif
