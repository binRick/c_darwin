#pragma once
#ifndef LS_WIN_HTTPSERVER_C
#define LS_WIN_HTTPSERVER_C
#define HTTPSERVER_IMPL
#include "allheaders.h"
#include "httpserver.h/httpserver.h"
#include <sys/types.h>
#include <sys/utsname.h>
///////////////////////////////////////////////
#include "c_fsio/include/fsio.h"
#include "c_img/src/img.h"
#include "c_stringfn/include/stringfn.h"
#include "capi.h"
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
#include "font-utils/font-utils.h"
#include "hotkey-utils/hotkey-utils.h"
#include "monitor-utils/monitor-utils.h"
#include "screen-utils/screen-utils.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
#include "window-utils/window-utils.h"
///////////////////////////////////////////////
#include "darwin-ls/darwin-ls-httpserver-incbin.c"
#include "darwin-ls/darwin-ls-httpserver-vars.c"
#include "darwin-ls/darwin-ls-httpserver.h"
///////////////////////////////////////////////
static bool DARWIN_LS_HTTPSERVER_DEBUG_MODE = false;
static void handle_sigterm(int signum);
static int request_target_is(const struct http_request_s *request, const char *target);
static char * convert_png_to_grayscale(char *png_file, size_t resize_factor);
static void request_params_parser(void *data, char *fst, char *snd);
static struct parsed_data_t *parse_request(const struct http_request_s *request);
static struct http_server_s *server;
static char * get_extracted_image_text(char *image_file);
static const char           *tess_lang = "eng";
#define RETURN_ERROR_PNG(CAPTURE_TYPE, CAPTURE_TYPE_ID)                          \
  http_response_status(response, UNHANDLED_REQUEST_RESPONSE_CODE);               \
  log_error("Failed to capture %s #%lu", CAPTURE_TYPE, (size_t)CAPTURE_TYPE_ID); \
  http_response_header(response, "Content-Type", CONTENT_TYPE_PNG);              \
  http_response_body(response, inc_errorpng_data, inc_errorpng_size);
#define ADD_REQUEST_HANDLER(URI, TYPE, INC)                                   \
  if (request_target_is(request, URI)) {                                      \
    http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);            \
    http_response_header(response, "Content-Type", CONTENT_TYPE_ ## TYPE);    \
    http_response_body(response, inc_ ## INC ## _data, inc_ ## INC ## _size); \
    http_respond(request, response);                                          \
    return(EXIT_SUCCESS);                                                     \
  }

//    http_response_header(response, "Cache-Control", "max-age:3600, immutable");
static int iterate_incbin_responses(struct http_request_s *request, struct http_response_s *response){
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
} /* iterate_incbin_responses */
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

static char * convert_png_to_grayscale(char *png_file, size_t resize_factor){
  char *grayscale_file;

  asprintf(&grayscale_file, "%s-grayscale-%lld-grayscale.tif", gettempdir(), timestamp());
  if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
    log_info("Converting %s to %s resized by %lu%%", png_file, grayscale_file, resize_factor);
  }
  FILE       *input_png_file = fopen(png_file, "rb");
  CGImageRef png_gs          = png_file_to_grayscale_cgimage_ref_resized(input_png_file, resize_factor);

  assert(write_cgimage_ref_to_tif_file_path(png_gs, grayscale_file) == true);
  if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
    log_info("Converted to %s grayscale from %s PNG", bytes_to_string(fsio_file_size(grayscale_file)), bytes_to_string(fsio_file_size(png_file)));
  }
  fsio_remove(png_file);
  return(grayscale_file);
}

static char * get_extracted_image_text(char *image_file){
  unsigned long             extract_started = timestamp(), started;
  char                      *tess_ver = TessVersion(), *extracted_text = NULL;
  struct Pix                *img, *loaded_img;
  struct image_dimensions_t *png_size;

  if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
    log_debug("Initializing Tesseract Version %s", tess_ver);
  }

  TessBaseAPI *api = TessBaseAPICreate();

  assert(api != NULL);
  assert(TessBaseAPIInit3(api, NULL, tess_lang) == EXIT_SUCCESS);
  started = timestamp();

  if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
    log_debug("Reading file %s", image_file);
  }
  img = pixRead(image_file);

  assert(img != NULL);
  if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
    log_info("Read %s Image %s in %s",
             bytes_to_string(fsio_file_size(image_file)), image_file, milliseconds_to_string(timestamp() - started)
             );
  }
  started = timestamp();
  TessBaseAPISetImage2(api, img);
  loaded_img = TessBaseAPIGetInputImage(api);

  assert(loaded_img != NULL);

  if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
    log_info("Loaded %s Image %s in %s",
             bytes_to_string(fsio_file_size(image_file)), image_file, milliseconds_to_string(timestamp() - started)
             );
  }
  pixDestroy(&img);
  started = timestamp();
  assert(TessBaseAPIRecognize(api, NULL) == EXIT_SUCCESS);
  if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
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

static struct parsed_data_t *parse_request(const struct http_request_s *request){
  http_string_t url = http_request_target(request);
  http_string_t _method = http_request_method(request);
  char          *method = stringfn_substring(_method.buf, 0, _method.len), *purl;

  if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
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
            if (res == EXIT_SUCCESS && DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
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
} /* request_target_is */

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
    size_t               display_id = validate_id_or_get_default_capture_id(CAPTURE_TYPE_DISPLAY, data->display_id);
    char                 *output_file;
    asprintf(&output_file, "%sdisplay-%lu-%lld.png", gettempdir(), display_id, timestamp());
    CGImageRef           img_ref     = capture(CAPTURE_TYPE_DISPLAY, display_id);
    size_t               rgb_len     = 0;
    unsigned char        *rgb_pixels = cgimage_ref_to_rgb_pixels(img_ref, &rgb_len);
    log_debug("Got %lu rgb pixels", rgb_len);
    fsio_write_binary_file("/tmp/a.rgb", rgb_pixels, rgb_len);

    save_cgref_to_qoi_file(img_ref, "/tmp/a.qoi");

    size_t        qoi_len     = 0;
    unsigned char *qoi_pixels = cgref_to_qoi_memory(img_ref, &qoi_len);
    log_debug("Got %lu qoi pixels", qoi_len);

    int qoi_width = 0, qoi_height = 0;
    image_types[IMAGE_TYPE_QOI].get_dimensions(qoi_pixels, qoi_len, &qoi_width, &qoi_height);
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
      ok = image_types[IMAGE_TYPE_PNG].validator(pixels);
      log_debug("PNG OK: %s %s %s", ok?"Yes":"No", file, bytes_to_string(fsio_file_size(file)));
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
      ok = image_types[IMAGE_TYPE_GIF].validator(pixels);
      log_debug("GIF OK: %s %s %s", ok?"Yes":"No", file, bytes_to_string(fsio_file_size(file)));
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
      image_types[IMAGE_TYPE_TIFF].get_dimensions(pixels, len, &width, &height);
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
      image_types[IMAGE_TYPE_BMP].get_dimensions(pixels, len, &width, &height);
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
      image_types[IMAGE_TYPE_JPEG].get_dimensions(pixels, len, &width, &height);
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
    size_t               space_id = validate_id_or_get_default_capture_id(CAPTURE_TYPE_SPACE, data->space_id);
    char                 *output_file;
    asprintf(&output_file, "%sspace-%lu-%lld.png", gettempdir(), space_id, timestamp());
    CGImageRef           img_ref = capture(CAPTURE_TYPE_SPACE, space_id);
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
        if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
          log_info("Converting to grayscale");
        }
        output_file = convert_png_to_grayscale(output_file, data->resize_factor);
      }else{
        if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
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
    size_t               window_id = validate_id_or_get_default_capture_id(CAPTURE_TYPE_WINDOW, data->window_id);
    char                 *output_file = NULL, *extracted_text = NULL;
    asprintf(&output_file, "%swindow-%lu-%lld.png", gettempdir(), window_id, timestamp());
    CGImageRef           img_ref = capture(CAPTURE_TYPE_WINDOW, window_id);
    if (save_cgref_to_png_file(img_ref, output_file) == true && fsio_file_exists(output_file) && fsio_file_size(output_file) > 4096) {
      if (data->grayscale_conversion == true) {
        if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
          log_info("Converting to grayscale");
        }
        output_file = convert_png_to_grayscale(output_file, data->resize_factor);
      }else{
        if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
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
    size_t               window_id    = validate_id_or_get_default_capture_id(CAPTURE_TYPE_WINDOW, data->window_id);
    char                 *output_file = NULL;
    unsigned char        *png_data    = NULL;
    size_t               png_len      = 0;
    asprintf(&output_file, "%swindow-%lu-%lld.png", gettempdir(), window_id, timestamp());
    CGImageRef           img_ref = capture(CAPTURE_TYPE_WINDOW, window_id);
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
    unsigned long long      config_started = timestamp();
    char                    *cfg_path = get_homedir_yaml_config_file_path(), *cfg_data_s = NULL, *cfg_data_pretty_s = NULL;
    assert(cfg_path != NULL && fsio_file_exists(cfg_path) && fsio_file_size(cfg_path) > 0);
    struct hotkeys_config_t *cfg = load_yaml_config_file_path(cfg_path);
    assert(cfg != NULL);
    char                    *cfg_contents = fsio_read_text_file(cfg_path), *cfg_contents_b64 = b64_encode(cfg_contents, strlen(cfg_contents));
    assert(cfg_contents != NULL && cfg_contents_b64 != NULL);
    JSON_Value              *cfg_data    = json_value_init_object();
    int                     cur_space_id = get_space_id();
    uid_t                   euid         = geteuid();
    struct passwd           *user        = getpwuid(euid);
    struct utsname          buffer;
    struct statvfs          info;
    struct Vector           *monitors_v = get_monitors_v(), *displays_v = get_displays_v(), *spaces_v = get_spaces_v();
    struct Vector           *_space_window_ids_v = get_space_window_ids_v(cur_space_id), *window_infos_v = get_window_infos_v();
    struct Vector           *_space_minimized_window_ids_v = get_space_minimized_window_ids_v(cur_space_id);
    struct Vector           *_space_non_minimized_window_ids_v = get_space_non_minimized_window_ids_v(cur_space_id);
    struct Vector           *_space_owners_v = get_space_owners(cur_space_id), *_process_infos_v = get_all_process_infos_v();
//    struct Vector *_installed_apps_v = get_installed_apps_v();
//    struct Vector *_installed_fonts_v = get_installed_fonts_v();
    unsigned long left      = (info.f_bavail * info.f_frsize) / 1024 / 1024;
    unsigned long total     = (info.f_blocks * info.f_frsize) / 1024 / 1024;
    unsigned long used      = total - left;
    int           used_perc = ((int)((float)used * 100 / (float)total * 100)) / 100;
    int           free_perc = 100 - used_perc;
    assert(uname(&buffer) == EXIT_SUCCESS);
    assert(statvfs("/", &info) == EXIT_SUCCESS);
    {
      json_object_set_value(json_object(cfg_data), "config", json_value_init_object());
      json_object_set_value(json_object(cfg_data), "status", json_value_init_object());
      json_object_set_value(json_object(cfg_data), "windows", json_value_init_array());
      json_object_set_value(json_object(cfg_data), "monitors", json_value_init_array());
      json_object_set_value(json_object(cfg_data), "processes", json_value_init_array());
      json_object_set_value(json_object(cfg_data), "spaces", json_value_init_array());
      json_object_set_value(json_object(cfg_data), "displays", json_value_init_array());
      json_object_set_value(json_object(cfg_data), "space", json_value_init_object());
      json_object_set_value(json_object(cfg_data), "dock", json_value_init_object());
      json_object_set_value(json_object(cfg_data), "menu_bar", json_value_init_object());
    }
    JSON_Value *cfg_o       = json_object_get_object(json_object(cfg_data), "config");
    JSON_Value *space_o     = json_object_get_object(json_object(cfg_data), "space");
    JSON_Value *dock_o      = json_object_get_object(json_object(cfg_data), "dock");
    JSON_Value *menu_bar_o  = json_object_get_object(json_object(cfg_data), "menu_bar");
    JSON_Value *status_o    = json_object_get_object(json_object(cfg_data), "status");
    JSON_Value *windows_a   = json_object_get_array(json_object(cfg_data), "windows");
    JSON_Value *processes_a = json_object_get_array(json_object(cfg_data), "processes");
    JSON_Value *monitors_a  = json_object_get_array(json_object(cfg_data), "monitors");
    JSON_Value *displays_a  = json_object_get_array(json_object(cfg_data), "displays");
    JSON_Value *spaces_a    = json_object_get_array(json_object(cfg_data), "spaces");
    json_object_set_value(cfg_o, "keys", json_value_init_array());
    JSON_Value *keys_a        = json_object_get_array(cfg_o, "keys");
    CGRect     cur_space_rect = get_space_rect(cur_space_id);
    {
      for (size_t i = 0; i < vector_size(displays_v); i++) {
        struct display_t *d   = (struct display_t *)vector_get(displays_v, i);
        JSON_Value       *d_v = json_value_init_object();
        json_object_set_number(json_object(d_v), "id", d->display_id);
        json_object_set_number(json_object(d_v), "height", d->height);
        json_object_set_number(json_object(d_v), "width", d->width);
        json_object_set_number(json_object(d_v), "index", d->index);
        json_object_set_boolean(json_object(d_v), "is_main", d->is_main);
        json_object_set_string(json_object(d_v), "uuid", d->uuid);
        json_object_set_number(json_object(d_v), "spaces_qty", vector_size(d->space_ids_v));
        json_object_set_value(json_object(d_v), "space_ids", json_value_init_array());
        JSON_Value *display_spaces_a = json_object_get_array(json_object(d_v), "space_ids");
        size_t     display_space_id;
        for (size_t ii = 0; ii < vector_size(d->space_ids_v); ii++) {
          display_space_id = (size_t)vector_get(d->space_ids_v, ii);
          json_array_append_number(display_spaces_a, display_space_id);
        }
        json_array_append_value(displays_a, d_v);
      }
    }
    {
      for (size_t i = 0; i < vector_size(monitors_v); i++) {
        struct monitor_t *m   = (struct monitor_t *)vector_get(monitors_v, i);
        JSON_Value       *m_v = json_value_init_object();
        json_object_set_number(json_object(m_v), "id", m->id);
        json_object_set_string(json_object(m_v), "uuid", m->uuid);
        json_object_set_string(json_object(m_v), "name", m->name);
        json_object_set_boolean(json_object(m_v), "is_primary", m->primary);
        json_object_set_number(json_object(m_v), "width_pixels", m->width_pixels);
        json_object_set_number(json_object(m_v), "height_pixels", m->height_pixels);
        json_object_set_number(json_object(m_v), "width_mm", m->width_mm);
        json_object_set_number(json_object(m_v), "height_mm", m->height_mm);
        json_object_set_number(json_object(m_v), "modes_qty", m->modes_qty);
        json_object_set_number(json_object(m_v), "refresh_hz", m->refresh_hz);
        json_array_append_value(monitors_a, m_v);
      }
    }
    {
      json_object_set_number(space_o, "id", cur_space_id);
      json_object_set_string(space_o, "uuid", get_space_uuid(cur_space_id));
      json_object_set_boolean(space_o, "is_fullscreen", get_space_is_fullscreen(cur_space_id));
      json_object_set_boolean(space_o, "is_active", get_space_is_active(cur_space_id));
      json_object_set_boolean(space_o, "is_visible", get_space_is_visible(cur_space_id));
      json_object_set_boolean(space_o, "is_user", get_space_is_user(cur_space_id));
      json_object_set_number(space_o, "windows_qty", vector_size(_space_window_ids_v));
      json_object_set_number(space_o, "minimized_windows_qty", vector_size(_space_minimized_window_ids_v));
      json_object_set_number(space_o, "non_minimized_windows_qty", vector_size(_space_non_minimized_window_ids_v));
      json_object_set_number(space_o, "owners_qty", vector_size(_space_owners_v));
      json_object_set_number(space_o, "display_id", get_display_id_for_space(cur_space_id));
      json_object_set_number(space_o, "type", get_space_type(cur_space_id));
      json_object_set_number(space_o, "width", (int)cur_space_rect.size.width);
      json_object_set_number(space_o, "height", (int)cur_space_rect.size.height);
      json_object_set_number(space_o, "x", (int)cur_space_rect.origin.x);
      json_object_set_number(space_o, "y", (int)cur_space_rect.origin.y);
    }
    {
      json_object_set_boolean(menu_bar_o, "is_visible", get_menu_bar_visible());
      json_object_set_number(menu_bar_o, "height", get_menu_bar_height());
    }
    {
      json_object_set_boolean(dock_o, "is_visible", dock_is_visible());
      json_object_set_number(dock_o, "orientation", dock_orientation());
      json_object_set_string(dock_o, "orientation_name", dock_orientation_name());
    }
    {
      json_object_set_string(cfg_o, "path", cfg_path);
      json_object_set_number(cfg_o, "hash", get_config_file_hash(cfg_path));
      json_object_set_number(cfg_o, "size", fsio_file_size(cfg_path));
      json_object_set_string(cfg_o, "name", cfg->name);
      json_object_set_string(cfg_o, "todo_app", cfg->todo_app);
      json_object_set_number(cfg_o, "todo_width", cfg->todo_width);
      json_object_set_number(cfg_o, "keys_count", cfg->keys_count);
      json_object_set_string(cfg_o, "raw_config_b64", cfg_contents_b64);
    }
    {
      struct space_t *s;
      for (size_t i = 0; i < vector_size(spaces_v); i++) {
        s = (struct space_t *)vector_get(spaces_v, i);
        JSON_Value *s_v = json_value_init_object();
        json_object_set_number(json_object(s_v), "id", s->id);
        json_object_set_number(json_object(s_v), "windows_qty", vector_size(s->window_ids_v));
        json_object_set_number(json_object(s_v), "display_id", get_display_id_for_space(s->id));
        json_object_set_boolean(json_object(s_v), "is_current", s->is_current);
        json_object_set_value(json_object(s_v), "window_ids", json_value_init_array());
        JSON_Value *space_windows_a = json_object_get_array(json_object(s_v), "window_ids");
        for (size_t ii = 0; ii < vector_size(s->window_ids_v); ii++) {
          size_t space_window_id = (size_t)vector_get(s->window_ids_v, ii);
          json_array_append_number(space_windows_a, space_window_id);
        }
        json_array_append_value(spaces_a, s_v);
      }
    }
    {
      struct process_info_t *p;
      for (size_t i = 0; i < vector_size(_process_infos_v); i++) {
        p = (struct process_info_t *)vector_get(_process_infos_v, i);
        JSON_Value *p_v = json_value_init_object();
        json_object_set_number(json_object(p_v), "pid", p->pid);
        json_array_append_value(processes_a, p_v);
      }
    }
    {
      json_object_set_number(status_o, "pid", getpid());
      json_object_set_string(status_o, "sysname", buffer.sysname);
      json_object_set_string(status_o, "nodename", buffer.nodename);
      json_object_set_string(status_o, "version", buffer.version);
      json_object_set_string(status_o, "machine", buffer.machine);
      json_object_set_string(status_o, "release", buffer.release);
      json_object_set_number(status_o, "disk_space_total_mb", total);
      json_object_set_number(status_o, "disk_space_free_mb", left);
      json_object_set_number(status_o, "disk_space_used_mb", total - left);
      json_object_set_number(status_o, "disk_space_used_percent", used_perc);
      json_object_set_number(status_o, "disk_space_free_percent", free_perc);
      json_object_set_number(status_o, "euid", geteuid());
      json_object_set_number(status_o, "uid", getuid());
      json_object_set_boolean(status_o, "is_root", (euid == 0) ? true : false);
      json_object_set_string(status_o, "username", user->pw_name);
      json_object_set_string(status_o, "homedir", user->pw_dir);
      json_object_set_number(status_o, "gid", getgid());
      json_object_set_number(status_o, "egid", getegid());
      json_object_set_string(status_o, "shell", user->pw_shell);
      json_object_set_number(status_o, "port", DARWIN_LS_HTTPSERVER_PORT);
      json_object_set_string(status_o, "host", DARWIN_LS_HTTPSERVER_HOST);
      json_object_set_string(status_o, "protocol", DARWIN_LS_HTTPSERVER_PROTOCOL);
      json_object_set_number(status_o, "space_id", cur_space_id);
      json_object_set_number(status_o, "display_id", get_display_id_for_space(cur_space_id));
    }
    {
      struct window_info_t *w;
      for (size_t i = 0; i < vector_size(window_infos_v); i++) {
        w = (struct window_info_t *)vector_get(window_infos_v, i);
        if (!w) {
          continue;
        }
        JSON_Value *w_v = json_value_init_object();
        json_object_set_number(json_object(w_v), "pid", w->pid);
        json_object_set_number(json_object(w_v), "window_id", w->window_id);
        json_object_set_number(json_object(w_v), "memory_usage", w->memory_usage);
        json_object_set_string(json_object(w_v), "name", w->name);
        json_object_set_string(json_object(w_v), "title", w->title);
        json_object_set_number(json_object(w_v), "height", (int)w->rect.size.height);
        json_object_set_number(json_object(w_v), "width", (int)w->rect.size.width);
        json_object_set_number(json_object(w_v), "x", (int)w->rect.origin.x);
        json_object_set_number(json_object(w_v), "y", (int)w->rect.origin.y);
        json_object_set_number(json_object(w_v), "sharing_state", w->sharing_state);
        json_object_set_number(json_object(w_v), "dur", w->dur);
        json_object_set_number(json_object(w_v), "layer", w->layer);
        json_object_set_number(json_object(w_v), "store_type", w->store_type);
        json_object_set_boolean(json_object(w_v), "is_focused", w->is_focused);
        json_object_set_boolean(json_object(w_v), "is_onscreen", w->is_onscreen);
        json_array_append_value(windows_a, w_v);
        if (w->is_focused == true) {
          JSON_Value *fw_v = json_value_deep_copy(w_v);
          json_object_set_value(json_object(cfg_data), "focused_window", json_value_init_object());
          json_object_set_value(json_object(cfg_data), "focused_window", fw_v);
        }
      }
    }
    struct key_t *key;
    for (size_t i = 0; i < cfg->keys_count; i++) {
      JSON_Value *key_v = json_value_init_object();
      key = get_hotkey_config_key(cfg, cfg->keys[i].key);
      json_object_set_string(json_object(key_v), "name", key->name);
      json_object_set_string(json_object(key_v), "key", key->key);
      json_object_set_string(json_object(key_v), "action", key->action);
      json_object_set_number(json_object(key_v), "type", (key->action_type));
      json_object_set_string(json_object(key_v), "description", action_type_descriptions[key->action_type]);
      json_object_set_boolean(json_object(key_v), "enabled", key->enabled);
      json_array_append_value(keys_a, key_v);
    }

    cfg_data_s = json_serialize_to_string(cfg_data);
    assert(cfg_data_s != NULL && strlen(cfg_data_s) > 0);
    if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
      cfg_data_pretty_s = json_serialize_to_string_pretty(cfg_data);
      log_info(AC_GREEN "%s" AC_RESETALL, cfg_data_pretty_s);
    }

    asprintf(&response_body,
             "{\"response_code\":%d,\"response_data\":\"%s\",\"ts\":%lu"
             ",\"config\":%s,\"dur\":%lu,\"size\":%lu"
             "}%s",
             HANDLED_REQUEST_RESPONSE_CODE, "config", (size_t)timestamp(),
             cfg_data_s, (size_t)((size_t)timestamp() - (size_t)config_started), strlen(cfg_data_s),
             ""
             );
    {
      http_response_status(response, HANDLED_REQUEST_RESPONSE_CODE);
      http_response_header(response, "Content-Type", CONTENT_TYPE_JSON);
      http_response_body(response, response_body, strlen(response_body));
      http_respond(request, response);
    }
    {
      free(response_body);
      free(cfg_data_s);
      free(cfg_path);
      if (cfg_data_pretty_s) {
        free(cfg_data_pretty_s);
      }
    }
    return;
  }
  http_respond(request, response);
} /* handle_request */

static void handle_sigterm(int signum) {
  (void)signum;
  if (server) {
    free(server);
  }
  exit(EXIT_SUCCESS);
}

static void __attribute__((constructor)) __constructor__darwin_ls_httpserver(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_DARWIN_LS_HTTPSERVEr") != NULL) {
    log_debug("Enabling darwin-ls httpserver Debug Mode");
    DARWIN_LS_HTTPSERVER_DEBUG_MODE = true;
  }
}

int httpserver_main() {
  signal(SIGTERM, handle_sigterm);
  server = http_server_init(DARWIN_LS_HTTPSERVER_PORT, handle_request);
  http_server_listen_addr(server, DARWIN_LS_HTTPSERVER_HOST);
}

#endif
