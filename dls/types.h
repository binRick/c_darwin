#pragma once
#ifndef DLS_TYPE_H
#define DLS_TYPE_H
#include "capture/type/type.h"
enum common_option_capture_type_t {
  COMMON_OPTION_CAPTURE_TYPE_WINDOW = 0,
  COMMON_OPTION_CAPTURE_TYPE_DISPLAY,
  COMMON_OPTION_CAPTURE_TYPE_SPACE,
  COMMON_OPTION_CAPTURE_TYPE_PID,
  COMMON_OPTION_CAPTURE_TYPE_APPLICATION_NAME,
  COMMON_OPTION_CAPTURE_TYPES_QTY,
};
enum common_option_width_or_height_t {
  COMMON_OPTION_WIDTH_OR_HEIGHT_UNKNOWN = 0,
  COMMON_OPTION_WIDTH_OR_HEIGHT_HEIGHT,
  COMMON_OPTION_WIDTH_OR_HEIGHT_WIDTH,
  COMMON_OPTION_WIDTH_OR_HEIGHT_QTYS,
};
enum output_mode_type_t {
  OUTPUT_MODE_TEXT = 1,
  OUTPUT_MODE_TABLE,
  OUTPUT_MODE_JSON,
  OUTPUT_MODES_QTY,
};
enum dls_window_slide_direction_t {
  DLS_WINDOW_SLIDE_LEFT,
  DLS_WINDOW_SLIDE_RIGHT,
  DLS_WINDOW_SLIDE_TOP,
  DLS_WINDOW_SLIDE_BOTTOM,
  DLS_WINDOW_SLIDES_QTY,
};
static const char *dls_window_slide_direction_names[] = {
  [DLS_WINDOW_SLIDE_LEFT]="left",
  [DLS_WINDOW_SLIDE_RIGHT]="right",
  [DLS_WINDOW_SLIDE_TOP]="top",
  [DLS_WINDOW_SLIDE_BOTTOM]="bottom",
};

struct args_t {
  enum capture_type_id_t               capture_type;
  enum common_option_width_or_height_t width_or_height;
  struct window_info_t                 *window;
  struct Vector                        *formats_v, *format_ids_v;
  int                                  space_id, id, display_id, x, y, width, height, width_or_height_group, output_mode, retries, concurrency, limit, pid, width_greater, width_less, height_greater, height_less, frame_rate, duration_seconds, index;
  size_t                               offset;
  FILE                                 *fd;
  float                                layout_size; int layout_qty;
  float slide_percentage;
  float slide_duration;
  char *slide_direction_name;
  enum dls_window_slide_direction_t slide_direction;
  char                                 *windowids;
  char                                 **format_names, **hide_columns, **show_columns, **db_tables, **search_words, **prompt_commands, **globs;
  size_t                               format_names_qty, hide_columns_qty, show_columns_qty, db_tables_qty, search_words_qty, prompt_commands_qty, globs_qty;
  struct Vector                        *icon_sizes_v;
  double                               resize_factor;
  double                               display_size_percentage;
  int window_id;
  char *directory;
  bool current_id_mode;

  bool                                 display_sdl_mode, display_mode, all_mode, dls_clear_screen, compress, exact_match, case_sensitive, duplicate, non_duplicate, write_file, write_thumbnail, purge_write_directory_before_write, random_ids_mode, clear_icons_cache, minimized_only, not_minimized_only, verbose_mode, debug_mode, progress_bar_mode, write_images_mode, capture_mode[999], capture_window_mode, capture_space_mode, capture_display_mode, current_space_only, current_display_only, not_current_display_only, not_current_space_only, grayscale_mode, quantize_mode, db_save_mode, fullscreen_mode, list_tests_mode, list_suites_mode, abort_tests_mode, quiet_mode;
  char                                 *sort_key, *font_name, *font_family, *font_type, *font_style, *formats, *sort_direction, *application_name, *write_directory, *application_path, *output_icns_file, *input_icns_file, *output_png_file, *input_png_file, *input_file, *output_file, *output_mode_s, *xml_file_path, *content, *sizes, *input_gif_file, *layout_name;
};
//////////////////////////////////////

#endif
