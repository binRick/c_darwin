#pragma once
#ifndef CONFIG_UTILS_C
#define CONFIG_UTILS_C
#include "config-utils/config-utils.h"
#include "httpserver-utils/httpserver-utils.h"
#include "httpserver/httpserver.h"
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>
///////////////////////////////////////////////
#include "c_fsio/include/fsio.h"
#include "c_img/src/img.h"
#include "c_stringfn/include/stringfn.h"
#include "log/log.h"
#include "parson/parson.h"
#include "submodules/b64.c/b64.h"
#include "timestamp/timestamp.h"
#include "url.h/url.h"
///////////////////////////////////////////////
#include "app/utils/utils.h"
#include "capture/utils/utils.h"
#include "font-utils/font-utils.h"
#include "hotkey-utils/hotkey-utils.h"
#include "monitor/utils/utils.h"
#include "screen/utils/utils.h"
#include "space/utils/utils.h"
#include "string-utils/string-utils.h"
#include "window/utils/utils.h"
////////////////////////////////////////////
static bool CONFIG_UTILS_DEBUG_MODE = false;

///////////////////////////////////////////////////////////////////////
char *get_config_json_string_pretty(){
  return(json_serialize_to_string_pretty(get_json_config()));
}

char *get_config_json_string(){
  return(json_serialize_to_string(get_json_config()));
}

char *get_config_response_body(){
  unsigned long long config_started = timestamp();
  char               *cfg_data_s = get_config_json_string(), *response_body;

  asprintf(&response_body,
           "{\"response_code\":%d,\"response_data\":\"%s\",\"ts\":%lu"
           ",\"config\":%s,\"dur\":%lu,\"size\":%lu"
           "}%s",
           HANDLED_REQUEST_RESPONSE_CODE, "config", (size_t)timestamp(),
           cfg_data_s, (size_t)((size_t)timestamp() - (size_t)config_started), strlen(cfg_data_s),
           ""
           );
  if (cfg_data_s)
    free(cfg_data_s);
  return(response_body);
}

JSON_Value *get_json_config(){
  unsigned long long config_started = timestamp();
  char               *cfg_path      = get_homedir_yaml_config_file_path();

  assert(cfg_path != NULL && fsio_file_exists(cfg_path) && fsio_file_size(cfg_path) > 0);
  struct hotkeys_config_t *cfg = load_yaml_config_file_path(cfg_path);

  assert(cfg != NULL);
  char *cfg_contents = fsio_read_text_file(cfg_path), *cfg_contents_b64 = b64_encode(cfg_contents, strlen(cfg_contents));

  assert(cfg_contents != NULL && cfg_contents_b64 != NULL);
  JSON_Value     *cfg_data    = json_value_init_object();
  int            cur_space_id = get_current_space_id();
  uid_t          euid         = geteuid();
  struct passwd  *user        = getpwuid(euid);
  struct utsname buffer;
  struct statvfs info;
  struct Vector  *monitors_v = get_monitors_v(), *displays_v = get_displays_v(), *spaces_v = get_spaces_v();
  struct Vector  *_space_window_ids_v = get_space_window_ids_v(cur_space_id), *window_infos_v = get_window_infos_v();
  struct Vector  *_space_minimized_window_ids_v = get_space_minimized_window_ids_v(cur_space_id);
  struct Vector  *_space_non_minimized_window_ids_v = get_space_non_minimized_window_ids_v(cur_space_id);
  struct Vector  *_space_owners_v = get_space_owners(cur_space_id), *_process_infos_v = get_all_process_infos_v();
  //   struct Vector *_installed_apps_v = get_installed_apps_v();
  //struct Vector *_installed_fonts_v = get_installed_fonts_v();
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
      if (!w)
        continue;
      JSON_Value *w_v = json_value_init_object();
      json_object_set_number(json_object(w_v), "pid", w->pid);
      json_object_set_number(json_object(w_v), "display_id", w->display_id);
      json_object_set_number(json_object(w_v), "space_id", w->space_id);
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
  json_object_set_number(json_object(cfg_data), "dur", timestamp() - config_started);
  return(cfg_data);
} /* get_json_config */

static void __attribute__((constructor)) __constructor__config_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_config_utils") != NULL) {
    log_debug("Enabling config-utils Debug Mode");
    CONFIG_UTILS_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
