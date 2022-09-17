#pragma once
#ifndef LS_WIN_HTTPSERVER_C
#define LS_WIN_HTTPSERVER_C
#define DARWIN_LS_HTTPSERVER_PORT    49225
#define HTTPSERVER_IMPL
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "hotkey-utils/hotkey-utils.h"
#include "httpserver.h/httpserver.h"
#include "libyuarel/yuarel.h"
#include "log/log.h"
#include "parson/parson.h"
#include "querystring.c/querystring.h"
#include "timestamp/timestamp.h"
#include "url.h/url.h"
#include "window-utils/window-utils.h"
#include <signal.h>
#define RESPONSE         "Hello, World!"
#define MAX_URL_PARTS    10

struct parsed_data {
  char *name;
  int  age;
  int  size;
};

int request_target_is(struct http_request_s *request, char const *target) {
  http_string_t url = http_request_target(request);

  return(url.buf && target && (memcmp(url.buf, target, url.len)) == 0);
}

void handle_request(struct http_request_s *request) {
  http_request_connection(request, HTTP_AUTOMATIC);
  struct http_response_s *response = http_response_init();
  http_response_status(response, 200);
  if (request_target_is(request, "/echo")) {
    http_string_t body = http_request_body(request);
    http_response_header(response, "Content-Type", "text/plain");
    http_response_body(response, body.buf, body.len);
  } else if (request_target_is(request, "/config")) {
    http_string_t body           = http_request_body(request);
    char          *response_body = "{\"response_code\":404,\"response_data\":null}";
    if (body.len > 1) {
      log_info("%s", body.buf);
      JSON_Value *schema = json_parse_string(body.buf);
      char       *name   = json_object_get_string(json_object(schema), "name");
      char       *type   = json_object_get_string(json_object(schema), "type");
      if (name && strlen(name) > 0) {
        log_debug("name:%s", name);
        if (type && strlen(type) > 0) {
          log_debug("type:%s", type);
          char                    *cfg_path = get_homedir_yaml_config_file_path();
          struct hotkeys_config_t *cfg      = load_yaml_config_file_path(cfg_path);
          JSON_Value              *cfg_data = json_value_init_object();
          json_object_set_string(json_object(cfg_data), "path", cfg_path);
          json_object_set_number(json_object(cfg_data), "hash", get_config_file_hash(cfg_path));
          json_object_set_number(json_object(cfg_data), "size", fsio_file_size(cfg_path));
          json_object_set_string(json_object(cfg_data), "name", cfg->name);
          json_object_set_string(json_object(cfg_data), "todo_app", cfg->todo_app);
          json_object_set_number(json_object(cfg_data), "todo_width", cfg->todo_width);
          json_object_set_number(json_object(cfg_data), "keys_count", cfg->keys_count);
          {
            json_object_set_value(json_object(cfg_data), "status", json_value_init_object());
            JSON_Value *status_v = json_object_get_object(json_object(cfg_data), "status");
            json_object_set_number(status_v, "pid", getpid());
          }
          json_object_set_value(json_object(cfg_data), "windows", json_value_init_array());
          JSON_Value *windows_v = json_object_get_array(json_object(cfg_data), "windows");
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
              json_array_append_value(windows_v, w_v);
              if (w->is_focused == true) {
                JSON_Value *fw_v = json_value_deep_copy(w_v);
                json_object_set_value(json_object(cfg_data), "focused_window", json_value_init_object());
                json_object_set_value(json_object(cfg_data), "focused_window", fw_v);
              }
            }
          }
          json_object_set_value(json_object(cfg_data), "keys", json_value_init_array());
          JSON_Value *keys_v = json_object_get_array(json_object(cfg_data), "keys");
          for (size_t i = 0; i < cfg->keys_count; i++) {
            JSON_Value   *key_v = json_value_init_object();
            struct key_t *key   = get_hotkey_config_key(cfg, cfg->keys[i].key);
            json_object_set_string(json_object(key_v), "name", key->name);
            json_object_set_string(json_object(key_v), "key", key->key);
            json_object_set_string(json_object(key_v), "action", key->action);
            json_object_set_number(json_object(key_v), "type", (key->action_type));
            json_object_set_string(json_object(key_v), "description", action_type_descriptions[key->action_type]);
            json_object_set_boolean(json_object(key_v), "enabled", key->enabled);
            json_array_append_value(keys_v, key_v);
          }

          char *cfg_data_s        = json_serialize_to_string(cfg_data);
          char *cfg_data_pretty_s = json_serialize_to_string_pretty(cfg_data);
          log_info("%s\n", cfg_data_pretty_s);

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

struct http_server_s *server;

void handle_sigterm(int signum) {
  (void)signum;
  free(server);
  exit(0);
}

int httpserver_main() {
  signal(SIGTERM, handle_sigterm);
  server = http_server_init(DARWIN_LS_HTTPSERVER_PORT, handle_request);
  http_server_listen(server);
}

#endif
