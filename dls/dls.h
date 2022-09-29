#pragma once
#ifndef LS_WIN_H
#define LS_WIN_H
#include "alacritty/alacritty.h"
#include "ansi-codes/ansi-codes.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "capture/window/window.h"
#include "core-utils/core-utils.h"
#include "core-utils/core-utils.h"
#include "display/utils/utils.h"
#include "dls/dls-commands.h"
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
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
enum common_option_width_or_height_t {
  COMMON_OPTION_WIDTH_OR_HEIGHT_UNKNOWN = 0,
  COMMON_OPTION_WIDTH_OR_HEIGHT_HEIGHT,
  COMMON_OPTION_WIDTH_OR_HEIGHT_WIDTH,
  COMMON_OPTION_WIDTH_OR_HEIGHT_QTYS,
};
struct args_t {
  bool                                 verbose, current_space_only, current_display_only;
  int                                  space_id, window_id;
  int                                  x, y;
  int                                  width, height;
  int                                  width_or_height_group;
  int                                  output_mode; char *output_mode_s;
  char                                 *application_path, *output_icns_file, *input_icns_file;
  char                                 *output_png_file, *input_png_file, *input_file, *output_file;
  enum common_option_width_or_height_t width_or_height;
  struct window_info_t                 *window;
  size_t                               icon_size;
  int                                  retries;
  int                                  concurrency, limit;
  double                               resize_factor;
  char                                 *xml_file_path, *content;
  bool                                 clear_icons_cache, minimized_only, non_minimized_only;
  bool                                 display_output_file, all_windows;
  bool                                 random_window_id, clear_screen, compress;
  int                                  pid, display_id, width_greater, width_less, height_greater, height_less;
  char                                 *sort_direction, *application_name;
  char                                 *sort_key, *font_name, *font_family, *font_type, *font_style, *image_format;
  enum image_type_id_t                 image_format_type;
  bool                                 exact_match, case_sensitive, duplicate, non_duplicate;
};
struct args_t *args;
#endif
