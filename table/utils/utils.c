#pragma once
#ifndef TABLE_UTILS_C
#define TABLE_UTILS_C
#define MAX_NAME_TERMINAL_WIDTH_PERCENTAGE    0.20
#define LOCAL_DEBUG_MODE                      TABLE_UTILS_DEBUG_MODE
#define DEFAULT_FONTS_LIMIT                   50
#define DEFAULT_APPS_LIMIT                    50
#define CAPTURED_WINDOW_COLUMNS               "ID", "Width", "Height", "Dur", "Size", "Compressed", "Application", "File", "Format"
#define MONITOR_COLUMNS                       "Name", "UUID", "ID", "Primary", "Width", "Height", "Refresh", "Modes"
#define LAYOUT_COLUMNS                        "Name"
#define PROCESS_COLUMNS                       "PID", "Open Ports", "Open Files", "Open Connections", "PPID", "PPIDs", "# Env", "# PIDs", "Dur"
#define KITTY_COLUMNS                         "PID"
#define USB_COLUMNS                           "Product", "Manufacturer"
#define FONT_COLUMNS                          "ID", "Family", "Enabled", "Size", "Type", "Style", "Faces", "Dupe"
#define APP_COLUMNS                           "ID", "Name", "Version", "Path", "Icon"
#define WINDOW_COLUMNS                        "ID", "PID", "Name", "Size", "Position", "Space", "Disp", "Min", "Can Min", "Full", "Dur"
#define SPACE_COLUMNS                         "ID", "Current", "Window IDs", "Windows"
#define HOTKEY_COLUMNS                        "ID", "Name", "Key", "Action", "Enabled"
#define DISPLAY_COLUMNS                       "Index", "ID", "Main", "Width", "Height", "# Spaces", "# Windows", "Offset", "Center"
#include "active-app/active-app.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "core/utils/utils.h"
#include "db/db.h"
#include "font-utils/font-utils.h"
#include "frameworks/frameworks.h"
#include "hotkey-utils/hotkey-utils.h"
#include "kitty/kitty.h"
#include "layout/utils/utils.h"
#include "libfort/lib/fort.h"
#include "monitor/utils/utils.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "space/utils/utils.h"
#include "string-utils/string-utils.h"
#include "submodules/log/log.h"
#include "table/sort/sort.h"
#include "table/table.h"
#include "table/utils/utils.h"
#include "timestamp/timestamp.h"
#include "usbdevs-utils/usbdevs-utils.h"
#include "wildcardcmp/wildcardcmp.h"
#include "window/info/info.h"
///////////////////////////////////////////////////////////////////////////////
static size_t term_width = 80, cur_display_id, cur_space_id;
static bool string_compare_skip_row(char *s0, char *s1, bool exact_match, bool case_sensitive);
static struct table_logic_t *tables[TABLE_TYPES_QTY] = {
#define VECTOR_ITEM(VECTOR,                                                                          TYPE,                                    INDEX)    (struct TYPE)vector_get(VECTOR, INDEX)
  [TABLE_TYPE_LAYOUT] = &(struct table_logic_t){
#define type    layout_t
    .query_items = get_layouts_v,                                                                    .columns = { LAYOUT_COLUMNS },
    .row         = ^ void (
      ft_table_t *table,
      size_t __attribute__((unused)) index,
      void *item
      ){
      struct type *i = (struct type *)item;
      ft_printf_ln(table,
                   "%s",
                   "x"
                   );
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
    },
  },
#undef type
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
                   "|%.5s"
                   "|%d"
                   "|%s"
                   "|%lupx/%lumm"
                   "|%lupx/%lumm"
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
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
    },
  },
#undef type
  [TABLE_TYPE_KITTY] = &(struct table_logic_t){
#define type    kitty_t
    .query_items = get_kittys,                                                                       .columns = { KITTY_COLUMNS },
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
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
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
      ft_printf_ln(table,                                                                            "%d|%lu|%lu|%lu|%d|%lu|%lu|%lu|%lu"
                   "%s",
                   (i->pid),
                   vector_size(i->open_ports_v),
                   vector_size(i->open_files_v),
                   vector_size(i->open_connections_v),
                   (i->ppid),
                   vector_size(i->ppids_v),
                   vector_size(i->env_v),
                   vector_size(i->child_pids_v),
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
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
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
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
    },
  },
  [TABLE_TYPE_FONT] = &(struct table_logic_t){
    .query_items = get_installed_fonts_v,
    .columns     = { FONT_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t index,                            void *item){
      struct font_t *f = (struct font_t *)item;
      term_width = 120;//get_terminal_width();
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
    .row_skip = ^ bool (ft_table_t __attribute__((unused)) *table,                                   size_t __attribute__((unused)) i,        void *item,                               struct list_table_t *args){
      struct font_t *f = (struct font_t *)item;
      return((string_compare_skip_row(args->font_family,                                             f->family,                               args->exact_match,                        args->case_sensitive))
             || (string_compare_skip_row(args->font_name,                                            f->name,                                 args->exact_match,                        args->case_sensitive))
             || (string_compare_skip_row(args->font_style,                                           f->style,                                args->exact_match,                        args->case_sensitive))
             || (string_compare_skip_row(args->font_type,                                            f->type,                                 args->exact_match,                        args->case_sensitive))
             || (args->duplicate && !f->duplicate)
             || (args->non_duplicate && f->duplicate)
             );
    },
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                                void *item){
      struct font_t *f = (struct font_t *)item;
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                   2,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                   2,                                        FT_CPROP_CONT_FG_COLOR, (f->enabled == true)?FT_COLOR_GREEN:FT_COLOR_RED);
      ft_set_cell_prop(table,                                                                        i + 1,                                   3,                                        FT_CPROP_CONT_FG_COLOR, f->size > 5 * 1024 * 1024 ? FT_COLOR_RED : f->size > 1024 * 1024 ? FT_COLOR_BLUE : FT_COLOR_LIGHT_BLUE);
      ft_set_cell_prop(table,                                                                        i + 1,                                   4,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                   4,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                   4,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      ft_set_cell_prop(table,                                                                        i + 1,                                   5,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      ft_set_cell_prop(table,                                                                        i + 1,                                   5,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                   6,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_BLUE);
      ft_set_cell_prop(table,                                                                        i + 1,                                   6,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                   7,                                        FT_CPROP_CONT_FG_COLOR, f->duplicate ? FT_COLOR_RED : FT_COLOR_GREEN);
      ft_set_cell_prop(table,                                                                        i + 1,                                   7,                                        FT_CPROP_CONT_TEXT_STYLE, f->duplicate ? (FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED) : FT_TSTYLE_DEFAULT);
    },
  },
  [TABLE_TYPE_APP] = &(struct table_logic_t){
    .query_items = get_installed_apps_v,
    .columns     = { APP_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t index,                            void *item){
      struct app_t *app = (struct app_t *)item;
      term_width = get_terminal_width();
      ft_printf_ln(table,
                   "%ld"
                   "|%.*s"
                   "|%.*s"
                   "|%.*s"
                   "|%.*s"
                   "%s",
                   index + 1,
                   (int)((float)term_width * .25),                                                   app->name,
                   (int)((float)term_width * .5),                                                    app->version,
                   (int)((float)term_width * .40),                                                   app->path,
                   (int)((float)term_width * .10),                                                   bytes_to_string((size_t)(app->png_len)),
                   ""
                   );
    },
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                                void *item){
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);

      ft_set_cell_prop(table,                                                                        i + 1,                                   2,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
      ft_set_cell_prop(table,                                                                        i + 1,                                   2,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
      ft_set_cell_prop(table,                                                                        i + 1,                                   2,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);

      ft_set_cell_prop(table,                                                                        i + 1,                                   3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);
    },
    .row_skip = ^ bool (ft_table_t __attribute__((unused)) *table,                                   size_t __attribute__((unused)) i,        void *item,                               struct list_table_t *args){
      struct app_t *app = (struct app_t *)item;
      return(
        string_compare_skip_row(args->application_name,                                              app->name,                               args->exact_match,                        args->case_sensitive)
        );
    },
  },
  [TABLE_TYPE_CAPTURED_WINDOW] = &(struct table_logic_t){
    .query_items = get_captured_window_infos_v,
    .columns     = { CAPTURED_WINDOW_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t __attribute__((unused)) index,    void *item){
      return;
    },
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                                void *item){
      size_t row              = ft_row_count(table) - 1;
      struct window_info_t *w = (struct window_info_t *)item;
      return;
    },
    .row_skip = ^ bool (ft_table_t __attribute__((unused)) *table,                                   size_t __attribute__((unused)) i,        void *item,                               struct list_table_t *args){
      struct window_info_t *w = (struct window_info_t *)item;
      bool skip_row           = false;

      return(skip_row);
    },
  },
  [TABLE_TYPE_WINDOW] = &(struct table_logic_t){
    .query_items = get_window_infos_v,
    .columns     = { WINDOW_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t __attribute__((unused)) index,    void *item){
      struct window_info_t *w = (struct window_info_t *)item;
      int max_name_len        = (int)((float)get_terminal_width() * MAX_NAME_TERMINAL_WIDTH_PERCENTAGE);
      if (stringfn_ends_with(w->name,                                                                ".app"))
        w->name = stringfn_substring(w->name,                                                        0,                                       strlen(w->name) - strlen(".app"));
      ft_printf_ln(table,
                   "%lu|%d|%.*s|%dx%d|%dx%d|%lu|%d|%s|%s|%s|%s"
                   "%s",
                   w->window_id,
                   w->pid,
                   max_name_len,                                                                     w->name,
                   (int)w->rect.size.width,                                                          (int)w->rect.size.height,
                   (int)w->rect.origin.x,                                                            (int)w->rect.origin.y,
                   w->space_id,
                   get_display_id_index(w->display_id),
                   (w->is_minimized == true) ? "Yes" : "No",
                   (w->can_minimize == true) ? "Yes" : "No",
                   (w->is_fullscreen == true) ? "Yes" : "No",
                   milliseconds_to_string(w->dur),
                   "");
    },
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                                void *item){
      size_t row              = ft_row_count(table) - 1;
      struct window_info_t *w = (struct window_info_t *)item;
      ft_set_cell_prop(table,                                                                        row,                                     0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table,                                                                        row,                                     0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      ft_set_cell_prop(table,                                                                        row,                                     0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        row,                                     1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        row,                                     1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      ft_set_cell_prop(table,                                                                        row,                                     2,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
      ft_set_cell_prop(table,                                                                        row,                                     2,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      if (w->rect.size.width < 2 || w->rect.size.height < 2)
        ft_set_cell_prop(table,                                                                      row,                                     3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_BLUE);
      else if (w->rect.size.width == 0 && w->rect.size.height == 0)
        ft_set_cell_prop(table,                                                                      row,                                     3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_YELLOW);
      else
        ft_set_cell_prop(table,                                                                      row,                                     3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      if ((int)w->rect.origin.x < 0 || (int)w->rect.origin.y < 0) {
        ft_set_cell_prop(table,                                                                      row,                                     4,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
        ft_set_cell_prop(table,                                                                      row,                                     4,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      }else
        ft_set_cell_prop(table,                                                                      row,                                     4,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GREEN);
      if (w->space_id == cur_space_id) {
        ft_set_cell_prop(table,                                                                      row,                                     5,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
        ft_set_cell_prop(table,                                                                      row,                                     5,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      }else
        ft_set_cell_prop(table,                                                                      row,                                     5,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_RED);
      if (w->display_id == cur_display_id) {
        ft_set_cell_prop(table,                                                                      row,                                     6,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_BLUE);
        ft_set_cell_prop(table,                                                                      row,                                     6,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      }else
        ft_set_cell_prop(table,                                                                      row,                                     6,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);
      if (w->is_minimized == true) {
        ft_set_cell_prop(table,                                                                      row,                                     7,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
        ft_set_cell_prop(table,                                                                      row,                                     7,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      }else
        ft_set_cell_prop(table,                                                                      row,                                     7,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      if (w->can_minimize == true) {
        ft_set_cell_prop(table,                                                                      row,                                     8,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
        ft_set_cell_prop(table,                                                                      row,                                     8,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      }else
        ft_set_cell_prop(table,                                                                      row,                                     8,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      if (w->is_fullscreen == true) {
        ft_set_cell_prop(table,                                                                      row,                                     9,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
        ft_set_cell_prop(table,                                                                      row,                                     9,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      }else
        ft_set_cell_prop(table,                                                                      row,                                     9,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      if (w->dur == 0)
        ft_set_cell_prop(table,                                                                      row,                                     10,                                       FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      else if (w->dur > 20) {
        ft_set_cell_prop(table,                                                                      row,                                     10,                                       FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
        ft_set_cell_prop(table,                                                                      row,                                     10,                                       FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      }else if (w->dur > 10)
        ft_set_cell_prop(table,                                                                      row,                                     10,                                       FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      else
        ft_set_cell_prop(table,                                                                      row,                                     10,                                       FT_CPROP_CONT_FG_COLOR, FT_COLOR_MAGENTA);
    },
    .row_skip = ^ bool (ft_table_t __attribute__((unused)) *table,                                   size_t __attribute__((unused)) i,        void *item,                               struct list_table_t *args){
      struct window_info_t *w = (struct window_info_t *)item;
      bool skip_row           = false;
      return(skip_row);

      cur_display_id = get_current_display_id();
      cur_space_id   = get_current_space_id();
      if (args->application_name && string_compare_skip_row(args->application_name,                  w->name,                                 args->exact_match,                        args->case_sensitive))
        skip_row = true;
      if (args->application_name) {
        char *name;
        asprintf(&name,                                                                              "%s.app",                                args->application_name);
        if (
          (strcmp(stringfn_to_lowercase(args->application_name),                                     stringfn_to_lowercase(w->name)) != 0)
          && (strcmp(stringfn_to_lowercase(name),                                                    stringfn_to_lowercase(w->name)) != 0)
          )
          skip_row = true;
        if (name)
          free(name);
      }
      if (args->width >= 0 && (int)w->rect.size.width != args->width)
        skip_row = true;
      if (args->height >= 0 && (int)w->rect.size.height != args->height)
        skip_row = true;
      if (args->height_greater >= 0 && (int)w->rect.size.height <= args->height_greater)
        skip_row = true;
      if (args->height_less >= 0 && (int)w->rect.size.height >= args->height_less)
        skip_row = true;
      if (args->width_greater >= 0 && (int)w->rect.size.width <= args->width_greater)
        skip_row = true;
      if (args->width_less >= 0 && (int)w->rect.size.width >= args->width_less)
        skip_row = true;
      if (args->pid > 0 && w->pid != args->pid)
        skip_row = true;
      if (args->window_id > 0 && w->window_id != (size_t)args->window_id)
        skip_row = true;
      if ((args->display_id > -1) && (int32_t)(args->display_id) != (int32_t)get_display_id_index(w->display_id))
        skip_row = true;
      if ((args->space_id > -1) && (w->space_id != (size_t)args->space_id))
        skip_row = true;
      if (args->not_minimized_only == true && w->is_minimized == true)
        skip_row = true;
      if (args->minimized_only == true && w->is_minimized == false)
        skip_row = true;
      if (args->not_current_space_only == true && w->space_id == cur_space_id)
        skip_row = true;
      if (args->not_current_display_only == true && w->display_id == cur_display_id)
        skip_row = true;
      if (args->current_display_only == true && (size_t)(w->display_id) != (size_t)(cur_display_id))
        skip_row = true;
      if (args->current_space_only == true && w->space_id != cur_space_id)
        skip_row = true;
      return(skip_row);
    }
  },
  [TABLE_TYPE_SPACE] = &(struct table_logic_t){
    .query_items = get_spaces_v,
    .columns     = { SPACE_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t __attribute__((unused)) index,    void *item){
      struct space_t *space = (struct space_t *)item;
      term_width = 120;//get_terminal_width();
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
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                                void *item){
      struct space_t *space = (struct space_t *)item;

      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      if (space->is_current == true) {
        ft_set_cell_prop(table,                                                                      i + 1,                                   1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
        ft_set_cell_prop(table,                                                                      i + 1,                                   1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      }else
        ft_set_cell_prop(table,                                                                      i + 1,                                   1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GRAY);
      if (vector_size(space->window_ids_v) > 0)
        ft_set_cell_prop(table,                                                                      i + 1,                                   3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GREEN);
      else
        ft_set_cell_prop(table,                                                                      i + 1,                                   3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_YELLOW);
    },
  },
  [TABLE_TYPE_DISPLAY] = &(struct table_logic_t){
    .query_items = get_displays_v,
    .columns     = { DISPLAY_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t __attribute__((unused)) index,    void *item){
      struct display_t *display = (struct display_t *)item;
      ft_printf_ln(table,
                   "%lu"
                   "|%lu"
                   "|%s"
                   "|%d"
                   "|%d"
                   "|%lu"
                   "|%lu"
                   "|%dx%d"
                   "|%dx%d"
                   "%s",
                   display->index,
                   display->display_id,
                   display->is_main ? "Yes" : "No",
                   display->width,
                   display->height,
                   vector_size(display->space_ids_v),
                   vector_size(display->window_ids_v),
                   display->offset_x,                                                                display->offset_y,
                   display->center_x,                                                                display->center_y,
                   "");
    },
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                                void *item){
      struct display_t *display = (struct display_t *)item;
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                   2,                                        FT_CPROP_CONT_FG_COLOR, ((display->is_main == true) ? FT_COLOR_GREEN : FT_COLOR_RED));
      ft_set_cell_prop(table,                                                                        i + 1,                                   5,                                        FT_CPROP_CONT_FG_COLOR, ((vector_size(display->space_ids_v) > 0) ? FT_COLOR_GREEN : FT_COLOR_RED));
      ft_set_cell_prop(table,                                                                        i + 1,                                   6,                                        FT_CPROP_CONT_FG_COLOR, ((vector_size(display->window_ids_v) > 0) ? FT_COLOR_GREEN : FT_COLOR_RED));
    },
  },
  [TABLE_TYPE_HOTKEY] = &(struct table_logic_t){
    .query_items = get_config_keys_v,
    .columns     = { HOTKEY_COLUMNS },
    .row         = ^ void (ft_table_t *table,                                                        size_t __attribute__((unused)) index,    void *item){
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
    .row_style = ^ void (ft_table_t *table,                                                          size_t i,                                void *item){
      struct key_t *hk = (struct key_t *)item;
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
      ft_set_cell_prop(table,                                                                        i + 1,                                   0,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
      ft_set_cell_prop(table,                                                                        i + 1,                                   1,                                        FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
      ft_set_cell_prop(table,                                                                        i + 1,                                   2,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
      ft_set_cell_prop(table,                                                                        i + 1,                                   2,                                        FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table,                                                                        i + 1,                                   3,                                        FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);
      ft_set_cell_prop(table,                                                                        i + 1,                                   4,                                        FT_CPROP_CONT_FG_COLOR, (hk->enabled == true)? FT_COLOR_GREEN : FT_COLOR_RED);
    },
  },
};

#define BREAK_IF_ROW_LIMIT(TABLE, ARGS) \
  if ((int)(args->limit) > 0 && (int)ft_row_count(table) > (int)(args->limit)) break;

#define CONTINUE_IF_ROW_SKIP(SKIP) \
  if (SKIP) {                      \
    filtered_qty++;                \
    continue;                      \
  }

#define TABLE_SUMMARY(ITEM_NAME, ITEM_VECTOR, TABLE, DURATIONS)                                      \
  { do {                                                                                             \
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
        ? " in " : "",                                                                               \
                   DURATIONS[TABLE_DUR_TYPE_FILTER_ROWS].dur > 0                                     \
        ? milliseconds_to_string(DURATIONS[TABLE_DUR_TYPE_FILTER_ROWS].dur) : "",                    \
                   ft_row_count(TABLE) - 1,                                                          \
                   milliseconds_to_string(DURATIONS[TABLE_DUR_TYPE_TOTAL].dur),                      \
                   ""                                                                                \
                   );                                                                                \
      ft_set_cell_span(TABLE, ft_row_count(TABLE) - 1, 0, ft_col_count(TABLE));                      \
      ft_set_cell_prop(TABLE, ft_row_count(TABLE) - 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD); \
      size_t w = 0, h = 0;                                                                           \
      char   *table_s = ft_to_string(table);                                                         \
      fprintf(TABLE_FD, "%s\n", table_s);                                                            \
      ft_destroy_table(table);                                                                       \
      return((strlen(table_s) > 0) ? EXIT_SUCCESS : EXIT_FAILURE);                                   \
    } while (0); }
static bool TABLE_UTILS_DEBUG_MODE = false;

static bool string_compare_skip_row(char *s0, char *s1, bool exact_match, bool case_sensitive){
  if (!s0 || strlen(s0) < 1)
    return(false);

  bool skip_row = false;
  char *s[2];
  asprintf(&s[0], "%s%s%s", exact_match ? "" : "*", stringfn_trim(s0), exact_match ? "" : "*");
  asprintf(&s[1], "%s", stringfn_trim(s1));
  s[0]     = case_sensitive ? s[0] : stringfn_to_lowercase(s[0]);
  s[1]     = case_sensitive ? s[1] : stringfn_to_lowercase(s[1]);
  skip_row = (!wildcardcmp(s[0], s[1]));
  if (s[0])
    free(s[0]);
  if (s[1])
    free(s[1]);
  return(skip_row);
}

#define INIT_TABLE_VARS(TABLE, TYPE, STRUCT_TYPE)              \
  struct list_table_t *args     = (struct list_table_t *)ARGS; \
  FILE                *TABLE_FD = args->fd;                    \
  struct Vector       *items_v;                                \
  struct Vector       *sorted_items = vector_new();            \
  struct STRUCT_TYPE  *item, *_item;                           \
  ft_table_t          *table;                                  \
  size_t              filtered_qty = 0;                        \
  struct table_dur_t  durs[TABLE_DUR_TYPES_QTY];
#define SETUP_TABLE_VARS(TABLE, TYPE, STRUCT_TYPE)           { do {                                                                                                 \
                                                                 durs[TABLE_DUR_TYPE_QUERY_ITEMS].started = timestamp();                                            \
                                                                 items_v                                  = tables[TABLE_TYPE_ ## TYPE]->query_items();             \
                                                                 durs[TABLE_DUR_TYPE_QUERY_ITEMS].dur     = timestamp() - durs[TABLE_DUR_TYPE_QUERY_ITEMS].started; \
                                                                 durs[TABLE_DUR_TYPE_TOTAL].started       = timestamp();                                            \
                                                                 durs[TABLE_DUR_TYPE_TOTAL].dur           = 0;                                                      \
                                                                 durs[TABLE_DUR_TYPE_FILTER_ROWS].dur     = 0;                                                      \
                                                                 TABLE                                    = ft_create_table();                                      \
                                                                 ft_write_ln(TABLE, TYPE ## _COLUMNS);                                                              \
                                                                 ft_set_border_style(TABLE, FT_FRAME_STYLE);                                                        \
                                                                 ft_set_border_style(TABLE, FT_SOLID_ROUND_STYLE);                                                  \
                                                                 ft_set_tbl_prop(TABLE, FT_TPROP_LEFT_MARGIN, 0);                                                   \
                                                                 ft_set_tbl_prop(TABLE, FT_TPROP_RIGHT_MARGIN, 0);                                                  \
                                                                 ft_set_tbl_prop(TABLE, FT_TPROP_TOP_MARGIN, 0);                                                    \
                                                                 ft_set_tbl_prop(TABLE, FT_TPROP_BOTTOM_MARGIN, 0);                                                 \
                                                                 ft_set_cell_prop(TABLE, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);                       \
                                                                 ft_set_cell_prop(TABLE, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);                 \
                                                                 ft_set_cell_prop(TABLE, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);               \
                                                                 ft_set_cell_prop(TABLE, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);                 \
                                                                 ft_set_cell_prop(TABLE, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);                 \
                                                               } while (0); }
#define IF_SORT_FUNCTION_DO_SORT(NAME, TYPE, STRUCT_TYPE)    { do {                                                                                                                \
                                                                 if (TABLE_UTILS_DEBUG_MODE)                                                                                       \
                                                                 log_debug("sort key:%s|dir:%s", args->sort_key, args->sort_direction);                                            \
                                                                 if (args->sort_key && args->sort_direction) {                                                                     \
                                                                   sort_function sort = get_sort_type_function_from_key(SORT_TYPE_ ## TYPE, args->sort_key, args->sort_direction); \
                                                                   if (sort) {                                                                                                     \
                                                                     if (TABLE_UTILS_DEBUG_MODE) {                                                                                 \
                                                                       log_debug("Sorting %s %lu items", NAME, vector_size(items_v));                                              \
                                                                     }                                                                                                             \
                                                                     _item = calloc((vector_size(items_v) + 1), sizeof(struct STRUCT_TYPE));                                       \
                                                                     for (size_t i = 0; i < vector_size(items_v); i++) {                                                           \
                                                                       _item[i] = *(VECTOR_ITEM(items_v, STRUCT_TYPE *, i));                                                       \
                                                                     }                                                                                                             \
                                                                     qsort(_item, vector_size(items_v), sizeof(struct STRUCT_TYPE), sort);                                         \
                                                                     for (size_t i = 0; i < vector_size(items_v); i++) {                                                           \
                                                                       vector_push(sorted_items, (void *)&(_item[i]));                                                             \
                                                                     }                                                                                                             \
                                                                     items_v = sorted_items;                                                                                       \
                                                                   }else{                                                                                                          \
                                                                     if (TABLE_UTILS_DEBUG_MODE) {                                                                                 \
                                                                       log_error("Not Sorting %s %lu items", NAME, vector_size(items_v));                                          \
                                                                     }                                                                                                             \
                                                                   }                                                                                                               \
                                                                 }                                                                                                                 \
                                                               }while (0); }
#define LIST_TABLE(FXN, NAME, TYPE, STRUCT_TYPE)                                                                                                                  \
  int FXN(void *ARGS) {                                                                                                                                           \
    INIT_TABLE_VARS(table, TYPE, STRUCT_TYPE);                                                                                                                    \
    SETUP_TABLE_VARS(table, TYPE, STRUCT_TYPE);                                                                                                                   \
    IF_SORT_FUNCTION_DO_SORT(NAME, TYPE, STRUCT_TYPE);                                                                                                            \
    for (size_t i = 0; i < vector_size(items_v); i++) {                                                                                                           \
      BREAK_IF_ROW_LIMIT(table, args)                                                                                                                             \
      item = VECTOR_ITEM(items_v, TYPE *, i);                                                                                                                     \
      CONTINUE_IF_ROW_SKIP(i < args->offset ? true : tables[TABLE_TYPE_ ## TYPE]->row_skip ? tables[TABLE_TYPE_ ## TYPE]->row_skip(table, i, item, args) : false) \
      tables[TABLE_TYPE_ ## TYPE]->row(table, i, item);                                                                                                           \
      tables[TABLE_TYPE_ ## TYPE]->row_style(table, i, item);                                                                                                     \
    }                                                                                                                                                             \
    TABLE_SUMMARY(NAME, items_v, table, durs);                                                                                                                    \
  }
LIST_TABLE_ITEMS()
#undef LIST_TABLE

////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#endif
