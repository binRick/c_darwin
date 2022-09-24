#pragma once
#ifndef TABLE_UTILS_C
#define TABLE_UTILS_C
#include "active-app/active-app.h"
#include "app/utils/utils.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "font-utils/font-utils.h"
#include "frameworks/frameworks.h"
#include "hotkey-utils/hotkey-utils.h"
#include "libfort/lib/fort.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process/process.h"
#include "process/utils/utils.h"
#include "process/utils/utils.h"
#include "space/utils/utils.h"
#include "string-utils/string-utils.h"
#include "submodules/log.h/log.h"
#include "table/utils/utils.h"
#include "timestamp/timestamp.h"
#include "wildcardcmp/wildcardcmp.h"
#include "window/info/info.h"
#include "window/sort/sort.h"
///////////////////////////////////////////////////////////////////////////////
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
  if (TABLE_UTILS_DEBUG_MODE) {
    log_debug("%s|%s skip? %s", s[0], s[1], skip_row ? "Yes":"No");
  }
  free(s[0]);
  free(s[1]);
  return(skip_row);
}

int list_installed_fonts_table(void *ARGS) {
  struct list_table_t *args = (struct list_table_t *)ARGS;
  struct table_dur_t  durs[TABLE_DUR_TYPES_QTY];

  durs[TABLE_DUR_TYPE_TOTAL].started = timestamp();
  ft_table_t    *table;
  struct font_t *f;
  struct Vector *v;
  int           term_width;
  {
    term_width                               = get_terminal_width();
    durs[TABLE_DUR_TYPE_QUERY_ITEMS].started = timestamp();
    v                                        = get_installed_fonts_v();
    durs[TABLE_DUR_TYPE_QUERY_ITEMS].dur     = timestamp() - durs[TABLE_DUR_TYPE_QUERY_ITEMS].started;
    table                                    = ft_create_table();
    ft_write_ln(table,
                "ID",
                "Family",
                "Enabled",
                "Size",
                "Type",
                "Style",
                "Faces",
                "Dupe"
                );
    ft_set_border_style(table, FT_FRAME_STYLE);
    ft_set_border_style(table, FT_SOLID_ROUND_STYLE);
    ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_RIGHT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 0);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);
  }

  durs[TABLE_DUR_TYPE_SORT_ROWS].started = timestamp();
  if (args->sort_key && args->sort_direction && get_font_sort_function_from_key(args->sort_key, args->sort_direction)) {
    struct font_t *sorted_fonts  = calloc((vector_size(v) + 1), sizeof(struct font_t));
    struct font_t *_sorted_fonts = vector_new();
    for (size_t i = 0; i < vector_size(v); i++) {
      sorted_fonts[i] = *((struct font_t *)vector_get(v, i));
    }
    qsort(sorted_fonts, vector_size(v), sizeof(struct font_t), get_font_sort_function_from_key(args->sort_key, args->sort_direction));
    for (size_t i = 0; i < vector_size(v); i++) {
      vector_push(_sorted_fonts, (void *)&(sorted_fonts[i]));
    }
    v = _sorted_fonts;
  }
  durs[TABLE_DUR_TYPE_SORT_ROWS].dur = timestamp() - durs[TABLE_DUR_TYPE_SORT_ROWS].started;
  size_t filtered_qty = 0;

  durs[TABLE_DUR_TYPE_FILTER_ROWS].dur = 0;
  for (unsigned long i = 0; i < vector_size(v); i++) {
    if (args->limit >= 0 && (size_t)ft_row_count(table) > (size_t)args->limit) {
      break;
    }
    f = (struct font_t *)vector_get(v, i);
    if (TABLE_UTILS_DEBUG_MODE) {
      debug_font(f);
    }
    durs[TABLE_DUR_TYPE_FILTER_ROWS].started = timestamp();
    if ((string_compare_skip_row(args->font_family, f->family, args->exact_match, args->case_sensitive))
        || (string_compare_skip_row(args->font_name, f->name, args->exact_match, args->case_sensitive))
        || (string_compare_skip_row(args->font_style, f->style, args->exact_match, args->case_sensitive))
        || (string_compare_skip_row(args->font_type, f->type, args->exact_match, args->case_sensitive))
        || (args->duplicate && !f->duplicate)
        || (args->non_duplicate && f->duplicate)
        ) {
      filtered_qty++;
      continue;
    }
    durs[TABLE_DUR_TYPE_FILTER_ROWS].dur += timestamp() - durs[TABLE_DUR_TYPE_FILTER_ROWS].started;
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
                 i + 1,
                 (int)((float)term_width * .40), stringfn_trim(f->family),
                 (int)((float)term_width * .5), f->enabled ? "Yes":"No",
                 (int)((float)term_width * .8), bytes_to_string(f->size),
                 (int)((float)term_width * .10), stringfn_trim(f->type),
                 (int)((float)term_width * .10), stringfn_trim(f->style),
                 f->typefaces_qty,
                 (int)((float)term_width * .5), f->duplicate ? "Yes":"No",
                 ""
                 );

    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_FG_COLOR, (f->enabled == true)? FT_COLOR_GREEN : FT_COLOR_RED);

    ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, f->size > 5 * 1024 * 1024 ? FT_COLOR_RED : f->size > 1024 * 1024 ? FT_COLOR_BLUE : FT_COLOR_LIGHT_BLUE);

    ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
    ft_set_cell_prop(table, i + 1, 4, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
    ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);

    ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    ft_set_cell_prop(table, i + 1, 5, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_BLUE);
    ft_set_cell_prop(table, i + 1, 6, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_FG_COLOR, f->duplicate ? FT_COLOR_RED : FT_COLOR_GREEN);
    ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_TEXT_STYLE, f->duplicate ? (FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED) : FT_TSTYLE_DEFAULT);
  }

  ft_add_separator(table);
  durs[TABLE_DUR_TYPE_TOTAL].dur = timestamp() - durs[TABLE_DUR_TYPE_TOTAL].started;
  ft_printf_ln(table,
               "Queried %lu %s in %s, filtered %lu items%s%s, and rendered %lu rows in %s"
               "%s",
               vector_size(v),
               "Fonts",
               milliseconds_to_string(durs[TABLE_DUR_TYPE_QUERY_ITEMS].dur),
               filtered_qty,
               durs[TABLE_DUR_TYPE_FILTER_ROWS].dur > 0
        ? " in " : "",
               durs[TABLE_DUR_TYPE_FILTER_ROWS].dur > 0
        ? milliseconds_to_string(durs[TABLE_DUR_TYPE_FILTER_ROWS].dur) : "",
               ft_row_count(table) - 1,
               milliseconds_to_string(durs[TABLE_DUR_TYPE_TOTAL].dur),
               ""
               );
  ft_set_cell_span(table, ft_row_count(table) - 1, 0, ft_col_count(table));
  ft_set_cell_prop(table, ft_row_count(table) - 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
  char *table_s = ft_to_string(table);

  fprintf(stdout, "%s\n", table_s);
  ft_destroy_table(table);
  return(EXIT_SUCCESS);
} /* list_installed_fonts_table */

int list_installed_apps_table(void *ARGS) {
  struct list_table_t *args = (struct list_table_t *)ARGS;
  ft_table_t          *table;
  struct app_t        *app;
  struct Vector       *v;
  int                 term_width;
  {
    term_width = get_terminal_width();
    v          = get_installed_apps_v();
    table      = ft_create_table();
    ft_write_ln(table,
                "ID",
                "Name",
                "Version",
                "Path"
                );
    ft_set_border_style(table, FT_FRAME_STYLE);
    ft_set_border_style(table, FT_SOLID_ROUND_STYLE);
    ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_RIGHT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 0);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);
  }
  char *s[2];

  for (unsigned long i = 0; i < vector_size(v); i++) {
    if (args->limit >= 0 && (size_t)ft_row_count(table) > (size_t)args->limit) {
      break;
    }
    app = (struct app_t *)vector_get(v, i);
    if (args->application_name != NULL) {
      bool skip_row = false;
      asprintf(&s[0], "%s%s%s", "*", stringfn_to_lowercase(args->application_name), "*");
      asprintf(&s[1], "%s", stringfn_to_lowercase(app->name));
      if (wildcardcmp(s[0], s[1]) == 0) {
        skip_row = true;
      }
      if (skip_row == true) {
        continue;
      }
      if (s[0]) {
        free(s[0]);
      }
      if (s[1]) {
        free(s[1]);
      }
    }
    ft_printf_ln(table,
                 "%ld"
                 "|%.*s"
                 "|%.*s"
                 "|%.*s"
                 "%s",
                 i + 1,
                 (int)((float)term_width * .15), app->name,
                 (int)((float)term_width * .10), app->version,
                 (int)((float)term_width * .50), app->path,
                 ""
                 );

    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);

    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);

    ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);
  }
  char *table_s = ft_to_string(table);

  fprintf(stdout, "%s\n", table_s);
  ft_destroy_table(table);
  return(EXIT_SUCCESS);
} /* list_installed_apps_table */

int list_hotkeys_table(void *ARGS) {
  struct list_table_t     *args = (struct list_table_t *)ARGS;
  ft_table_t              *table;
  struct hotkeys_config_t *cfg;
  struct key_t            *hk;
  {
    cfg   = load_yaml_config_file_path(get_homedir_yaml_config_file_path());
    table = ft_create_table();
    ft_write_ln(table,
                "ID",
                "Name",
                "Key",
                "Action",
                "Enabled"
                );
    ft_set_border_style(table, FT_FRAME_STYLE);
    ft_set_border_style(table, FT_SOLID_ROUND_STYLE);
    ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_RIGHT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 0);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);
  }

  for (unsigned long i = 0; i < cfg->keys_count; i++) {
    if (args->limit >= 0 && (size_t)ft_row_count(table) > (size_t)args->limit) {
      break;
    }
    hk = &(cfg->keys[i]);
    ft_printf_ln(table,
                 "%ld"
                 "|%s"
                 "|%.45s"
                 "|%s"
                 "|%s"
                 "%s",
                 i + 1,
                 (strlen(hk->name) > 0) ? hk->name : get_hotkey_type_action_name(hk->action_type),
                 hk->key,
                 (hk->action != NULL && strlen(hk->action) > 0 && strcmp(hk->action, "null") != 0) ? hk->action : "",
                 hk->enabled ? "Yes" : "No",
                 ""

                 );

    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);

    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);

    ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);

    ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, (hk->enabled == true)? FT_COLOR_GREEN : FT_COLOR_RED);
  }
  char *table_s = ft_to_string(table);

  fprintf(stdout, "%s\n", table_s);
  ft_destroy_table(table);
  return(EXIT_SUCCESS);
} /* list_hotkeys_table */

int list_window_infos_table(void *ARGS) {
  struct list_table_t  *args = (struct list_table_t *)ARGS;
  struct Vector        *window_infos_v;
  struct window_info_t *w;
  struct table_dur_t   durs[TABLE_DUR_TYPES_QTY];
  int                  term_width = get_terminal_width();
  size_t               cur_display_id, cur_space_id;
  ft_table_t           *table;

  durs[TABLE_DUR_TYPE_TOTAL].started = timestamp();
  for (int o = 0; o < TABLE_DUR_TYPES_QTY; o++) {
    durs[o].dur = 0;
  }
  {
    unsigned long started = timestamp();
    unsigned long s0;
    s0                                          = timestamp();
    window_infos_v                              = get_window_infos_v();
    durs[TABLE_DUR_TYPE_QUERY_WINDOWS].dur     += (timestamp() - s0);
    s0                                          = timestamp();
    cur_space_id                                = get_current_space_id();
    durs[TABLE_DUR_TYPE_QUERY_SPACES].dur      += (timestamp() - s0);
    s0                                          = timestamp();
    cur_display_id                              = get_display_id_for_space(cur_space_id);
    s0                                          = timestamp();
    durs[TABLE_DUR_TYPE_QUERY_CUR_DISPLAY].dur += (timestamp() - s0);
    durs[TABLE_DUR_TYPE_QUERIES].dur           += (timestamp() - started);
  }
  {
    for (size_t i = 0; i < vector_size(window_infos_v); i++) {
      struct window_info_t *w = ((struct window_info_t *)vector_get(window_infos_v, i));
      if (w->rect.size.width <= 1 && w->rect.size.height <= 1) {
        vector_remove(window_infos_v, i);
      }
    }
  }

  if (args->sort_key && args->sort_direction && get_window_sort_function_from_key(args->sort_key, args->sort_direction)) {
    struct window_info_t *sorted_window_infos  = calloc((vector_size(window_infos_v) + 1), sizeof(struct window_info_t));
    struct window_info_t *_sorted_window_infos = vector_new();
    for (size_t i = 0; i < vector_size(window_infos_v); i++) {
      sorted_window_infos[i] = *((struct window_info_t *)vector_get(window_infos_v, i));
    }
    qsort(sorted_window_infos, vector_size(window_infos_v), sizeof(struct window_info_t), get_window_sort_function_from_key(args->sort_key, args->sort_direction));
    for (size_t i = 0; i < vector_size(window_infos_v); i++) {
      vector_push(_sorted_window_infos, (void *)&(sorted_window_infos[i]));
    }
    window_infos_v = _sorted_window_infos;
  }

  {
    unsigned long started = timestamp();
    table = ft_create_table();
    ft_set_border_style(table, FT_FRAME_STYLE);
    ft_set_border_style(table, FT_SOLID_ROUND_STYLE);
    ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_RIGHT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 0);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);
    durs[TABLE_DUR_TYPE_FORT].dur += (timestamp() - started);
  }

  ft_write_ln(table,
              "ID",
              "PID",
              "Application",
              "Size",
              "Position",
              "Space",
              "Disp",
              "Min",
              "Dur"
              );
  int max_name_len = 0;

  for (size_t i = 0; i < vector_size(window_infos_v); i++) {
    w            = (struct window_info_t *)vector_get(window_infos_v, i);
    max_name_len = (strlen(w->name) > (size_t)max_name_len) ? (int)strlen(w->name) : max_name_len;
  }
  while (term_width > 0 && max_name_len > (term_width / 4)) {
    max_name_len--;
  }
  for (size_t i = 0; i < vector_size(window_infos_v); i++) {
    if (TABLE_UTILS_DEBUG_MODE) {
      for (int o = 0; o < WINDOW_INFO_DUR_TYPES_QTY; o++) {
        log_debug("Window Duration> %s %s: %s",
                  w->name,
                  window_info_dur_type_names[o],
                  milliseconds_to_string(w->durs[o].dur)
                  );
      }
    }
    w = (struct window_info_t *)vector_get(window_infos_v, i);
    if (args->limit >= 0 && (size_t)ft_row_count(table) > (size_t)args->limit) {
      break;
    }
    if (args->width >= 0 && (int)w->rect.size.width != args->width) {
      continue;
    }
    if (args->height >= 0 && (int)w->rect.size.height != args->height) {
      continue;
    }
    if (args->height_greater >= 0 && (int)w->rect.size.height <= args->height_greater) {
      continue;
    }
    if (args->height_less >= 0 && (int)w->rect.size.height >= args->height_less) {
      continue;
    }
    if (args->width_greater >= 0 && (int)w->rect.size.width <= args->width_greater) {
      continue;
    }
    if (args->width_less >= 0 && (int)w->rect.size.width >= args->width_less) {
      continue;
    }
    if (args->pid > 0 && w->pid != args->pid) {
      continue;
    }
    if (args->window_id > 0 && w->window_id != args->window_id) {
      continue;
    }
    if ((args->display_id > 0 && (((size_t)(w->display_id) != (size_t)(args->display_id)) && ((size_t)(args->display_id) != (size_t)(get_display_id_index(w->display_id)))))) {
      continue;
    }
    if (args->space_id > 0 && w->space_id != args->space_id) {
      continue;
    }
    if (args->current_display_only == true && w->space_id != cur_space_id) {
      continue;
    }
    if (args->current_space_only == true && w->space_id != cur_space_id) {
      continue;
    }
    if (args->application_name != NULL) {
      bool skip_row = false;
      char *s[2];
      asprintf(&s[0], "%s%s%s", "*", stringfn_to_lowercase(args->application_name), "*");
      asprintf(&s[1], "%s", stringfn_to_lowercase(w->name));

      if (wildcardcmp(s[0], s[1]) == 1) {
      }else{
        skip_row = true;
      }
      free(s[0]);
      free(s[1]);
      if (skip_row == true) {
        continue;
      }
    }
    ft_printf_ln(table,
                 "%lu|%d|%.*s|%dx%d|%dx%d|%lu|%d|%s|%s"
                 "%s",
                 w->window_id,
                 w->pid,
                 max_name_len, w->name,
                 (int)w->rect.size.width, (int)w->rect.size.height,
                 (int)w->rect.origin.x, (int)w->rect.origin.y,
                 w->space_id,
                 get_display_id_index(w->display_id),
                 (w->is_minimized == true) ? "Yes" : "No",
                 milliseconds_to_string(w->dur),
                 ""
                 );

    unsigned long colors_started = timestamp();
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);

    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);

    if (w->rect.size.width < 2 || w->rect.size.height < 2) {
      ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_BLUE);
    }else if (w->rect.size.width == 0 && w->rect.size.height == 0) {
      ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_YELLOW);
    }else{
      ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
    }

    if ((int)w->rect.origin.x < 0 || (int)w->rect.origin.y < 0) {
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
    }else{
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GREEN);
    }
    if (w->space_id == cur_space_id) {
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
    }else{
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_RED);
    }
    if (w->display_id == cur_display_id) {
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_BLUE);
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    }else{
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);
    }
    if (w->is_minimized == true) {
      ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
      ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    }else{
      ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }
    if (w->dur == 0) {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else if (w->dur > 20) {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }else if (w->dur > 10) {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
    }else {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_MAGENTA);
    }
    durs[TABLE_DUR_TYPE_COLORS].dur += (timestamp() - colors_started);
  }
  char *table_s = ft_to_string(table);

  printf("%s\n", table_s);

  durs[TABLE_DUR_TYPE_TOTAL].dur = timestamp() - durs[TABLE_DUR_TYPE_TOTAL].started;
  if (TABLE_UTILS_DEBUG_MODE == true) {
    log_info("Rendered %s %lu row, %lu column table from %lu windows in %s",
             bytes_to_string(strlen(table_s)),
             ft_row_count(table),
             ft_col_count(table),
             vector_size(window_infos_v),
             milliseconds_to_string(durs[TABLE_DUR_TYPE_TOTAL].dur)
             );
    for (int o = 0; o < TABLE_DUR_TYPES_QTY; o++) {
      log_debug("Table Duration> %s %s: %s",
                w->name,
                table_dur_type_names[o],
                milliseconds_to_string(durs[o].dur)
                );
    }
  }
  ft_destroy_table(table);
} /* list_window_infos_table */

int list_displays_table(void *ARGS) {
  struct list_table_t *args                       = (struct list_table_t *)ARGS;
  unsigned long       list_displays_table_started = timestamp();
  struct Vector       *displays_v                 = get_displays_v();
  ft_table_t          *table                      = ft_create_table();
  {
    ft_set_border_style(table, FT_FRAME_STYLE);
    ft_set_border_style(table, FT_SOLID_ROUND_STYLE);
    ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_RIGHT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 0);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);

    ft_write_ln(table,
                "Index",
                "ID",
                "Main",
                "Width", "Height",
                "# Spaces",
                "# Windows"
                );
  }

  for (size_t i = 0; i < vector_size(displays_v); i++) {
    if (args->limit >= 0 && (size_t)ft_row_count(table) > (size_t)args->limit) {
      break;
    }
    struct display_t *display = (struct display_t *)vector_get(displays_v, i);
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
                 ""

                 );

    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_FG_COLOR, ((display->is_main == true) ? FT_COLOR_GREEN : FT_COLOR_RED));
    ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, ((vector_size(display->space_ids_v) > 0) ? FT_COLOR_GREEN : FT_COLOR_RED));
    ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, ((vector_size(display->window_ids_v) > 0) ? FT_COLOR_GREEN : FT_COLOR_RED));
  }
  char *table_s = ft_to_string(table);

  fprintf(stdout,
          "" AC_YELLOW AC_BOLD AC_UNDERLINE "%lu Spaces" AC_RESETALL
          "\n",
          vector_size(displays_v)
          );
  fprintf(stdout, "%s\n", table_s);
  unsigned long list_displays_table_dur = timestamp() - list_displays_table_started;

  if (TABLE_UTILS_DEBUG_MODE == true) {
    log_info("Rendered %s %lu row, %lu column table from %lu spaces in %s",
             bytes_to_string(strlen(table_s)),
             ft_row_count(table),
             ft_col_count(table),
             vector_size(displays_v),
             milliseconds_to_string(list_displays_table_dur)
             );
    ft_destroy_table(table);
  }
  return(0);
} /* list_displays_table */

int list_spaces_table(void *ARGS) {
  struct list_table_t *args                     = (struct list_table_t *)ARGS;
  unsigned long       list_spaces_table_started = timestamp();
  int                 terminal_width            = get_terminal_width();
  struct Vector       *spaces_v                 = get_spaces_v();
  ft_table_t          *table                    = ft_create_table();

  {
    ft_set_border_style(table, FT_FRAME_STYLE);
    ft_set_border_style(table, FT_SOLID_ROUND_STYLE);
    ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_RIGHT_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
    ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 0);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);

    ft_write_ln(table,
                "ID",
                "Active",
                "Windows",
                "#"
                );
  }

  for (size_t i = 0; i < vector_size(spaces_v); i++) {
    if (args->limit >= 0 && (size_t)ft_row_count(table) > (size_t)args->limit) {
      break;
    }
    struct space_t *space = (struct space_t *)vector_get(spaces_v, i);
    ft_printf_ln(table,
                 "%d"
                 "|%s"
                 "|%s"
                 "|%lu"
                 "%s",
                 space->id,
                 space->is_current ? "Yes" : "No",
                 vector_size_ts_to_csv(space->window_ids_v, terminal_width / 5),
                 vector_size(space->window_ids_v),
                 ""

                 );

    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
    if (space->is_current == true) {
      ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
      ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD | FT_TSTYLE_INVERTED);
    }else{
      ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GRAY);
    }
    if (vector_size(space->window_ids_v) > 0) {
      ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_YELLOW);
    }
  }
  char *table_s = ft_to_string(table);

  fprintf(stdout,
          "" AC_YELLOW AC_BOLD AC_UNDERLINE "%lu Spaces" AC_RESETALL
          "\n",
          vector_size(spaces_v)
          );
  fprintf(stdout, "%s\n", table_s);
  unsigned long list_spaces_table_dur = timestamp() - list_spaces_table_started;

  if (TABLE_UTILS_DEBUG_MODE == true) {
    log_info("Rendered %s %lu row, %lu column table from %lu spaces in %s",
             bytes_to_string(strlen(table_s)),
             ft_row_count(table),
             ft_col_count(table),
             vector_size(spaces_v),
             milliseconds_to_string(list_spaces_table_dur)
             );
  }
  ft_destroy_table(table);

  return(0);
} /* list_spaces_table */
static void __attribute__((constructor)) __constructor__table_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_TABLE_UTILS") != NULL) {
    log_debug("Enabling Table Utils Debug Mode");
    TABLE_UTILS_DEBUG_MODE = true;
  }
}

#endif