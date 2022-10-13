#pragma once
#ifndef LS_WIN_H
#define LS_WIN_H
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
enum common_option_capture_type_t {
  COMMON_OPTION_CAPTURE_TYPE_WINDOW = 0,
  COMMON_OPTION_CAPTURE_TYPE_DISPLAY,
  COMMON_OPTION_CAPTURE_TYPE_SPACE,
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
const enum output_mode_type_t DEFAULT_OUTPUT_MODE;
const char                    *output_modes[OUTPUT_MODES_QTY + 1];
#include "core/utils/utils.h"
#include "dls/dls-aliases.h"
#include "dls/dls-commands.h"
#include "image/utils/utils.h"
#include "capture/type/type.h"
#include "capture/animate/animate.h"
struct args_t {
  enum capture_type_id_t capture_type;
  enum common_option_width_or_height_t width_or_height;
  struct window_info_t                 *window;
  struct Vector *formats_v, *format_ids_v;
  int                                  space_id, id, display_id, x, y, width, height, width_or_height_group, output_mode, retries, concurrency, limit, pid, width_greater, width_less, height_greater, height_less, frame_rate, duration_seconds;
  size_t                               icon_size;
  double                               resize_factor;
  bool                                 display_mode, all_mode, clear_screen, compress, exact_match, case_sensitive, duplicate, non_duplicate, write_file, write_thumbnail, purge_write_directory_before_write, random_ids_mode, clear_icons_cache, minimized_only, not_minimized_only, verbose_mode, debug_mode, progress_bar_mode, write_images_mode, capture_mode[999], capture_window_mode, capture_space_mode, capture_display_mode, current_space_only, current_display_only, not_current_display_only, not_current_space_only, grayscale_mode;
  char                                 *sort_key, *font_name, *font_family, *font_type, *font_style, *formats, *sort_direction, *application_name, *write_directory, *application_path, *output_icns_file, *input_icns_file, *output_png_file, *input_png_file, *input_file, *output_file, *output_mode_s, *xml_file_path, *content;
};
struct args_t *args;
#include "alacritty/alacritty.h"
#include "ansi-codes/ansi-codes.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "core/utils/utils.h"
#include "display/utils/utils.h"
#include "dock/utils/utils.h"
#include "focused/focused.h"
#include "font-utils/font-utils.h"
#include "kitty/kitty.h"
#include "log/log.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "monitor/utils/utils.h"
#include "ms/ms.h"
#include "optparse99/optparse99.h"
#include "process/utils/utils.h"
#include "space/utils/utils.h"
#include "string-utils/string-utils.h"
#include "table/utils/utils.h"
#include "timestamp/timestamp.h"
#include "usbdevs-utils/usbdevs-utils.h"
#include "window/utils/utils.h"
//////////////////////////////////////
struct whereami_report_t *whereami;
#endif
