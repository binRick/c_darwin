#pragma once
#include "dbg/dbg.h"
#include "generic-print/print.h"
#include "kitty/kitty.h"

struct Vector *get_kitty_procs(const char *KITTY_LS_RESPONSE){
  struct Vector *kitty_procs_v = vector_new();
  JSON_Value    *V             = json_parse_string(KITTY_LS_RESPONSE);
  JSON_Array    *A             = json_value_get_array(V);

  assert(json_array_get_count(A) > 0);
  JSON_Object *kitty_process_o;
  JSON_Value  *kitty_process_v;
  JSON_Value  *kitty_tabs_v;
  JSON_Array  *kitty_tabs_a;
  JSON_Value  *kitty_tab_v;
  JSON_Object *kitty_tab_o;
  JSON_Value  *kitty_windows_v;
  JSON_Array  *kitty_windows_a;
  JSON_Value  *kitty_foreground_process_v;
  JSON_Object *kitty_foreground_process_o;
  JSON_Array  *kitty_foreground_process_a;
  JSON_Array  *kitty_cmdline_a;
  JSON_Value  *kitty_cmdline_v;
  JSON_Object *kitty_cmdline_o;
  JSON_Array  *kitty_fg_cmdline_a;
  JSON_Value  *kitty_fg_cmdline_v;
  JSON_Object *kitty_fg_cmdline_o;
  JSON_Value  *kitty_window_v;
  JSON_Object *kitty_window_o;
  char        *kitty_cmdline_s;

  for (size_t kitty_process_index = 0; kitty_process_index < json_array_get_count(A); kitty_process_index++) {
    kitty_process_v = json_array_get_value(A, kitty_process_index);
    kitty_process_o = json_value_get_object(kitty_process_v);
    struct kitty_proc_t *kp = calloc(1, sizeof(struct kitty_proc_t));
    kp->id         = json_value_get_number(json_object_get_value(kitty_process_o, "id"));
    kp->window_id  = json_value_get_number(json_object_get_value(kitty_process_o, "platform_window_id"));
    kp->is_focused = json_value_get_boolean(json_object_get_value(kitty_process_o, "is_focused"));
    kp->tabs_qty   = json_array_get_count(json_value_get_array(json_object_get_value(kitty_process_o, "tabs")));
    kp->tabs_v     = vector_new();
    kitty_tabs_v   = json_object_get_value(kitty_process_o, "tabs");
    kitty_tabs_a   = json_value_get_array(kitty_tabs_v);
    for (size_t kitty_tab_index = 0; kitty_tab_index < kp->tabs_qty; kitty_tab_index++) {
      kitty_tab_v = json_array_get_value(kitty_tabs_a, kitty_tab_index);
      kitty_tab_o = json_value_get_object(kitty_tab_v);
      struct kitty_tab_t *kt = calloc(1, sizeof(struct kitty_tab_t));
      kt->id          = json_object_get_number(kitty_tab_o, "id");
      kt->title       = json_object_get_string(kitty_tab_o, "title");
      kt->is_focused  = json_object_get_boolean(kitty_tab_o, "is_focused");
      kt->layout      = json_object_get_string(kitty_tab_o, "layout");
      kt->windows_qty = json_array_get_count(json_value_get_array(json_object_get_value(kitty_tab_o, "windows")));
      kt->windows_v   = vector_new();
      kitty_windows_v = json_object_get_value(kitty_tab_o, "windows");
      kitty_windows_a = json_value_get_array(kitty_windows_v);
      for (size_t kitty_window_index = 0; kitty_window_index < kt->windows_qty; kitty_window_index++) {
        kitty_window_v             = json_array_get_value(kitty_windows_a, kitty_window_index);
        kitty_window_o             = json_value_get_object(kitty_window_v);
        kitty_foreground_process_v = json_object_get_value(kitty_window_o, "foreground_processes");
        kitty_foreground_process_o = json_value_get_object(kitty_foreground_process_v);
        kitty_foreground_process_a = json_value_get_array(kitty_foreground_process_v);
        struct kitty_window_t *kw = calloc(1, sizeof(struct kitty_window_t));
        kw->foreground_processes_qty = json_array_get_count(kitty_foreground_process_a);
        kw->foreground_processes     = vector_new();
        kw->id                       = json_object_get_number(kitty_window_o, "id");
        kw->lines                    = json_object_get_number(kitty_window_o, "lines");
        kw->columns                  = json_object_get_number(kitty_window_o, "columns");
        kw->pid                      = json_object_get_number(kitty_window_o, "pid");
        kw->is_focused               = json_object_get_boolean(kitty_window_o, "is_focused");
        kw->cwd                      = json_object_get_string(kitty_window_o, "cwd");
        kw->title                    = json_object_get_string(kitty_window_o, "title");
        for (size_t kitty_cmdline_index = 0; kitty_cmdline_index < kw->foreground_processes_qty; kitty_cmdline_index++) {
          kitty_cmdline_v    = json_array_get_value(kitty_foreground_process_a, kitty_cmdline_index);
          kitty_cmdline_o    = json_value_get_object(kitty_cmdline_v);
          kitty_fg_cmdline_v = json_object_get_value(kitty_cmdline_o, "cmdline");
          kitty_fg_cmdline_o = json_value_get_object(kitty_fg_cmdline_v);
          kitty_fg_cmdline_a = json_value_get_array(kitty_fg_cmdline_v);
          for (size_t kitty_fg_cmdline_index = 0; kitty_fg_cmdline_index < json_array_get_count(kitty_fg_cmdline_a); kitty_fg_cmdline_index++) {
            kitty_cmdline_s = json_array_get_string(kitty_fg_cmdline_a, kitty_fg_cmdline_index);
            //dbg(kitty_cmdline_s, %s);
          }
        }
//              kw->cmdline = stringfn_join(, " ", 0, json_array_get_count(cmdline_a));
        vector_push(kt->windows_v, kw);
      }
      vector_push(kp->tabs_v, kt);
    }
    vector_push(kitty_procs_v, kp);
  }
  return(kitty_procs_v);
} /* get_kitty_procs */
