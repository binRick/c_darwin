#pragma once
#ifndef LS_WIN_H
#define LS_WIN_H
#include "ansi-codes/ansi-codes.h"
//////////////////////////////////////
#include "alacritty/alacritty.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "core-utils/core-utils.h"
#include "darwin-ls/darwin-ls-commands.h"
#include "display-utils/display-utils.h"
#include "dock-utils/dock-utils.h"
#include "focused/focused.h"
#include "font-utils/font-utils.h"
#include "kitty/kitty.h"
#include "log/log.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "monitor-utils/monitor-utils.h"
#include "ms/ms.h"
#include "optparse99/optparse99.h"
#include "process-utils/process-utils.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
#include "table-utils/table-utils.h"
#include "timestamp/timestamp.h"
#include "usbdevs-utils/usbdevs-utils.h"
#include "window-utils/window-utils.h"
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
char *EXECUTABLE_PATH_DIRNAME, *EXECUTABLE_PATH, *EXECUTABLE, **ARGV, *ASSETS_DIR;
enum common_option_width_or_height_t {
  COMMON_OPTION_WIDTH_OR_HEIGHT_UNKNOWN = 0,
  COMMON_OPTION_WIDTH_OR_HEIGHT_HEIGHT,
  COMMON_OPTION_WIDTH_OR_HEIGHT_WIDTH,
  COMMON_OPTION_WIDTH_OR_HEIGHT_QTYS,
};
char *common_option_width_or_height_name(enum common_option_width_or_height_t width_or_height);
struct args_t {
  bool                                 verbose, current_space_only;
  int                                  space_id, window_id;
  int                                  x, y;
  int                                  width, height;
  int                                  width_or_height_group;
  int                                  output_mode; char *output_mode_s;
  char                                 *application_path, *output_icns_file, *input_icns_file;
  char                                 *output_png_file, *input_png_file, *input_file, *output_file;
  enum common_option_width_or_height_t width_or_height;
  struct window_t                      *window;
  size_t                               icon_size;
  double                               resize_factor;
  char                                 *xml_file_path;
  bool                                 clear_icons_cache;
  int                                  pid;
};
extern struct args_t *args;
#endif
