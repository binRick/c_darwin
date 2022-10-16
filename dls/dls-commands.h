#pragma once
#ifndef LS_WIN_COMMANDS_H
#define LS_WIN_COMMANDS_H
#include "dls/dls.h"
////////////////////////////////////////////
#define ICON_LIST                     "💈"
#define ICON_SHOW                     "🐝"
#define ICON_TEST                     "🔔"
#define ICON_RENDER                   "🍺"
#define ICON_APPLY                    "🧢"
#define ICON_LAYOUT                   "💥"
#define ICON_INIT                     "🚛"
#define ICON_LOAD                     "🚛"
#define ICON_TABLE                    "🚛"
#define ICON_ICON                     "🚛"
#define ICON_WINDOW                   "🚛"
#define ICON_MOVE                     "🚛"
#define ICON_STICKY                   "🚛"
#define ICON_UNSTICKY                 "🚛"
#define ICON_RESIZE                   "🚛"
#define ICON_INFO                     "🚛"
#define ICON_ROW                      "🚛"
#define ICON_IDS                      "🚛"
#define ICON_SERVER                   "🚛"
#define ICON_DB                       "🔋"
#define ICON_CAPTURE                  "🔋"
#define ICON_SPACE                    "🔋"
#define ICON_DISPLAY                  "🔋"
#define ICON_EXTRACT                  "🔋"
#define COLOR_CAPTURE_MODE            "\x1b[38;2;50;252;142m" AC_BOLD AC_CURLY_UNDERLINE
#define COLOR_CAPTURE_WINDOW_MODE     "\x1b[38;2;151;252;50m" AC_ITALIC
#define COLOR_CAPTURE_DISPLAY_MODE    "\x1b[38;2;97;50;50m" AC_ITALIC
#define COLOR_CAPTURE_SPACE_MODE      "\x1b[38;2;189;163;50m" AC_ITALIC
#define COLOR_FOCUS                   "\x1b[38;2;151;252;50m"
#define COLOR_ID                      "\x1b[38;2;162;194;208m"
#define COLOR_CAPTURE                 "\x1b[38;2;162;194;208m"
#define COLOR_INIT                    "\x1b[38;2;162;194;208m"
#define COLOR_LOAD                    "\x1b[38;2;0;255;34m"
#define COLOR_TABLE                   "\x1b[38;2;126;94;82m"
#define COLOR_ROW                     "\x1b[38;2;80;100;127m"
#define COLOR_SERVER                  "\x1b[38;2;12;255;12m"
#define COLOR_LIST                    "\x1b[38;2;50;175;252m"
#define COLOR_TEST                    "\x1b[38;2;243;233;217m"
#define COLOR_SHOW                    "\x1b[38;2;25;25;112m"
#define COLOR_APPLY                   "\x1b[38;2;153;0;102m"
#define COLOR_LAYOUT                  "\x1b[38;2;211;221;228m"
#define COLOR_RENDER                  "\x1b[38;2;0;130;161m"
#define COLOR_ICON                    "\x1b[38;2;0;251;255m"
#define COLOR_DISPLAY                 "\x1b[38;2;0;251;255m"
#define COLOR_SPACE                   "\x1b[38;2;97;252;50m"
#define COLOR_INFO                    "\x1b[38;2;189;50;252m"
#define COLOR_DEBUG                   "\x1b[38;2;252;50;127m"
#define COLOR_WINDOW                  "\x1b[38;2;252;163;50m"
#define COLOR_MOVE                    "\x1b[38;2;252;83;50m"
#define COLOR_START                   "\x1b[38;2;62;252;50m"
#define COLOR_GET                     "\x1b[38;2;252;50;252m"
#define COLOR_SET                     "\x1b[38;2;252;163;252m"
#define COLOR_HELP                    "\x1b[38;2;50;50;252m"
#define COLOR_ICNS                    "\x1b[38;2;252;252;252m"
typedef struct optparse_opt (^common_option_b)(struct args_t *args);
typedef char (^common_option_description)(void *sorts);
enum common_option_group_t {
  COMMON_OPTION_GROUP_WIDTH_OR_HEIGHT = 1,
  COMMON_OPTION_GROUP_PROGRESS_BAR_MODE,
  COMMON_OPTION_GROUP_CAPTURE_MODE,
  COMMON_OPTION_GROUP_ID,
  COMMON_OPTION_GROUP_DISPLAY_SIZE_ROWS_OR_COLS_OR_PERCENT_OR_PIXELS,
  COMMON_OPTION_GROUP_DISPLAY_CORNER_TOP_LEFT_OR_TOP_RIGHT_OR_BOTTOM_RIGHT_OR_BOTTOM_LEFT,
  COMMON_OPTION_GROUP_RESIZE_HEIGHT_OR_WIDTH,
  COMMON_OPTION_GROUP_RESIZE_ROWS_OR_COLS_OR_PERCENT_OR_PIXELS,
  COMMON_OPTION_GROUP_TITLE_FILENAME_OR_FILENAME_AND_SIZE_OR_SIZE_GROUP,
  COMMON_OPTION_GROUPS_QTY,
};
enum check_command_type_t {
  CHECK_COMMAND_ID = 1,
  CHECK_COMMAND_WIDTH,
  CHECK_COMMAND_CAPTURE_WINDOW_MODE,
  CHECK_COMMAND_CAPTURE_SPACE_MODE,
  CHECK_COMMAND_CAPTURE_DISPLAY_MODE,
  CHECK_COMMAND_RETRIES,
  CHECK_COMMAND_HEIGHT,
  CHECK_COMMAND_WIDTH_GROUP,
  CHECK_COMMAND_HEIGHT_GROUP,
  CHECK_COMMAND_TITLE_FILENAME_GROUP,
  CHECK_COMMAND_TITLE_SIZE_GROUP,
  CHECK_COMMAND_TITLE_FORMAT_GROUP,
  CHECK_COMMAND_TITLE_FILENAME_AND_SIZE_GROUP,
  CHECK_COMMAND_TITLE_FILENAME_AND_FORMAT_GROUP,
  CHECK_COMMAND_TITLE_FILENAME_AND_SIZE_AND_FORMAT_GROUP,
  CHECK_COMMAND_RESIZE_HEIGHT_GROUP,
  CHECK_COMMAND_RESIZE_WIDTH_GROUP,
  CHECK_COMMAND_DISPLAY_SIZE_ROWS_GROUP,
  CHECK_COMMAND_DISPLAY_SIZE_COLS_GROUP,
  CHECK_COMMAND_DISPLAY_SIZE_PERCENT_GROUP,
  CHECK_COMMAND_DISPLAY_SIZE_PIXELS_GROUP,
  CHECK_COMMAND_DISPLAY_TOP_LEFT_CORNER_GROUP,
  CHECK_COMMAND_DISPLAY_TOP_RIGHT_CORNER_GROUP,
  CHECK_COMMAND_DISPLAY_BOTTOM_RIGHT_CORNER_GROUP,
  CHECK_COMMAND_DISPLAY_BOTTOM_LEFT_CORNER_GROUP,
  CHECK_COMMAND_OUTPUT_MODE,
  CHECK_COMMAND_DISPLAY_OUTPUT_FILE,
  CHECK_COMMAND_WRITE_DIRECTORY,
  CHECK_COMMAND_WRITE_IMAGES_MODE,
  CHECK_COMMAND_PURGE_WRITE_DIRECTORY,
  CHECK_COMMAND_OUTPUT_FILE,
  CHECK_COMMAND_INPUT_FILE,
  CHECK_COMMAND_OUTPUT_ICNS_FILE,
  CHECK_COMMAND_IMAGE_FORMATS,
  CHECK_COMMAND_INPUT_ICNS_FILE,
  CHECK_COMMAND_INPUT_PNG_FILE,
  CHECK_COMMAND_OUTPUT_PNG_FILE,
  CHECK_COMMAND_APPLICATION_PATH,
  CHECK_COMMAND_APPLICATION_NAME,
  CHECK_COMMAND_ICON_SIZES,
  CHECK_COMMAND_RESIZE_FACTOR,
  CHECK_COMMAND_XML_FILE,
  CHECK_COMMAND_CONTENT,
  CHECK_COMMAND_RANDOM_ID,
  CHECK_COMMAND_HEIGHT_LESS,
  CHECK_COMMAND_HEIGHT_GREATER,
  CHECK_COMMAND_RUN_HOTKEYS,
  CHECK_COMMAND_WIDTH_LESS,
  CHECK_COMMAND_WIDTH_GREATER,
  CHECK_COMMAND_PID,
  CHECK_COMMAND_SORT_DIRECTION_DESC,
  CHECK_COMMAND_SORT_DIRECTION_ASC,
  CHECK_COMMAND_PROGRESS_BAR_MODE,
  CHECK_COMMAND_DISABLE_PROGRESS_BAR_MODE,
  CHECK_COMMAND_ENABLE_PROGRESS_BAR_MODE,
  CHECK_COMMAND_SORT_KEY,
  CHECK_COMMAND_MINIMIZED_ONLY,
  CHECK_COMMAND_NOT_MINIMIZED_ONLY,
  CHECK_COMMAND_CLEAR_SCREEN,
  CHECK_COMMAND_CONCURRENCY,
  CHECK_COMMAND_WINDOW_IDS,
  CHECK_COMMAND_LIMIT,
  CHECK_COMMAND_TYPES_QTY,
};
enum common_option_name_t {
  COMMON_OPTION_SPACE_ID = 1,
  COMMON_OPTION_DISPLAY_ID,
  COMMON_OPTION_WINDOW_IDS,
  COMMON_OPTION_CAPTURE_WINDOW_MODE,
  COMMON_OPTION_CAPTURE_SPACE_MODE,
  COMMON_OPTION_CAPTURE_DISPLAY_MODE,
  COMMON_OPTION_PURGE_WRITE_DIRECTORY_BEFORE_WRITE,
  COMMON_OPTION_HELP_SUBCMD,
  COMMON_OPTION_ID,
  COMMON_OPTION_IMAGE_FORMATS,
  COMMON_OPTION_CLEAR_SCREEN,
  COMMON_OPTION_HIDE_COLUMNS,
  COMMON_OPTION_SHOW_COLUMNS,
  COMMON_OPTION_COMPRESS,
  COMMON_OPTION_DISABLE_PROGRESS_BAR_MODE,
  COMMON_OPTION_QUANTIZE_MODE,
  COMMON_OPTION_ENABLE_PROGRESS_BAR_MODE,
  COMMON_OPTION_CONTENT,
  COMMON_OPTION_WRITE_IMAGES_MODE,
  COMMON_OPTION_WINDOW_WIDTH,
  COMMON_OPTION_CONCURRENCY,
  COMMON_OPTION_LIMIT,
  COMMON_OPTION_ICON_LIST,
  COMMON_OPTION_ALL_MODE,
  COMMON_OPTION_WINDOW_HEIGHT,
  COMMON_OPTION_WINDOW_WIDTH_GROUP,
  COMMON_OPTION_WINDOW_HEIGHT_GROUP,
  COMMON_OPTION_WINDOW_X,
  COMMON_OPTION_WRITE_DIRECTORY,
  COMMON_OPTION_WINDOW_Y,
  COMMON_OPTION_VERBOSE_MODE,
  COMMON_OPTION_DEBUG_MODE,
  COMMON_OPTION_OUTPUT_MODE,
  COMMON_OPTION_HELP,
  COMMON_OPTION_EXACT_MATCH,
  COMMON_OPTION_GRAYSCALE_MODE,
  COMMON_OPTION_CASE_SENSITIVE,
  COMMON_OPTION_FONT_NAME,
  COMMON_OPTION_FONT_FAMILY,
  COMMON_OPTION_FONT_STYLE,
  COMMON_OPTION_FONT_TYPE,
  COMMON_OPTION_RUN_HOTKEYS,
  COMMON_OPTION_CURRENT_SPACE,
  COMMON_OPTION_NOT_CURRENT_SPACE,
  COMMON_OPTION_CURRENT_DISPLAY,
  COMMON_OPTION_NOT_CURRENT_DISPLAY,
  COMMON_OPTION_OUTPUT_FILE,
  COMMON_OPTION_RANDOM_ID,
  COMMON_OPTION_DUPLICATE,
  COMMON_OPTION_NOT_DUPLICATE,
  COMMON_OPTION_RETRIES,
  COMMON_OPTION_INPUT_FILE,
  COMMON_OPTION_OUTPUT_ICNS_FILE,
  COMMON_OPTION_INPUT_ICNS_FILE,
  COMMON_OPTION_OUTPUT_PNG_FILE,
  COMMON_OPTION_INPUT_PNG_FILE,
  COMMON_OPTION_APPLICATION_PATH,
  COMMON_OPTION_APPLICATION_NAME,
  COMMON_OPTION_ICON_SIZES,
  COMMON_OPTION_INPUT_GIF_FILE,
  COMMON_OPTION_LAYOUT_NAME,
  COMMON_OPTION_XML_FILE,
  COMMON_OPTION_RESIZE_FACTOR,
  COMMON_OPTION_HEIGHT,
  COMMON_OPTION_WIDTH,
  COMMON_OPTION_HEIGHT_GREATER,
  COMMON_OPTION_HEIGHT_LESS,
  COMMON_OPTION_WIDTH_GREATER,
  COMMON_OPTION_WIDTH_LESS,
  COMMON_OPTION_CLEAR_ICONS_CACHE,
  COMMON_OPTION_PID,
  COMMON_OPTION_SORT_DIRECTION_DESC,
  COMMON_OPTION_SORT_DIRECTION_ASC,
  COMMON_OPTION_DISPLAY_OUTPUT_FILE,
  COMMON_OPTION_SORT_KEY,
  COMMON_OPTION_DB_TABLES,
  COMMON_OPTION_MINIMIZED,
  COMMON_OPTION_NOT_MINIMIZED,
  COMMON_OPTION_FRAME_RATE,
  COMMON_OPTION_SORT_WINDOW,
  COMMON_OPTION_SORT_WINDOW_KEYS,
  COMMON_OPTION_SORT_FONT_KEYS,
  COMMON_OPTION_DURATION_SECONDS,
  COMMON_OPTION_SORT_APP_KEYS,
  COMMON_OPTION_NAMES_QTY,
};
enum command_type_t {
  COMMAND_WINDOW_MOVE = 1,
  COMMAND_WINDOW_MINIMIZE,
  COMMAND_WINDOW_UNMINIMIZE,
  COMMAND_WINDOW_RESIZE,
  COMMAND_WINDOW_LIST,
  COMMAND_WINDOW_STICKY,
  COMMAND_WINDOW_UNSTICKY,
  COMMAND_WINDOW_ALL_SPACES,
  COMMAND_WINDOW_SPACE,
  COMMAND_WINDOW_NOT_ALL_SPACES,
  COMMAND_WINDOW,
  COMMAND_CAPTURE_WINDOW,
  COMMAND_CAPTURE_SPACE,
  COMMAND_CAPTURE_DISPLAY,
  COMMAND_LIST,
  COMMAND_SPACE,
  COMMAND_SPACE_LIST,
  COMMAND_SPACE_CREATE,
  COMMAND_ICON,
  COMMAND_RESIZE_WINDOW,
  COMMAND_FOCUS,
  COMMAND_FOCUSED,
  COMMAND_HOTKEYS,
  COMMAND_SET_WINDOW_SPACE,
  COMMAND_SET_SPACE,
  COMMAND_CREATE_SPACE,
  COMMAND_SET_SPACE_INDEX,
  COMMAND_WINDOWS,
  COMMAND_SPACES,
  COMMAND_DISPLAYS,
  COMMAND_FOCUSED_SERVER,
  COMMAND_FOCUSED_CLIENT,
  COMMAND_HTTPSERVER,
  COMMAND_STICKY_WINDOW,
  COMMAND_MENU_BAR,
  COMMAND_DOCK,
  COMMAND_APPS,
  COMMAND_PROCESSES,
  COMMAND_USBS,
  COMMAND_MONITORS,
  COMMAND_FONTS,
  COMMAND_ICON_LIST,
  COMMAND_CLIPBOARD,
  COMMAND_KITTYS,
  COMMAND_ALACRITTYS,
  COMMAND_CAPTURE,
  COMMAND_QUANT,
  COMMAND_ANIMATE,
  COMMAND_ANIMATE_WINDOW,
  COMMAND_ANIMATE_SPACE,
  COMMAND_ANIMATE_DISPLAY,
  COMMAND_EXTRACT,
  COMMAND_EXTRACT_WINDOW,
  COMMAND_EXTRACT_SPACE,
  COMMAND_EXTRACT_DISPLAY,
  COMMAND_SAVE_APP_ICON_PNG,
  COMMAND_SET_APP_ICON_PNG,
  COMMAND_SAVE_APP_ICON_ICNS,
  COMMAND_SET_APP_ICON_ICNS,
  COMMAND_ICON_INFO,
  COMMAND_GRAYSCALE_PNG_FILE,
  COMMAND_PARSE_XML_FILE,
  COMMAND_APP_PLIST_INFO_PATH,
  COMMAND_APP_ICNS_PATH,
  COMMAND_CLEAR_ICONS_CACHE,
  COMMAND_MINIMIZE_WINDOW,
  COMMAND_LAYOUT,
  COMMAND_LAYOUT_LIST,
  COMMAND_LAYOUT_TEST,
  COMMAND_LAYOUT_APPLY,
  COMMAND_LAYOUT_SHOW,
  COMMAND_LAYOUT_RENDER,
  COMMAND_DB,
  COMMAND_DB_INIT,
  COMMAND_DB_INFO,
  COMMAND_DB_ROWS,
  COMMAND_DB_TABLES,
  COMMAND_DB_TEST,
  COMMAND_DB_TABLE_IDS,
  COMMAND_DB_SAVE,
  COMMAND_DB_LOAD,
  COMMAND_HOTKEYS_SERVER,
  COMMAND_HOTKEYS_LIST,
  COMMAND_UNMINIMIZE_WINDOW,
  COMMAND_PID_IS_MINIMIZED,
  COMMAND_WINDOW_IS_MINIMIZED,
  COMMAND_WINDOW_LAYER,
  COMMAND_WINDOW_LEVEL,
  COMMAND_IMAGE_CONVERSIONS,
  COMMAND_SECURITY,
  COMMAND_PASTE,
  COMMAND_COPY,
  COMMAND_WINDOW_PID_INFOS,
  COMMAND_WINDOW_ID_INFO,
  COMMAND_TYPES_QTY,
};
enum color_types_t {
  COLOR_TYPE_FOCUS,
  COLOR_TYPES_QTY,
};
enum arg_clamp_type_t {
  ARG_CLAMP_TYPE_WINDOW_SIZE,
  ARG_CLAMP_TYPE_FRAME_RATE,
  ARG_CLAMP_TYPE_LIMIT,
  ARG_CLAMP_TYPE_CONCURRENCY,
  ARG_CLAMP_TYPE_DURATION,
  ARG_CLAMP_TYPES_QTY,
};
struct item_color_t {
  const char *color;
};
struct cmd_t {
  void            (*fxn)(void);
  char            *name, *description, *icon, *color;
  common_option_b *options;
};
struct check_cmd_t {
  void (*fxn)(void);
  int  arg_data_type;
};
struct cmd_t        cmds[COMMAND_TYPES_QTY + 1];
struct check_cmd_t  check_cmds[CHECK_COMMAND_TYPES_QTY + 1];
struct item_color_t item_colors[COLOR_TYPES_QTY + 1];
common_option_b     common_options_b[COMMON_OPTION_NAMES_QTY + 1];
char *get_command_about(enum command_type_t COMMAND_ID);
char *common_option_width_or_height_name(enum common_option_width_or_height_t width_or_height);
#endif
