#pragma once
#ifndef LS_WIN_HTTPSERVER_C
#define LS_WIN_HTTPSERVER_C
#define HTTPSERVER_IMPL
#define INCBIN_SILENCE_BITCODE_WARNING
#define INCBIN_STYLE                                  INCBIN_STYLE_SNAKE
#define INCBIN_PREFIX                                 inc_
#define INCLUDE_FAVICON_FILE                          "assets/favicon.ico"
#define INCLUDE_ERROR_IMAGE                           "assets/error.png"
#define INCLUDE_REVEAL_MULTIPLE_PRESENTATIONS_HTML    "assets/multiple-presentations.html"
#define INCLUDE_REVEAL_DIST_REVEAL_JS                 "../submodules/c_deps/submodules/reveal.js/dist/reveal.js"
#define INCLUDE_REVEAL_DIST_REVEAL_CSS                "../submodules/c_deps/submodules/reveal.js/dist/reveal.css"
#define INCLUDE_REVEAL_DIST_THEME_WHITE_CSS           "../submodules/c_deps/submodules/reveal.js/dist/theme/white.css"
#define INCLUDE_REVEAL_DIST_MONOKAI_CSS               "../submodules/c_deps/submodules/reveal.js/plugin/highlight/monokai.css"
#define INCLUDE_REVEAL_DIST_HIGHLIGHT_JS              "../submodules/c_deps/submodules/reveal.js/plugin/highlight/highlight.js"
#define INCLUDE_REVEAL_DIST_MARKDOWN_JS               "../submodules/c_deps/submodules/reveal.js/plugin/markdown/markdown.js"
#define INCLUDE_REVEAL_DIST_MATH_JS                   "../submodules/c_deps/submodules/reveal.js/plugin/math/math.js"
#include "incbin/incbin.h"
INCBIN(favicon, INCLUDE_FAVICON_FILE);
INCBIN(errorpng, INCLUDE_ERROR_IMAGE);
INCBIN(reveal_js, INCLUDE_REVEAL_DIST_REVEAL_JS);
INCBIN(reveal_css, INCLUDE_REVEAL_DIST_REVEAL_CSS);
INCBIN(reveal_theme_white_css, INCLUDE_REVEAL_DIST_THEME_WHITE_CSS);
INCBIN(reveal_monokai_css, INCLUDE_REVEAL_DIST_MONOKAI_CSS);
INCBIN(reveal_highlight_js, INCLUDE_REVEAL_DIST_HIGHLIGHT_JS);
INCBIN(reveal_markdown_js, INCLUDE_REVEAL_DIST_MARKDOWN_JS);
INCBIN(reveal_math_js, INCLUDE_REVEAL_DIST_MATH_JS);
INCBIN(reveal_multiple_presentations_html, INCLUDE_REVEAL_MULTIPLE_PRESENTATIONS_HTML);
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "darwin-ls/darwin-ls-httpserver.h"
#include "hotkey-utils/hotkey-utils.h"
#include "httpserver.h/httpserver.h"
#include "log/log.h"
#include "monitor-utils/monitor-utils.h"
#include "parson/parson.h"
#include "querystring.c/querystring.h"
#include "screen-utils/screen-utils.h"
#include "space-utils/space-utils.h"
#include "submodules/b64.c/b64.h"
#include "timestamp/timestamp.h"
#include "url.h/url.h"
#include "window-utils/window-utils.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>
static int request_target_is(volatile const struct http_request_s *request, volatile const char *target);
static struct parsed_data *parse_request(volatile const struct http_request_s *request);
#define RESPONSE         "Hello, World!"
#define MAX_URL_PARTS    10
#define RETURN_ERROR_PNG(CAPTURE_TYPE, CAPTURE_TYPE_ID)                          \
  log_error("Failed to capture %s #%lu", CAPTURE_TYPE, (size_t)CAPTURE_TYPE_ID); \
  http_response_header(response, "Content-Type", "image/png");                   \
  http_response_body(response, inc_errorpng_data, inc_errorpng_size);
struct incbin_response_t {
  size_t size;
  char   *data; char *content_type; char *request_path;
};

static int iterate_incbin_responses(struct http_request_s *request, struct http_response_s *response){
  if (request_target_is(request, "/dist/theme/white.css")) {
    http_response_header(response, "Content-Type", CONTENT_TYPE_CSS);
    http_response_body(response, inc_reveal_theme_white_css_data, inc_reveal_theme_white_css_size);
    http_respond(request, response);
    return(EXIT_SUCCESS);
  }else if (request_target_is(request, "/plugin/highlight/monokai.css")) {
    http_response_header(response, "Content-Type", CONTENT_TYPE_CSS);
    http_response_body(response, inc_reveal_monokai_css_data, inc_reveal_monokai_css_size);
    http_respond(request, response);
    return(EXIT_SUCCESS);
  }else if (request_target_is(request, "/dist/reveal.js")) {
    http_response_header(response, "Content-Type", CONTENT_TYPE_JAVASCRIPT);
    http_response_body(response, inc_reveal_js_data, inc_reveal_js_size);
    http_respond(request, response);
    return(EXIT_SUCCESS);
  }else if (request_target_is(request, "/plugin/highlight/highlight.js")) {
    http_response_header(response, "Content-Type", CONTENT_TYPE_JAVASCRIPT);
    http_response_body(response, inc_reveal_highlight_js_data, inc_reveal_highlight_js_size);
    http_respond(request, response);
    return(EXIT_SUCCESS);
  }else if (request_target_is(request, "/plugin/markdown/markdown.js")) {
    http_response_header(response, "Content-Type", CONTENT_TYPE_JAVASCRIPT);
    http_response_body(response, inc_reveal_markdown_js_data, inc_reveal_markdown_js_size);
    http_respond(request, response);
    return(EXIT_SUCCESS);
  }else if (request_target_is(request, "/plugin/math/math.js")) {
    http_response_header(response, "Content-Type", CONTENT_TYPE_JAVASCRIPT);
    http_response_body(response, inc_reveal_math_js_data, inc_reveal_math_js_size);
    http_respond(request, response);
    return(EXIT_SUCCESS);
  }else if (request_target_is(request, "/dist/reveal.css")) {
    http_response_header(response, "Content-Type", CONTENT_TYPE_CSS);
    http_response_body(response, inc_reveal_css_data, inc_reveal_css_size);
    http_respond(request, response);
    return(EXIT_SUCCESS);
  }
  return(EXIT_FAILURE);
}

static const int DARWIN_LS_HTTPSERVER_PORT       = 49225;
static bool      DARWIN_LS_HTTPSERVER_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__darwin_ls_httpserver(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_DARWIN_LS_HTTPSERVEr") != NULL) {
    log_debug("Enabling darwin-ls httpserver Debug Mode");
    DARWIN_LS_HTTPSERVER_DEBUG_MODE = true;
  }
}
static struct http_server_s *server;

struct parsed_data {
  size_t window_id, space_id, display_id, screen_id, pid;
};

void parser(void *data, char *fst, char *snd) {
  struct parsed_data *parsed_data = (struct parsed_data *)data;

  if (strcmp(fst, "window_id") == 0) {
    parsed_data->window_id = (size_t)atoi(snd);
  }else if (strcmp(fst, "pid") == 0) {
    parsed_data->pid = (size_t)atoi(snd);
  }else if (strcmp(fst, "display_id") == 0) {
    parsed_data->display_id = (size_t)atoi(snd);
  }else if (strcmp(fst, "space_id") == 0) {
    parsed_data->space_id = (size_t)atoi(snd);
  }
}

static struct parsed_data *parse_request(volatile const struct http_request_s *request){
  volatile http_string_t url = http_request_target(request);
  char                   *urlbuf = strdup(url.buf), *purl;

  asprintf(&purl, "http://localhost:%d/%s", DARWIN_LS_HTTPSERVER_PORT, urlbuf);
  url_data_t         *parsed = url_parse(purl);
  char               *query  = strdup(parsed->query);
  struct parsed_data *data   = calloc(1, sizeof(struct parsed_data));

  data->window_id = -1;
  data->space_id  = -1;
  parse_querystring(query, (void *)data, parser);
  return(data);
}

static int request_target_is(volatile const struct http_request_s *request, volatile const char *target) {
  volatile http_string_t url;
  int                    res = EXIT_FAILURE;

  if (request && target) {
    url = http_request_target(request);
    char *urlbuf = strdup(url.buf);
    int  urllen  = url.len;
    if (url.buf && url.len > 0) {
      volatile char *purl = NULL, *url_buf = stringfn_substring(urlbuf, 1, urllen - 1);
      if (url_buf) {
        asprintf(&purl, "http://localhost:%d/%s", DARWIN_LS_HTTPSERVER_PORT, url_buf);
        if (purl) {
          url_data_t *parsed   = url_parse(purl);
          char       *pathname = strdup(parsed->pathname);
          if (pathname) {
            res = (url.buf && target && pathname && (memcmp(pathname, target, strlen(pathname))) == 0);
            if (res == EXIT_SUCCESS && strcmp(pathname, "/favicon.ico") != 0) {
              log_debug("%s", url_buf);
              log_debug("%s", purl);
              if (parsed) {
                // url_data_inspect(parsed);
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
  char *response_body = "{\"response_code\":404,\"response_data\":null}";

  http_request_connection(request, HTTP_AUTOMATIC);
  struct http_response_s *response = http_response_init();

  http_response_status(response, 200);
  if (iterate_incbin_responses(request, response) == EXIT_SUCCESS) {
    return;
  }
  if (request_target_is(request, "/echo")) {
    http_string_t body = http_request_body(request);
    http_response_header(response, "Content-Type", "text/plain");
    http_response_body(response, body.buf, body.len);
  } else if (request_target_is(request, "/enable_configured_key")) {
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
                 (ok == true) ? 200 : 500, (ok == true) ? "key_enabled" : "failed", (size_t)timestamp(),
                 ""
                 );
      }
    }
    http_response_header(response, "Content-Type", "application/json");
    http_response_body(response, response_body, strlen(response_body));
  } else if (request_target_is(request, "/multiple-presentations.html")) {
    http_response_header(response, "Content-Type", "text/html; charset=UTF-8");
    http_response_body(response, inc_reveal_multiple_presentations_html_data, inc_reveal_multiple_presentations_html_size);
  } else if (request_target_is(request, "/favicon.ico")) {
    http_response_header(response, "Cache-Control", "max-age:3600, immutable");
    http_response_header(response, "Content-Type", "image/x-icon");
    http_response_body(response, inc_favicon_data, inc_favicon_size);
  } else if (request_target_is(request, "/capture/display")) {
    struct parsed_data *data      = parse_request(request);
    size_t             display_id = (data->display_id > 0) ? (size_t)(data->display_id) : (size_t)(get_main_display_id());
    char               *output_file;
    asprintf(&output_file, "%sdisplay-%lu-%lld.png", gettempdir(), display_id, timestamp());
    CGImageRef         img_ref = capture_display_id(display_id);
    if (save_window_cgref_to_png(img_ref, output_file) == true) {
      size_t        png_len   = fsio_file_size(output_file);
      unsigned char *png_data = fsio_read_binary_file(output_file);
      fsio_remove(output_file);
      http_response_header(response, "Content-Type", "image/png");
      http_response_body(response, png_data, png_len);
    }else{
      RETURN_ERROR_PNG("Display", display_id);
    }
  } else if (request_target_is(request, "/capture/space")) {
    struct parsed_data *data    = parse_request(request);
    size_t             space_id = (data->space_id > 0) ? (size_t)(data->space_id) : (size_t)(get_space_id());
    char               *output_file;
    asprintf(&output_file, "%sspace-%lu-%lld.png", gettempdir(), space_id, timestamp());
    CGImageRef         img_ref = space_capture((uint32_t)space_id);
    if (save_window_cgref_to_png(img_ref, output_file) == true) {
      size_t        png_len   = fsio_file_size(output_file);
      unsigned char *png_data = fsio_read_binary_file(output_file);
      fsio_remove(output_file);
      http_response_header(response, "Content-Type", "image/png");
      http_response_body(response, png_data, png_len);
    }else{
      RETURN_ERROR_PNG("Space", space_id);
    }
  } else if (request_target_is(request, "/capture/window")) {
    struct parsed_data *data     = parse_request(request);
    size_t             window_id = (data->window_id > 0) ? (size_t)(data->window_id) : (size_t)(get_focused_window_id());
    char               *output_file;
    asprintf(&output_file, "%swindow-%lu-%lld.png", gettempdir(), window_id, timestamp());
    CGImageRef         img_ref = window_capture(get_window_id(window_id));
    if (save_window_cgref_to_png(img_ref, output_file) == true) {
      size_t        png_len   = fsio_file_size(output_file);
      unsigned char *png_data = fsio_read_binary_file(output_file);
      fsio_remove(output_file);
      http_response_header(response, "Content-Type", "image/png");
      http_response_body(response, png_data, png_len);
    }else{
      RETURN_ERROR_PNG("Window", window_id);
    }
  } else if (request_target_is(request, "/disable_configured_key")) {
    http_string_t body = http_request_body(request);
    if (body.len > 1) {
      JSON_Value              *schema   = json_parse_string(body.buf);
      char                    *key      = json_object_get_string(json_object(schema), "key");
      char                    *cfg_path = get_homedir_yaml_config_file_path();
      struct hotkeys_config_t *cfg      = load_yaml_config_file_path(cfg_path);
      struct key_t            *k        = get_hotkey_config_key(cfg, key);
      if (key && strlen(key) > 0 && k) {
        log_info("disabling key %s", key);
        bool ok = disable_hotkey_config_key(k->key);
        cfg = load_yaml_config_file_path(cfg_path);
        k   = get_hotkey_config_key(cfg, key);
        ok  = (ok == true && k->enabled == false) ? true : false;
        asprintf(&response_body,
                 "{\"response_code\":%d,\"response_data\":\"%s\",\"ts\":%lu"
                 "}"
                 "%s",
                 (ok == true) ? 200 : 500, (ok == true) ? "key_disabled" : "failed", (size_t)timestamp(),
                 ""
                 );
      }
    }
    http_response_header(response, "Content-Type", "application/json");
    http_response_body(response, response_body, strlen(response_body));
  } else if (request_target_is(request, "/config")) {
    http_string_t body = http_request_body(request);
    if (body.len > 1) {
      log_info("%s", body.buf);
      JSON_Value *schema = json_parse_string(body.buf);
      char       *name   = json_object_get_string(json_object(schema), "name");
      char       *type   = json_object_get_string(json_object(schema), "type");
      if (name && strlen(name) > 0) {
        if (type && strlen(type) > 0) {
          char                    *cfg_path         = get_homedir_yaml_config_file_path();
          struct hotkeys_config_t *cfg              = load_yaml_config_file_path(cfg_path);
          char                    *cfg_contents     = fsio_read_text_file(cfg_path);
          char                    *cfg_contents_b64 = b64_encode(cfg_contents, strlen(cfg_contents));
          JSON_Value              *cfg_data         = json_value_init_object();
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
          int        cur_space_id   = get_space_id();
          CGRect     cur_space_rect = get_space_rect(cur_space_id);
          {
            struct Vector *displays_v = get_displays_v();
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
              for (size_t ii = 0; ii < vector_size(d->space_ids_v); ii++) {
                size_t display_space_id = (size_t)vector_get(d->space_ids_v, ii);
                json_array_append_number(display_spaces_a, display_space_id);
              }
              json_array_append_value(displays_a, d_v);
            }
          }
          {
            struct Vector *monitors_v = get_monitors_v();
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
            struct Vector *_space_window_ids_v               = get_space_window_ids_v(cur_space_id);
            struct Vector *_space_minimized_window_ids_v     = get_space_minimized_window_ids_v(cur_space_id);
            struct Vector *_space_non_minimized_window_ids_v = get_space_non_minimized_window_ids_v(cur_space_id);
            struct Vector *_space_owners_v                   = get_space_owners(cur_space_id);

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
            struct Vector *spaces_v = spaces_v = get_spaces_v();
            for (size_t i = 0; i < vector_size(spaces_v); i++) {
              struct space_t *s = (struct space_t *)vector_get(spaces_v, i);
              log_info("space %d", s->id);
              JSON_Value     *s_v = json_value_init_object();
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
            struct Vector *_process_infos_v = get_all_process_infos_v();
            for (size_t i = 0; i < vector_size(_process_infos_v); i++) {
              struct process_info_t *p   = (struct process_info_t *)vector_get(_process_infos_v, i);
              JSON_Value            *p_v = json_value_init_object();
              json_object_set_number(json_object(p_v), "pid", p->pid);
              json_array_append_value(processes_a, p_v);
            }
          }
          {
            uid_t          euid  = geteuid();
            struct passwd  *user = getpwuid(euid);
            struct utsname buffer;
            struct statvfs info;

            errno = 0;
            if (uname(&buffer) < 0) {
              log_error("uname error");
              exit(EXIT_FAILURE);
            }

            if (statvfs("/", &info)) {
              log_error("statvfs error");
              exit(EXIT_FAILURE);
            }
            unsigned long left      = (info.f_bavail * info.f_frsize) / 1024 / 1024;
            unsigned long total     = (info.f_blocks * info.f_frsize) / 1024 / 1024;
            unsigned long used      = total - left;
            int           used_perc = ((int)((float)used * 100 / (float)total * 100)) / 100;
            int           free_perc = 100 - used_perc;

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
            json_object_set_number(status_o, "space_id", cur_space_id);
            json_object_set_number(status_o, "display_id", get_display_id_for_space(cur_space_id));
          }
          {
            struct Vector *window_infos_v = get_window_infos_v();
            for (size_t i = 0; i < vector_size(window_infos_v); i++) {
              struct window_info_t *w = (struct window_info_t *)vector_get(window_infos_v, i);
              if (!w) {
                continue;
              }
              JSON_Value *w_v = json_value_init_object();
              json_object_set_number(json_object(w_v), "pid", w->pid);
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
          for (size_t i = 0; i < cfg->keys_count; i++) {
            JSON_Value   *key_v = json_value_init_object();
            struct key_t *key   = get_hotkey_config_key(cfg, cfg->keys[i].key);
            json_object_set_string(json_object(key_v), "name", key->name);
            json_object_set_string(json_object(key_v), "key", key->key);
            json_object_set_string(json_object(key_v), "action", key->action);
            json_object_set_number(json_object(key_v), "type", (key->action_type));
            json_object_set_string(json_object(key_v), "description", action_type_descriptions[key->action_type]);
            json_object_set_boolean(json_object(key_v), "enabled", key->enabled);
            json_array_append_value(keys_a, key_v);
          }

          char *cfg_data_s        = json_serialize_to_string(cfg_data);
          char *cfg_data_pretty_s = json_serialize_to_string_pretty(cfg_data);
          if (DARWIN_LS_HTTPSERVER_DEBUG_MODE) {
            log_info("%s\n", cfg_data_pretty_s);
          }

          asprintf(&response_body,
                   "{\"response_code\":%d,\"response_data\":\"%s\",\"ts\":%lu"
                   ",\"config\":%s"
                   "}"
                   "%s",
                   200, "config", (size_t)timestamp(),
                   cfg_data_s,
                   ""
                   );
        }
      }
    }
    http_response_header(response, "Content-Type", "application/json");
    http_response_body(response, response_body, strlen(response_body));
  } else {
    http_response_header(response, "Content-Type", "text/plain");
    http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
  }
  http_respond(request, response);
} /* handle_request */

void handle_sigterm(int signum) {
  (void)signum;
  if (server) {
    free(server);
  }
  exit(EXIT_SUCCESS);
}

int httpserver_main() {
  signal(SIGTERM, handle_sigterm);
  server = http_server_init(DARWIN_LS_HTTPSERVER_PORT, handle_request);
  http_server_listen(server);
}

#endif
