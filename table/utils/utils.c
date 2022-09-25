#pragma once
#ifndef TABLE_UTILS_C
#define TABLE_UTILS_C
#include "active-app/active-app.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "font-utils/font-utils.h"
#include "frameworks/frameworks.h"
#include "hotkey-utils/hotkey-utils.h"
#include "kitty/kitty.h"
#include "libfort/lib/fort.h"
#include "monitor/utils/utils.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process/process.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "process/utils/utils.h"
#include "space/utils/utils.h"
#include "string-utils/string-utils.h"
#include "submodules/log.h/log.h"
#include "table/table.h"
#include "table/utils/utils.h"
#include "timestamp/timestamp.h"
#include "usbdevs-utils/usbdevs-utils.h"
#include "wildcardcmp/wildcardcmp.h"
#include "window/info/info.h"
#include "window/sort/sort.h"
///////////////////////////////////////////////////////////////////////////////
#define DEFAULT_FONTS_LIMIT    50
#define DEFAULT_APPS_LIMIT     50
#define MONITOR_COLUMNS        "Name", "UUID", "ID", "Primary", "Width", "Height", "Refresh", "Modes"
#define PROCESS_COLUMNS        "PID", "Open Ports", "Open Files", "Open Connections", "Dur"
#define KITTY_COLUMNS          "PID"
#define USB_COLUMNS            "Product", "Manufacturer"
#define FONT_COLUMNS           "ID", "Family", "Enabled", "Size", "Type", "Style", "Faces", "Dupe"
#define APP_COLUMNS            "ID", "Name", "Version", "Path"
#define WINDOW_COLUMNS         "ID", "PID", "Application", "Size", "Position", "Space", "Disp", "Min", "Dur"
#define SPACE_COLUMNS          "ID", "Current", "Window IDs", "Windows"
#define HOTKEY_COLUMNS         "ID", "Name", "Key", "Action", "Enabled"
#define DISPLAY_COLUMNS        "Index", "ID", "Main", "Width", "Height", "# Spaces", "# Windows"
static size_t               term_width, cur_display_id, cur_space_id;
static bool string_compare_skip_row(char *s0, char *s1, bool exact_match, bool case_sensitive);
static struct table_logic_t *tables[TABLE_TYPES_QTY] = {
#define VECTOR_ITEM(VECTOR,                                                                          TYPE,                                  INDEX)    (struct TYPE)vector_get(VECTOR, INDEX)
  [TABLE_TYPE_MONITOR] = &(struct table_logic_t){
#define type    monitor_t
    .query_items = get_monitors_v,                                                                   .columns = { MONITOR_COLUMNS },
    .row         = ^ void (
      ft_table_t *table,
      size_t __attribute__((unused)) index,
      void *item
      ){
      struct type *i = (struct type *)item;
      ft_printf_ln(table,
                   "%s"
                   "|%s"
                   "|%d"
                   "|%s"
                   "|%lu/%lu"
                   "|%lu/%lu"
                   "|%d"
                   "|%lu"
                   "%s",
                   (i->name),
                   (i->uuid),
                   (i->id),
                   (i->primary)?"Yes":"No",
                   (i->width_pixels),                                                                (i->width_mm),
                   (i->height_pixels),                                                               (i->height_mm),
                   (i->refresh_hz),
                   (i->modes_qty),
                   "");
    },
    .row_skip = ^ bool (
      ft_table_t __attribute__((unused)) *table,
      size_t __attribute__((unused)) index,
      void __attribute__((unused)) *item,
      struct list_table_t __attribute__((unused)) *args
      ){
      return(false);
    },
    .row_style = ^ void (
      ft_table_t *table,
      size_t i,
      void __attribute__((unused)) *item
      ){
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
    },
  },
#undef type
  [TABLE_TYPE_KITTY] = &(struct table_logic_t){
#define type    kitty_t
    .query_items = get_kittys,                                                                       .columns = { PROCESS_COLUMNS },
    .row         = ^ void (
      ft_table_t *table,
      size_t __attribute__((unused)) index,
      void *item
      ){
      struct type *i = (struct type *)item;
      ft_printf_ln(table,                                                                            "%d"
                   "%s",
                   (i->pid),
                   "");
    },
    .row_skip = ^ bool (
      ft_table_t __attribute__((unused)) *table,
      size_t __attribute__((unused)) index,
      void __attribute__((unused)) *item,
      struct list_table_t __attribute__((unused)) *args
      ){
      return(false);
    },
    .row_style = ^ void (
      ft_table_t *table,
      size_t i,
      void __attribute__((unused)) *item
      ){
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
    },
#undef type
  },
  [TABLE_TYPE_PROCESS] = &(struct table_logic_t){
#define type    process_info_t
    .query_items = get_all_process_infos_v,                                                          .columns = { PROCESS_COLUMNS },
    .row         = ^ void (
      ft_table_t *table,
      size_t __attribute__((unused)) index,
      void *item
      ){
      struct type *i = (struct type *)item;
      ft_printf_ln(table,                                                                            "%d|%lu|%lu|%lu|%lu"
                   "%s",
                   (i->pid),
                   vector_size(i->open_ports_v),
                   vector_size(i->open_files_v),
                   vector_size(i->open_connections_v),
                   (i->dur),
                   "");
    },
    .row_skip = ^ bool (
      ft_table_t __attribute__((unused)) *table,
      size_t __attribute__((unused)) index,
      void __attribute__((unused)) *item,
      struct list_table_t __attribute__((unused)) *args
      ){
      return(false);
    },
    .row_style = ^ void (
      ft_table_t *table,
      size_t i,
      void __attribute__((unused)) *item
      ){
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
    },
#undef type
  },
  [TABLE_TYPE_USB] = &(struct table_logic_t){
    .query_items = get_usb_devices_v,                                                                .columns = { USB_COLUMNS },
    .row         = ^ void (
      ft_table_t *table,
      size_t __attribute__((unused)) index,
      void *item
      ){
      struct usbdev_t *i = (struct usbdev_t *)item;
      ft_printf_ln(table,                                                                            "%.*s|%.*s"
                   "%s",
                   (int)((float)term_width * .40),                                                   stringfn_trim(i->product_string),
                   (int)((float)term_width * .40),                                                   stringfn_trim(i->manufacturer_string),
                   "");
    },
    .row_skip = ^ bool (
      ft_table_t __attribute__((unused)) *table,
      size_t __attribute__((unused)) index,
      void __attribute__((unused)) *item,
      struct list_table_t __attribute__((unused)) *args
      ){
      return(false);
    },
    .row_style = ^ void (
      ft_table_t *table,
      size_t i,
      void __attribute__((unused)) *item
      ){
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
    },
  },
  [TABLE_TYPE_FONT] = &(struct table_logic_t){
    .query_items = get_installed_fonts_v,
    .columns     = { FONT_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t index,                          void *item){
      struct font_t *f = (struct font_t *)item;
      term_width = get_terminal_width();
      ft_printf_ln(table,
                   "%ld"
                   "|%.*s"
                   "|%.*s"
                   "|%.*s"
                   "|%.*s"
                   "|%.*s"
                   "|%lu"
                   "|%.*s"
                   "%s",
                   index + 1,
                   (int)((float)term_width * .40),                                                   stringfn_trim(f->family),
                   (int)((float)term_width * .5),                                                    f->enabled ? "Yes":"No",
                   (int)((float)term_width * .8),                                                    bytes_to_string(f->size),
                   (int)((float)term_width * .10),                                                   stringfn_trim(f->type),
                   (int)((float)term_width * .10),                                                   stringfn_trim(f->style),
                   f->typefaces_qty,
                   (int)((float)term_width * .5),                                                    f->duplicate ? "Yes":"No",
                   ""
                   );
    },
    .row_skip = ^ bool (ft_table_t __attribute__((unused)) *table,                                   size_t __attribute__((unused)) i,      void *item,                               struct list_table_t *args){
      struct font_t *f = (struct font_t *)item;
      return((string_compare_skip_row(args->font_family,                                             f->family,                             args->exact_match,                        args->case_sensitive))
             || (string_compare_skip_row(args->font_name,                                            f->name,                               args->exact_match,                        args->case_sensitive))
             || (string_compare_skip_row(args->font_style,                                           f->style,                              args->exact_match,                        args->case_sensitive))
             || (string_compare_skip_row(args->font_type,                                            f->type,                               args->exact_match,                        args->case_sensitive))
             || (args->duplicate && !f->duplicate)
             || (args->non_duplicate && f->duplicate)
             );
    },
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                              void *item){
      struct font_t *f = (struct font_t *)item;
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                 2,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                 2,                                        FT_CPROP_CONT_FG_COLOR, (f->enabled == true)?FT_COLOR_GREEN:FT_COLOR_RED);
      ft_set_cell_prop(table,                                                                        i + 1,                                 3,                                        FT_CPROP_CONT_FG_COLOR, f->size > 5 * 1024 * 1024 ? FT_COLOR_RED : f->size > 1024 * 1024 ? FT_COLOR_BLUE : FT_COLOR_LIGHT_BLUE);
      ft_set_cell_prop(table,                                                                        i + 1,                                 4,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                 4,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                 4,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      ft_set_cell_prop(table,                                                                        i + 1,                                 5,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      ft_set_cell_prop(table,                                                                        i + 1,                                 5,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                 6,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_BLUE);
      ft_set_cell_prop(table,                                                                        i + 1,                                 6,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                 7,                                        FT_CPROP_CONT_FG_COLOR, f->duplicate ? FT_COLOR_RED : FT_COLOR_GREEN);
      ft_set_cell_prop(table,                                                                        i + 1,                                 7,                                        FT_CPROP_CONT_TEXT_STYLE, f->duplicate ? (FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED) : FT_TSTYLE_DEFAULT);
    },
  },
  [TABLE_TYPE_APP] = &(struct table_logic_t){
    .query_items = get_installed_fonts_v,
    .columns     = { APP_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t index,                          void *item){
      struct app_t *app = (struct app_t *)item;
      term_width = get_terminal_width();
      ft_printf_ln(table,
                   "%ld"
                   "|%.*s"
                   "|%.*s"
                   "|%.*s"
                   "%s",
                   index + 1,
                   (int)((float)term_width * .25),                                                   app->name,
                   (int)((float)term_width * .5),                                                    app->version,
                   (int)((float)term_width * .50),                                                   app->path,
                   ""
                   );
    },
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                              void *item){
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);

      ft_set_cell_prop(table,                                                                        i + 1,                                 2,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
      ft_set_cell_prop(table,                                                                        i + 1,                                 2,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
      ft_set_cell_prop(table,                                                                        i + 1,                                 2,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);

      ft_set_cell_prop(table,                                                                        i + 1,                                 3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);
    },
    .row_skip = ^ bool (ft_table_t __attribute__((unused)) *table,                                   size_t __attribute__((unused)) i,      void *item,                               struct list_table_t *args){
      struct app_t *app = (struct app_t *)item;
      return(
        string_compare_skip_row(args->application_name,                                              app->name,                             args->exact_match,                        args->case_sensitive)
        );
    },
  },
  [TABLE_TYPE_WINDOW] = &(struct table_logic_t){
    .query_items = get_window_infos_v,
    .columns     = { WINDOW_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t __attribute__((unused)) index,  void *item){
      int max_name_len        = 20;
      struct window_info_t *w = (struct window_info_t *)item;
      ft_printf_ln(table,
                   "%lu|%d|%.*s|%dx%d|%dx%d|%lu|%d|%s|%s"
                   "%s",
                   w->window_id,
                   w->pid,
                   max_name_len,                                                                     w->name,
                   (int)w->rect.size.width,                                                          (int)w->rect.size.height,
                   (int)w->rect.origin.x,                                                            (int)w->rect.origin.y,
                   w->space_id,
                   get_display_id_index(w->display_id),
                   (w->is_minimized == true) ? "Yes" : "No",
                   milliseconds_to_string(w->dur),
                   "");
    },
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                              void *item){
      struct window_info_t *w = (struct window_info_t *)item;
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      ft_set_cell_prop(table,                                                                        i + 1,                                 2,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
      ft_set_cell_prop(table,                                                                        i + 1,                                 2,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      if (w->rect.size.width < 2 || w->rect.size.height < 2) {
        ft_set_cell_prop(table,                                                                      i + 1,                                 3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_BLUE);
      }else if (w->rect.size.width == 0 && w->rect.size.height == 0) {
        ft_set_cell_prop(table,                                                                      i + 1,                                 3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_YELLOW);
      }else{
        ft_set_cell_prop(table,                                                                      i + 1,                                 3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      }
      if ((int)w->rect.origin.x < 0 || (int)w->rect.origin.y < 0) {
        ft_set_cell_prop(table,                                                                      i + 1,                                 4,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
        ft_set_cell_prop(table,                                                                      i + 1,                                 4,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      }else{
        ft_set_cell_prop(table,                                                                      i + 1,                                 4,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GREEN);
      }
      if (w->space_id == cur_space_id) {
        ft_set_cell_prop(table,                                                                      i + 1,                                 5,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
        ft_set_cell_prop(table,                                                                      i + 1,                                 5,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      }else{
        ft_set_cell_prop(table,                                                                      i + 1,                                 5,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_RED);
      }
      if (w->display_id == cur_display_id) {
        ft_set_cell_prop(table,                                                                      i + 1,                                 6,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_BLUE);
        ft_set_cell_prop(table,                                                                      i + 1,                                 6,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      }else{
        ft_set_cell_prop(table,                                                                      i + 1,                                 6,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);
      }
      if (w->is_minimized == true) {
        ft_set_cell_prop(table,                                                                      i + 1,                                 7,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
        ft_set_cell_prop(table,                                                                      i + 1,                                 7,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      }else{
        ft_set_cell_prop(table,                                                                      i + 1,                                 7,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      }
      if (w->dur == 0) {
        ft_set_cell_prop(table,                                                                      i + 1,                                 8,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      }else if (w->dur > 20) {
        ft_set_cell_prop(table,                                                                      i + 1,                                 8,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
        ft_set_cell_prop(table,                                                                      i + 1,                                 8,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      }else if (w->dur > 10) {
        ft_set_cell_prop(table,                                                                      i + 1,                                 8,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      }else {
        ft_set_cell_prop(table,                                                                      i + 1,                                 8,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_MAGENTA);
      }
    },
    .row_skip = ^ bool (ft_table_t __attribute__((unused)) *table,                                   size_t __attribute__((unused)) i,      void *item,                               struct list_table_t *args){
      struct window_info_t *w = (struct window_info_t *)item;
      bool skip_row           = false;
      if (string_compare_skip_row(args->application_name,                                            w->name,                               args->exact_match,                        args->case_sensitive)) {
        skip_row = true;
      }else if (args->width >= 0 && (int)w->rect.size.width != args->width) {
        skip_row = true;
      }else if (args->height >= 0 && (int)w->rect.size.height != args->height) {
        skip_row = true;
      }else if (args->height_greater >= 0 && (int)w->rect.size.height <= args->height_greater) {
        skip_row = true;
      }else if (args->height_less >= 0 && (int)w->rect.size.height >= args->height_less) {
        skip_row = true;
      }else if (args->width_greater >= 0 && (int)w->rect.size.width <= args->width_greater) {
        skip_row = true;
      }else if (args->width_less >= 0 && (int)w->rect.size.width >= args->width_less) {
        skip_row = true;
      }else if (args->pid > 0 && w->pid != args->pid) {
        skip_row = true;
      }else if (args->window_id > 0 && w->window_id != args->window_id) {
        skip_row = true;
      }else if ((args->display_id > 0 && (((size_t)(w->display_id) != (size_t)(args->display_id)) && ((size_t)(args->display_id) != (size_t)(get_display_id_index(w->display_id)))))) {
        skip_row = true;
      }else if (args->space_id > 0 && w->space_id != args->space_id) {
        skip_row = true;
      }else if (args->current_display_only == true && w->space_id != cur_space_id) {
        skip_row = true;
      }else if (args->current_space_only == true && w->space_id != cur_space_id) {
        skip_row = true;
      }
      return(skip_row);
    }
  },
  [TABLE_TYPE_SPACE] = &(struct table_logic_t){
    .query_items = get_spaces_v,
    .columns     = { SPACE_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t __attribute__((unused)) index,  void *item){
      struct space_t *space = (struct space_t *)item;
      term_width = get_terminal_width();
      ft_printf_ln(table,
                   "%d"
                   "|%s"
                   "|%s"
                   "|%lu"
                   "%s",
                   space->id,
                   space->is_current ? "Yes" : "No",
                   vector_size_ts_to_csv(space->window_ids_v,                                        term_width / 5),
                   vector_size(space->window_ids_v),
                   "");
    },
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                              void *item){
      struct space_t *space = (struct space_t *)item;

      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      if (space->is_current == true) {
        ft_set_cell_prop(table,                                                                      i + 1,                                 1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
        ft_set_cell_prop(table,                                                                      i + 1,                                 1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      }else{
        ft_set_cell_prop(table,                                                                      i + 1,                                 1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GRAY);
      }
      if (vector_size(space->window_ids_v) > 0) {
        ft_set_cell_prop(table,                                                                      i + 1,                                 3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GREEN);
      }else{
        ft_set_cell_prop(table,                                                                      i + 1,                                 3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_YELLOW);
      }
    },
  },
  [TABLE_TYPE_DISPLAY] = &(struct table_logic_t){
    .query_items = get_displays_v,
    .columns     = { DISPLAY_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t __attribute__((unused)) index,  void *item){
      struct display_t *display = (struct display_t *)item;
      ft_printf_ln(table,
                   "%lu"
                   "|%lu"
                   "|%s"
                   "|%d"
                   "|%d"
                   "|%lu"
                   "|%lu"
                   "%s",
                   display->index,
                   display->display_id,
                   display->is_main ? "Yes" : "No",
                   display->width,
                   display->height,
                   vector_size(display->space_ids_v),
                   vector_size(display->window_ids_v),
                   "");
    },
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                              void *item){
      struct display_t *display = (struct display_t *)item;
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                 2,                                        FT_CPROP_CONT_FG_COLOR, ((display->is_main == true) ? FT_COLOR_GREEN : FT_COLOR_RED));
      ft_set_cell_prop(table,                                                                        i + 1,                                 5,                                        FT_CPROP_CONT_FG_COLOR, ((vector_size(display->space_ids_v) > 0) ? FT_COLOR_GREEN : FT_COLOR_RED));
      ft_set_cell_prop(table,                                                                        i + 1,                                 6,                                        FT_CPROP_CONT_FG_COLOR, ((vector_size(display->window_ids_v) > 0) ? FT_COLOR_GREEN : FT_COLOR_RED));
    },
  },
  [TABLE_TYPE_HOTKEY] = &(struct table_logic_t){
    .query_items = get_config_keys_v,
    .columns     = { HOTKEY_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t __attribute__((unused)) index,  void *item){
      struct key_t *hk = (struct key_t *)item;
      ft_printf_ln(table,
                   "%ld"
                   "|%s"
                   "|%.45s"
                   "|%s"
                   "|%s"
                   "%s",
                   index + 1,
                   (strlen(hk->name) > 0) ? hk->name : get_hotkey_type_action_name(hk->action_type),
                   hk->key,
                   (hk->action != NULL && strlen(hk->action) > 0 && strcmp(hk->action,               "null") != 0) ? hk->action : "",
                   hk->enabled ? "Yes" : "No",
                   "");
    },
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                              void *item){
      struct key_t *hk = (struct key_t *)item;
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      ft_set_cell_prop(table,                                                                        i + 1,                                 0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      ft_set_cell_prop(table,                                                                        i + 1,                                 1,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
      ft_set_cell_prop(table,                                                                        i + 1,                                 2,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
      ft_set_cell_prop(table,                                                                        i + 1,                                 2,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                 3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);
      ft_set_cell_prop(table,                                                                        i + 1,                                 4,                                        FT_CPROP_CONT_FG_COLOR, (hk->enabled == true)? FT_COLOR_GREEN : FT_COLOR_RED);
    },
  },
};
#define INIT_TABLE(TABLE, COLUMNS)                                 { do {                                                                                   \
                                                                       durs[TABLE_DUR_TYPE_QUERY_ITEMS].dur = 0;                                            \
                                                                       durs[TABLE_DUR_TYPE_TOTAL].dur       = 0;                                            \
                                                                       durs[TABLE_DUR_TYPE_FILTER_ROWS].dur = 0;                                            \
                                                                       TABLE                                = ft_create_table();                            \
                                                                       ft_write_ln(TABLE, COLUMNS);                                                         \
                                                                       ft_set_border_style(TABLE, FT_FRAME_STYLE);                                          \
                                                                       ft_set_border_style(TABLE, FT_SOLID_ROUND_STYLE);                                    \
                                                                       ft_set_tbl_prop(TABLE, FT_TPROP_LEFT_MARGIN, 0);                                     \
                                                                       ft_set_tbl_prop(TABLE, FT_TPROP_RIGHT_MARGIN, 0);                                    \
                                                                       ft_set_tbl_prop(TABLE, FT_TPROP_TOP_MARGIN, 0);                                      \
                                                                       ft_set_tbl_prop(TABLE, FT_TPROP_BOTTOM_MARGIN, 0);                                   \
                                                                       ft_set_cell_prop(TABLE, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);         \
                                                                       ft_set_cell_prop(TABLE, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);   \
                                                                       ft_set_cell_prop(TABLE, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD); \
                                                                       ft_set_cell_prop(TABLE, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);   \
                                                                       ft_set_cell_prop(TABLE, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);   \
                                                                     } while (0); }
#define BREAK_IF_ROW_LIMIT(TABLE, ARGS) \
  if (args->limit >= 0 && (size_t)ft_row_count(table) > (size_t)args->limit) break;
#define CONTINUE_IF_ROW_SKIP(SKIP) \
  if ((SKIP)) continue;
#define TABLE_SUMMARY(ITEM_NAME, ITEM_VECTOR, TABLE, DURATIONS)    { do {                                                                                             \
                                                                       durs[TABLE_DUR_TYPE_TOTAL].dur = timestamp() - durs[TABLE_DUR_TYPE_TOTAL].started;             \
                                                                       ft_add_separator(table);                                                                       \
                                                                       DURATIONS[TABLE_DUR_TYPE_TOTAL].dur = timestamp() - DURATIONS[TABLE_DUR_TYPE_TOTAL].started;   \
                                                                       ft_printf_ln(TABLE,                                                                            \
                                                                                    "Queried %lu %s in %s, filtered %lu items%s%s, and rendered %lu rows in %s"       \
                                                                                    "%s",                                                                             \
                                                                                    vector_size(ITEM_VECTOR),                                                         \
                                                                                    ITEM_NAME,                                                                        \
                                                                                    milliseconds_to_string(DURATIONS[TABLE_DUR_TYPE_QUERY_ITEMS].dur),                \
                                                                                    filtered_qty,                                                                     \
                                                                                    DURATIONS[TABLE_DUR_TYPE_FILTER_ROWS].dur > 0                                     \
        ? " in " : "",                                                                                                                                                \
                                                                                    DURATIONS[TABLE_DUR_TYPE_FILTER_ROWS].dur > 0                                     \
        ? milliseconds_to_string(DURATIONS[TABLE_DUR_TYPE_FILTER_ROWS].dur) : "",                                                                                     \
                                                                                    ft_row_count(TABLE) - 1,                                                          \
                                                                                    milliseconds_to_string(DURATIONS[TABLE_DUR_TYPE_TOTAL].dur),                      \
                                                                                    ""                                                                                \
                                                                                    );                                                                                \
                                                                       ft_set_cell_span(TABLE, ft_row_count(TABLE) - 1, 0, ft_col_count(TABLE));                      \
                                                                       ft_set_cell_prop(TABLE, ft_row_count(TABLE) - 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD); \
                                                                       char *table_s = ft_to_string(table);                                                           \
                                                                       fprintf(stdout, "%s\n", table_s);                                                              \
                                                                       ft_destroy_table(table);                                                                       \
                                                                       return((strlen(table_s) > 0) ? EXIT_SUCCESS : EXIT_FAILURE);                                   \
                                                                     } while (0); }
static bool TABLE_UTILS_DEBUG_MODE = false;

static bool string_compare_skip_row(char *s0, char *s1, bool exact_match, bool case_sensitive){
  if (!s0 || strlen(s0) < 1) {
    return(false);
  }
  bool skip_row = false;
  char *s[2];
  asprintf(&s[0], "%s%s%s", exact_match ? "" : "*", stringfn_trim(s0), exact_match ? "" : "*");
  asprintf(&s[1], "%s", stringfn_trim(s1));
  s[0]     = case_sensitive ? s[0] : stringfn_to_lowercase(s[0]);
  s[1]     = case_sensitive ? s[1] : stringfn_to_lowercase(s[1]);
  skip_row = (!wildcardcmp(s[0], s[1]));
  if (s[0]) {
    free(s[0]);
  }
  if (s[1]) {
    free(s[1]);
  }
  return(skip_row);
}

int list_installed_fonts_table(void *ARGS) {
  struct list_table_t *args = (struct list_table_t *)ARGS;

  args->limit <= 0 ? DEFAULT_FONTS_LIMIT : args->limit;
  struct table_dur_t durs[TABLE_DUR_TYPES_QTY];

  durs[TABLE_DUR_TYPE_TOTAL].started = timestamp();
  ft_table_t    *table;
  struct font_t *f;
  struct Vector *fonts_v;
  {
    term_width                               = get_terminal_width();
    durs[TABLE_DUR_TYPE_QUERY_ITEMS].started = timestamp();
    fonts_v                                  = tables[TABLE_TYPE_FONT]->query_items();
    INIT_TABLE(table, FONT_COLUMNS);
  }

  durs[TABLE_DUR_TYPE_SORT_ROWS].started = timestamp();
  if (args->sort_key && args->sort_direction && get_font_sort_function_from_key(args->sort_key, args->sort_direction)) {
    struct font_t *sorted_fonts = calloc((vector_size(fonts_v) + 1), sizeof(struct font_t)), *_sorted_fonts = vector_new();
    for (size_t i = 0; i < vector_size(fonts_v); i++) {
      sorted_fonts[i] = *(VECTOR_ITEM(fonts_v, font_t *, i));
    }
    qsort(sorted_fonts, vector_size(fonts_v), sizeof(struct font_t), get_font_sort_function_from_key(args->sort_key, args->sort_direction));
    for (size_t i = 0; i < vector_size(fonts_v); i++) {
      vector_push(_sorted_fonts, (void *)&(sorted_fonts[i]));
    }
    fonts_v = _sorted_fonts;
  }
  durs[TABLE_DUR_TYPE_SORT_ROWS].dur = timestamp() - durs[TABLE_DUR_TYPE_SORT_ROWS].started;
  size_t filtered_qty = 0;

  durs[TABLE_DUR_TYPE_FILTER_ROWS].dur = 0;
  for (unsigned long i = 0; i < vector_size(fonts_v); i++) {
    BREAK_IF_ROW_LIMIT(table, args);
    f                                        = VECTOR_ITEM(fonts_v, font_t *, i);
    durs[TABLE_DUR_TYPE_FILTER_ROWS].started = timestamp();
    CONTINUE_IF_ROW_SKIP((tables[TABLE_TYPE_FONT]->row_skip(table, i, f, args)))
    durs[TABLE_DUR_TYPE_FILTER_ROWS].dur += timestamp() - durs[TABLE_DUR_TYPE_FILTER_ROWS].started;
    tables[TABLE_TYPE_FONT]->row(table, i, f);
    tables[TABLE_TYPE_FONT]->row_style(table, i, f);
  }
  TABLE_SUMMARY("Fonts", fonts_v, table, durs)
} /* list_installed_fonts_table */

int list_window_infos_table(void *ARGS) {
  struct list_table_t  *args = (struct list_table_t *)ARGS;
  struct Vector        *window_infos_v;
  struct window_info_t *w, *sorted_window_infos, *_sorted_window_infos;
  struct table_dur_t   durs[TABLE_DUR_TYPES_QTY];

  term_width = get_terminal_width();
  size_t     max_name_len = 0;
  size_t     filtered_qty = 0;
  ft_table_t *table;

  durs[TABLE_DUR_TYPE_TOTAL].started = timestamp();
  {
    durs[TABLE_DUR_TYPE_QUERY_ITEMS].started = timestamp();
    window_infos_v                           = tables[TABLE_TYPE_WINDOW]->query_items();
    cur_space_id                             = get_current_space_id();
    cur_display_id                           = get_display_id_for_space(cur_space_id);
    durs[TABLE_DUR_TYPE_QUERY_ITEMS].dur     = timestamp() - durs[TABLE_DUR_TYPE_QUERY_ITEMS].started;
  }
  {
    for (size_t i = 0; i < vector_size(window_infos_v); i++) {
      w = VECTOR_ITEM(window_infos_v, window_info_t *, i);
      if (w->rect.size.width <= 1 && w->rect.size.height <= 1) {
        vector_remove(window_infos_v, i);
      }
    }
  }

  if (args->sort_key && args->sort_direction && get_window_sort_function_from_key(args->sort_key, args->sort_direction)) {
    sorted_window_infos = calloc((vector_size(window_infos_v) + 1), sizeof(struct window_info_t));

    _sorted_window_infos = vector_new();
    for (size_t i = 0; i < vector_size(window_infos_v); i++) {
      sorted_window_infos[i] = *(VECTOR_ITEM(window_infos_v, window_info_t *, i));
    }
    qsort(sorted_window_infos, vector_size(window_infos_v), sizeof(struct window_info_t), get_window_sort_function_from_key(args->sort_key, args->sort_direction));
    for (size_t i = 0; i < vector_size(window_infos_v); i++) {
      vector_push(_sorted_window_infos, (void *)&(sorted_window_infos[i]));
    }
    window_infos_v = _sorted_window_infos;
  }
  INIT_TABLE(table, WINDOW_COLUMNS);
  for (size_t i = 0; i < vector_size(window_infos_v); i++) {
    w            = VECTOR_ITEM(window_infos_v, window_info_t *, i);
    max_name_len = (strlen(w->name) > (size_t)max_name_len) ? (int)strlen(w->name) : max_name_len;
  }
  while (term_width > 0 && max_name_len > (term_width / 4)) {
    max_name_len--;
  }
  for (size_t i = 0; i < vector_size(window_infos_v); i++) {
    BREAK_IF_ROW_LIMIT(table, args)
    w = VECTOR_ITEM(window_infos_v, window_info_t *, i);
    CONTINUE_IF_ROW_SKIP(tables[TABLE_TYPE_WINDOW]->row_skip(table, i, w, args))
    tables[TABLE_TYPE_WINDOW]->row(table, i, w);
    tables[TABLE_TYPE_WINDOW]->row_style(table, i, w);
  }
  TABLE_SUMMARY("Windows", window_infos_v, table, durs);
} /* list_window_infos_table */

#define LIST_TABLE(FXN, NAME, TYPE, STRUCT_TYPE)                               \
  int FXN(void *ARGS) {                                                        \
    struct list_table_t *args    = (struct list_table_t *)ARGS;                \
    struct Vector       *items_v = tables[TABLE_TYPE_ ## TYPE]->query_items(); \
    struct STRUCT       *item;                                                 \
    size_t              filtered_qty = 0;                                      \
    struct table_dur_t  durs[TABLE_DUR_TYPES_QTY];                             \
    durs[TABLE_DUR_TYPE_TOTAL].dur       = 0;                                  \
    durs[TABLE_DUR_TYPE_FILTER_ROWS].dur = 0;                                  \
    durs[TABLE_DUR_TYPE_QUERY_ITEMS].dur = 0;                                  \
    ft_table_t *table;                                                         \
    INIT_TABLE(table, TYPE ## _COLUMNS);                                       \
    for (size_t i = 0; i < vector_size(items_v); i++) {                        \
      BREAK_IF_ROW_LIMIT(table, args)                                          \
      item = VECTOR_ITEM(items_v, TYPE *, i);                                  \
      tables[TABLE_TYPE_ ## TYPE]->row(table, i, item);                        \
      tables[TABLE_TYPE_ ## TYPE]->row_style(table, i, item);                  \
    }                                                                          \
    TABLE_SUMMARY(NAME, items_v, table, durs);                                 \
  }

LIST_TABLE(list_process_table, "Processes", PROCESS, process_info_t)
LIST_TABLE(list_usb_table, "USB Devices", USB, usbdev_t)
LIST_TABLE(list_kitty_table, "Kitty Processes", KITTY, kitty_t)
LIST_TABLE(list_spaces_table, "Spaces", SPACE, space_t)
LIST_TABLE(list_displays_table, "Displays", DISPLAY, display_t)
LIST_TABLE(list_app_table, "Applications", APP, app_t)
LIST_TABLE(list_hotkey_table, "Hotkeys", HOTKEY, key_t)
LIST_TABLE(list_monitor_table, "Monitors", MONITOR, monitor_t)

#endif
