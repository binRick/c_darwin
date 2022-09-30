#pragma once
#ifndef LS_WIN_COMMANDS_C
#define LS_WIN_COMMANDS_C
#define OPEN_SECURITY_RETRY_INTERVAL_MS      500
#define OPEN_SECURITY_DEFAULT_RETRIES_QTY    3
#include "c_vector/vector/vector.h"
#include "capture/utils/utils.h"
#include "capture/window/window.h"
#include "clamp/clamp.h"
#include "dls/dls-commands.h"
#include "dls/dls.h"
#include "hotkey-utils/hotkey-utils.h"
#include "httpserver-utils/httpserver-utils.h"
#include "httpserver/httpserver.h"
#include "keylogger/keylogger.h"
#include "pasteboard/pasteboard.h"
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
#include "table/sort/sort.h"
#include "table/utils/utils.h"
#include "tesseract/utils/utils.h"
#include "timg/utils/utils.h"
#include "vips/vips.h"
#include "wildcardcmp/wildcardcmp.h"
#include "window/utils/utils.h"
#define MAX_CONCURRENCY    25
static bool DARWIN_LS_COMMANDS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__darwin_ls_commands(void);
////////////////////////////////////////////
static void _command_move_window();
static void _command_window_id_info();
static void _command_resize_window();
static void _command_minimize_window();
static void _command_set_window_space();
static void _command_set_space();
static void _command_copy();
static void _command_paste();
static void _command_image_conversions();
static void _command_set_space_index();
static void _command_set_window_all_spaces();
static void _command_windows();
static void _command_displays();
static void _command_focus_window();
static void _command_focus_space();
static void _command_focused_window();
static void _command_focused_pid();
static void _command_focused_space();
static void _command_spaces();
static void _command_sticky_window();
static void _command_menu_bar();
static void _command_list_usb();
static void _command_httpserver();
static void _command_dock();
static void _command_processes();
static void _command_list_app();
static void _command_list_monitor();
static void _command_list_font();
static void _command_list_kitty();
static void _command_alacrittys();
static void _command_capture_window();
static void _command_animated_capture_window();
static void _command_extract_window();
static void _command_save_app_icon_to_png();
static void _command_write_app_icon_from_png();
static void _command_save_app_icon_to_icns();
static void _command_write_app_icon_icns();
static void _command_icon_info();
static void _command_app_info_plist_path();
static void _command_app_icns_path();
static void _command_parse_xml_file();
static void _command_grayscale_png();
static void _command_clear_icons_cache();
static void _command_pid_is_minimized();
static void _command_window_is_minimized();
static void _command_window_layer();
static void _command_window_level();
static void _command_open_security();
static void _command_hotkeys();
static void _command_list_hotkey();
////////////////////////////////////////////
static void _check_window_id(uint16_t window_id);
static void _check_clear_screen(void);
static void _check_image_format(char *format);
static void _check_sort_direction_desc(void);
static void _check_sort_direction_asc(void);
static void _check_random_window_id(void);
static void _check_width_group(uint16_t window_id);
static void _check_height_greater(int height_greater);
static void _check_height_less(int height_less);
static void _check_width_greater(int width_greater);
static void _check_concurrency(int concurrency);
static void _check_limit(int limit);
static void _check_width_less(int width_less);
static void _check_height_group(uint16_t window_id);
static void _check_output_mode(char *output_mode);
static void _check_output_file(char *output_file);
static void _check_output_png_file(char *output_icns_file);
static void _check_input_png_file(char *input_png_file);
static void _check_output_icns_file(char *output_icns_file);
static void _check_input_icns_file(char *input_icns_file);
static void _check_application_path(char *application_path);
static void _check_application_name(char *application_name);
static void _check_resize_factor(double resize_factor);
static void _check_xml_file(char *xml_file_path);
static void _check_icon_size(size_t icon_size);
static void _check_pid(int pid);
////////////////////////////////////////////
common_option_b    common_options_b[COMMON_OPTION_NAMES_QTY + 1] = {
  [COMMON_OPTION_HEIGHT_LESS] = ^ struct optparse_opt (struct args_t *args)                                 {
    return((struct optparse_opt)                                                                            {
      .long_name = "height-less",
      .description = "Height Less Than",
      .arg_name = "HEIGHT",
      .arg_data_type = check_cmds[CHECK_COMMAND_HEIGHT_LESS].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_HEIGHT_LESS].fxn,
      .arg_dest = &(args->height_less),
    });
  },
  [COMMON_OPTION_HEIGHT_GREATER] = ^ struct optparse_opt (struct args_t *args)                              {
    return((struct optparse_opt)                                                                            {
      .long_name = "height-greater",
      .description = "Height Greater Than",
      .arg_name = "HEIGHT",
      .arg_data_type = check_cmds[CHECK_COMMAND_HEIGHT_GREATER].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_HEIGHT_GREATER].fxn,
      .arg_dest = &(args->height_greater),
    });
  },
  [COMMON_OPTION_CONCURRENCY] = ^ struct optparse_opt (struct args_t *args)                                 {
    return((struct optparse_opt)                                                                            {
      .short_name = 'c',
      .long_name = "concurrency",
      .description = "concurrency",
      .arg_name = "CONCURRENCY-LEVEL",
      .arg_data_type = check_cmds[CHECK_COMMAND_CONCURRENCY].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_CONCURRENCY].fxn,
      .arg_dest = &(args->concurrency),
    });
  },
  [COMMON_OPTION_HEIGHT] = ^ struct optparse_opt (struct args_t *args)                                      {
    return((struct optparse_opt)                                                                            {
      .short_name = 'H',
      .long_name = "height",
      .description = "Height",
      .arg_name = "HEIGHT",
      .arg_data_type = DATA_TYPE_INT,
      .arg_dest = &(args->height),
    });
  },
  [COMMON_OPTION_WIDTH] = ^ struct optparse_opt (struct args_t *args)                                       {
    return((struct optparse_opt)                                                                            {
      .short_name = 'W',
      .long_name = "width",
      .description = "Width",
      .arg_name = "WIDTH",
      .arg_data_type = DATA_TYPE_INT,
      .arg_dest = &(args->width),
    });
  },
  [COMMON_OPTION_WIDTH_LESS] = ^ struct optparse_opt (struct args_t *args)                                  {
    return((struct optparse_opt)                                                                            {
      .long_name = "width-less",
      .description = "Width Less Than",
      .arg_name = "WIDTH",
      .arg_data_type = check_cmds[CHECK_COMMAND_WIDTH_LESS].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_WIDTH_LESS].fxn,
      .arg_dest = &(args->width_less),
    });
  },
  [COMMON_OPTION_WIDTH_GREATER] = ^ struct optparse_opt (struct args_t *args)                               {
    return((struct optparse_opt)                                                                            {
      .long_name = "width-greater",
      .description = "Width Greater Than",
      .arg_name = "WIDTH",
      .arg_data_type = check_cmds[CHECK_COMMAND_WIDTH_GREATER].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_WIDTH_GREATER].fxn,
      .arg_dest = &(args->width_greater),
    });
  },
  [COMMON_OPTION_SORT_FONT_KEYS] = ^ struct optparse_opt (struct args_t *args)                              {
    return((struct optparse_opt)                                                                            {
      .short_name = 'S',
      .long_name = "sort-by",
      .description = get_sort_type_by_description(SORT_TYPE_FONT),
      .arg_name = "SORT-BY",
      .arg_dest = &(args->sort_key),
      .arg_data_type = DATA_TYPE_STR,
    });
  },
  [COMMON_OPTION_SORT_APP_KEYS] = ^ struct optparse_opt (struct args_t *args)                               {
    return((struct optparse_opt)                                                                            {
      .short_name = 'S',
      .long_name = "sort-by",
      .description = "", //get_app_sort_types_description(),
      .arg_name = "SORT-BY",
      .arg_dest = &(args->sort_key),
      .arg_data_type = DATA_TYPE_STR,
    });
  },
  [COMMON_OPTION_SORT_WINDOW_KEYS] = ^ struct optparse_opt (struct args_t *args)                            {
    return((struct optparse_opt)                                                                            {
      .short_name = 'S',
      .long_name = "sort-by",
      .description = get_sort_type_by_description(SORT_TYPE_WINDOW),
      .arg_name = "SORT-BY",
      .arg_data_type = check_cmds[CHECK_COMMAND_SORT_KEY].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_SORT_KEY].fxn,
      .arg_dest = &(args->sort_key),
    });
  },
  [COMMON_OPTION_SORT_KEY] = ^ struct optparse_opt (struct args_t *args)                                    {
    return((struct optparse_opt)                                                                            {
      .short_name = 'S',
      .long_name = "sort-by",
      .description = "Sort By Key Name",
      .arg_name = "SORT-KEY-NAME",
      .arg_dest = &(args->sort_key),
      .arg_data_type = DATA_TYPE_STR,
    });
  },
  [COMMON_OPTION_HELP_SUBCMD] = ^ struct optparse_opt (struct args_t __attribute__((unused)) *args)         {
    return((struct optparse_opt)                                                                            {
      .short_name = 'h',
      .long_name = "help",
      .description = COLOR_HELP "Command Help" AC_RESETALL,
      .function = optparse_print_help,
    });
  },
  [COMMON_OPTION_SORT_DIRECTION_ASC] = ^ struct optparse_opt (struct args_t __attribute__((unused)) *args)  {
    return((struct optparse_opt)                                                                            {
      .long_name = "asc",
      .description = "Sort Ascending",
      .function = check_cmds[CHECK_COMMAND_SORT_DIRECTION_ASC].fxn,
    });
  },
  [COMMON_OPTION_SORT_DIRECTION_DESC] = ^ struct optparse_opt (struct args_t __attribute__((unused)) *args) {
    return((struct optparse_opt)                                                                            {
      .long_name = "desc",
      .description = "Sort Descending",
      .function = check_cmds[CHECK_COMMAND_SORT_DIRECTION_DESC].fxn,
    });
  },
  [COMMON_OPTION_ALL_WINDOWS] = ^ struct optparse_opt (struct args_t *args)                                 {
    return((struct optparse_opt)                                                                            {
      .short_name = 'A',
      .long_name = "all-windows",
      .description = "All Windows",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->all_windows),
    });
  },
  [COMMON_OPTION_DISPLAY_OUTPUT_FILE] = ^ struct optparse_opt (struct args_t *args)                         {
    return((struct optparse_opt)                                                                            {
      .short_name = 'D',
      .long_name = "display-output-file",
      .description = "Display Output File",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->display_output_file),
    });
  },
  [COMMON_OPTION_NON_MINIMIZED] = ^ struct optparse_opt (struct args_t *args)                               {
    return((struct optparse_opt)                                                                            {
      .short_name = 'm',
      .long_name = "non-minimized",
      .description = "Show Non Minimized Only",
      .arg_data_type = check_cmds[CHECK_COMMAND_PID].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_PID].fxn,
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->non_minimized_only),
    });
  },
  [COMMON_OPTION_MINIMIZED] = ^ struct optparse_opt (struct args_t *args)                                   {
    return((struct optparse_opt)                                                                            {
      .short_name = 'm',
      .long_name = "minimized",
      .description = "Show Minimized Only",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->minimized_only),
    });
  },
  [COMMON_OPTION_RANDOM_WINDOW_ID] = ^ struct optparse_opt (struct args_t *args)                            {
    return((struct optparse_opt)                                                                            {
      .short_name = 'r',
      .long_name = "random-window-id",
      .description = "Random Window ID",
      .function = check_cmds[CHECK_COMMAND_RANDOM_WINDOW_ID].fxn,
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->random_window_id),
    });
  },
  [COMMON_OPTION_PID] = ^ struct optparse_opt (struct args_t *args)                                         {
    return((struct optparse_opt)                                                                            {
      .short_name = 'p',
      .long_name = "pid",
      .description = "PID",
      .arg_name = "PID",
      .arg_data_type = check_cmds[CHECK_COMMAND_PID].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_PID].fxn,
      .arg_dest = &(args->pid),
    });
  },
  [COMMON_OPTION_CLEAR_SCREEN] = ^ struct optparse_opt (struct args_t *args)                                {
    return((struct optparse_opt)                                                                            {
      .short_name = 'C',
      .long_name = "clear",
      .description = "Clear Screen to Create Clean Output",
      .function = check_cmds[CHECK_COMMAND_CLEAR_SCREEN].fxn,
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->clear_screen),
    });
  },
  [COMMON_OPTION_CURRENT_DISPLAY] = ^ struct optparse_opt (struct args_t *args)                             {
    return((struct optparse_opt)                                                                            {
      .short_name = 'X',
      .long_name = "current-display",
      .description = "Windows in Display only",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->current_display_only),
    });
  },
  [COMMON_OPTION_CURRENT_SPACE] = ^ struct optparse_opt (struct args_t *args)                               {
    return((struct optparse_opt)                                                                            {
      .long_name = "current-space",
      .description = "Windows in current space only",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->current_space_only),
    });
  },
  [COMMON_OPTION_HELP] = ^ struct optparse_opt (__attribute__((unused)) struct args_t *args)                {
    return((struct optparse_opt)                                                                            {
      .short_name = 'h',
      .long_name = "help",
      .description = "Print help information and quit",
      .function = optparse_print_help,
    });
  },
  [COMMON_OPTION_ICON_SIZE] = ^ struct optparse_opt (struct args_t *args)                                   {
    return((struct optparse_opt)                                                                            {
      .short_name = 's',
      .long_name = "icon-size",
      .description = "Icon Size",
      .arg_name = "ICON-SIZE",
      .arg_data_type = check_cmds[CHECK_COMMAND_ICON_SIZE].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_ICON_SIZE].fxn,
      .arg_dest = &(args->icon_size),
    });
  },
  [COMMON_OPTION_APPLICATION_NAME] = ^ struct optparse_opt (struct args_t *args)                            {
    return((struct optparse_opt)                                                                            {
      .short_name = 'a',
      .long_name = "application-name",
      .description = "Application Name",
      .arg_name = "APPLICATION-NAME",
      .arg_data_type = check_cmds[CHECK_COMMAND_APPLICATION_NAME].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_APPLICATION_NAME].fxn,
      .arg_dest = &(args->application_name),
    });
  },
  [COMMON_OPTION_APPLICATION_PATH] = ^ struct optparse_opt (struct args_t *args)                            {
    return((struct optparse_opt)                                                                            {
      .short_name = 'P',
      .long_name = "application-path",
      .description = "Application Path",
      .arg_name = "APPLICATION-PATH",
      .arg_data_type = check_cmds[CHECK_COMMAND_APPLICATION_PATH].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_APPLICATION_PATH].fxn,
      .arg_dest = &(args->application_path),
    });
  },
  [COMMON_OPTION_RESIZE_FACTOR] = ^ struct optparse_opt (struct args_t *args)                               {
    return((struct optparse_opt)                                                                            {
      .short_name = 'r',
      .long_name = "resize-factor",
      .description = "Resize Factor",
      .arg_name = "RESIZE-FACTOR",
      .arg_data_type = check_cmds[CHECK_COMMAND_RESIZE_FACTOR].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_RESIZE_FACTOR].fxn,
      .arg_dest = &(args->resize_factor),
    });
  },
  [COMMON_OPTION_XML_FILE] = ^ struct optparse_opt (struct args_t *args)                                    {
    return((struct optparse_opt)                                                                            {
      .short_name = 'x',
      .long_name = "xml-file",
      .description = "XML File",
      .arg_name = "XML-FILE",
      .arg_data_type = check_cmds[CHECK_COMMAND_XML_FILE].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_XML_FILE].fxn,
      .arg_dest = &(args->xml_file_path),
    });
  },
  [COMMON_OPTION_INPUT_PNG_FILE] = ^ struct optparse_opt (struct args_t *args)                              {
    return((struct optparse_opt)                                                                            {
      .short_name = 'f',
      .long_name = "png-file",
      .description = "PNG File",
      .arg_name = "PNG-FILE",
      .arg_data_type = check_cmds[CHECK_COMMAND_INPUT_PNG_FILE].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_INPUT_PNG_FILE].fxn,
      .arg_dest = &(args->input_png_file),
    });
  },
  [COMMON_OPTION_OUTPUT_PNG_FILE] = ^ struct optparse_opt (struct args_t *args)                             {
    return((struct optparse_opt)                                                                            {
      .short_name = 'f',
      .long_name = "png-file",
      .description = "PNG File",
      .arg_name = "PNG-FILE",
      .arg_data_type = check_cmds[CHECK_COMMAND_OUTPUT_PNG_FILE].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_OUTPUT_PNG_FILE].fxn,
      .arg_dest = &(args->output_png_file),
    });
  },
  [COMMON_OPTION_INPUT_ICNS_FILE] = ^ struct optparse_opt (struct args_t *args)                             {
    return((struct optparse_opt)                                                                            {
      .short_name = 'f',
      .long_name = "icns-file",
      .description = "ICNS File",
      .arg_name = "ICNS-FILE",
      .arg_data_type = check_cmds[CHECK_COMMAND_INPUT_ICNS_FILE].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_INPUT_ICNS_FILE].fxn,
      .arg_dest = &(args->input_icns_file),
    });
  },
  [COMMON_OPTION_OUTPUT_ICNS_FILE] = ^ struct optparse_opt (struct args_t *args)                            {
    return((struct optparse_opt)                                                                            {
      .short_name = 'f',
      .long_name = "icns-file",
      .description = "ICNS File",
      .arg_name = "ICNS-FILE",
      .arg_data_type = check_cmds[CHECK_COMMAND_OUTPUT_ICNS_FILE].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_OUTPUT_ICNS_FILE].fxn,
      .arg_dest = &(args->output_icns_file),
    });
  },
  [COMMON_OPTION_INPUT_FILE] = ^ struct optparse_opt (struct args_t *args)                                  {
    return((struct optparse_opt)                                                                            {
      .short_name = 'i',
      .long_name = "input-file",
      .description = "Input File",
      .arg_name = "INPUT-FILE",
      .arg_data_type = check_cmds[CHECK_COMMAND_INPUT_FILE].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_INPUT_FILE].fxn,
      .arg_dest = &(args->input_file),
    });
  },
  [COMMON_OPTION_OUTPUT_FILE] = ^ struct optparse_opt (struct args_t *args)                                 {
    return((struct optparse_opt)                                                                            {
      .short_name = 'o',
      .long_name = "output-file",
      .description = "Output File",
      .arg_name = "OUTPUT-FILE",
      .arg_data_type = check_cmds[CHECK_COMMAND_OUTPUT_FILE].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_OUTPUT_FILE].fxn,
      .arg_dest = &(args->output_file),
    });
  },
  [COMMON_OPTION_IMAGE_FORMAT] = ^ struct optparse_opt (struct args_t *args)                                {
    return((struct optparse_opt)                                                                            {
      .short_name = 'f',
      .long_name = "format",
      .description = get_image_format_names_csv(),
      .arg_name = "IMAGE-FORMAT",
      .arg_data_type = check_cmds[CHECK_COMMAND_IMAGE_FORMAT].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_IMAGE_FORMAT].fxn,
      .arg_dest = &(args->image_format),
    });
  },
  [COMMON_OPTION_OUTPUT_MODE] = ^ struct optparse_opt (struct args_t *args)                                 {
    return((struct optparse_opt)                                                                            {
      .short_name = 'm',
      .long_name = "mode",
      .description = "Output Mode- text, json, or table",
      .arg_name = "OUTPUT-MODE",
      .arg_data_type = check_cmds[CHECK_COMMAND_OUTPUT_MODE].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_OUTPUT_MODE].fxn,
      .arg_dest = &(args->output_mode_s),
    });
  },
  [COMMON_OPTION_CLEAR_ICONS_CACHE] = ^ struct optparse_opt (struct args_t *args)                           {
    return((struct optparse_opt)                                                                            {
      .short_name = 'c',
      .long_name = "clear-icons-cache",
      .description = "Clear Icons Cache",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->clear_icons_cache),
    });
  },
  [COMMON_OPTION_LIMIT] = ^ struct optparse_opt (struct args_t *args)                                       {
    return((struct optparse_opt)                                                                            {
      .short_name = 'l',
      .long_name = "limit",
      .description = "Limit",
      .arg_name = "LIMIT",
      .arg_dest = &(args->limit),
      .arg_data_type = check_cmds[CHECK_COMMAND_LIMIT].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_LIMIT].fxn,
    });
  },
  [COMMON_OPTION_NON_DUPLICATE] = ^ struct optparse_opt (struct args_t *args)                               {
    return((struct optparse_opt)                                                                            {
      .long_name = "non-duplicate",
      .description = "Show Non Duplicate Fonts",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->non_duplicate),
    });
  },
  [COMMON_OPTION_DUPLICATE] = ^ struct optparse_opt (struct args_t *args)                                   {
    return((struct optparse_opt)                                                                            {
      .long_name = "duplicate",
      .description = "Show Duplicate Fonts",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->duplicate),
    });
  },
  [COMMON_OPTION_CASE_SENSITIVE] = ^ struct optparse_opt (struct args_t *args)                              {
    return((struct optparse_opt)                                                                            {
      .long_name = "case-sensitive",
      .description = "Case Sensitive Match",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->case_sensitive),
    });
  },
  [COMMON_OPTION_EXACT_MATCH] = ^ struct optparse_opt (struct args_t *args)                                 {
    return((struct optparse_opt)                                                                            {
      .short_name = 'e',
      .long_name = "exact-match",
      .description = "Exact Match (does not use wildcard)",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->exact_match),
    });
  },
  [COMMON_OPTION_VERBOSE] = ^ struct optparse_opt (struct args_t *args)                                     {
    return((struct optparse_opt)                                                                            {
      .short_name = 'v',
      .long_name = "verbose",
      .description = "Increase verbosity",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->verbose),
    });
  },
  [COMMON_OPTION_WINDOW_ID] = ^ struct optparse_opt (struct args_t *args)                                   {
    return((struct optparse_opt)                                                                            {
      .short_name = 'w',
      .long_name = "window-id",
      .description = "window id",
      .arg_name = "WINDOW-ID",
      .arg_dest = &(args->window_id),
      .arg_data_type = check_cmds[CHECK_COMMAND_WINDOW_ID].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_WINDOW_ID].fxn,
    });
  },
  [COMMON_OPTION_DURATION_SECONDS] = ^ struct optparse_opt (struct args_t *args)                            {
    return((struct optparse_opt)                                                                            {
      .short_name = 'd',
      .long_name = "duration",
      .description = "Duration (seconds)",
      .arg_name = "SECONDS",
      .arg_data_type = DATA_TYPE_UINT,
      .arg_dest = &(args->duration_seconds),
    });
  },
  [COMMON_OPTION_WINDOW_X] = ^ struct optparse_opt (struct args_t *args)                                    {
    return((struct optparse_opt)                                                                            {
      .short_name = 'x',
      .long_name = "window-x",
      .description = "Window X",
      .arg_name = "WINDOW-X",
      .arg_data_type = DATA_TYPE_UINT16,
      .arg_dest = &(args->x),
    });
  },
  [COMMON_OPTION_COMPRESS] = ^ struct optparse_opt (struct args_t *args)                                    {
    return((struct optparse_opt)                                                                            {
      .short_name = 'z',
      .long_name = "compress",
      .description = "Compress Image",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->compress),
    });
  },
  [COMMON_OPTION_FONT_TYPE] = ^ struct optparse_opt (struct args_t *args)                                   {
    return((struct optparse_opt)                                                                            {
      .short_name = 't',
      .long_name = "type",
      .description = "Font Type",
      .arg_name = "FONT-TYPE",
      .arg_data_type = DATA_TYPE_STR,
      .arg_dest = &(args->font_type),
    });
  },
  [COMMON_OPTION_FONT_STYLE] = ^ struct optparse_opt (struct args_t *args)                                  {
    return((struct optparse_opt)                                                                            {
      .short_name = 's',
      .long_name = "style",
      .description = "Font Style",
      .arg_name = "FONT-STYLE",
      .arg_data_type = DATA_TYPE_STR,
      .arg_dest = &(args->font_style),
    });
  },
  [COMMON_OPTION_FONT_FAMILY] = ^ struct optparse_opt (struct args_t *args)                                 {
    return((struct optparse_opt)                                                                            {
      .short_name = 'f',
      .long_name = "family",
      .description = "Font Family",
      .arg_name = "FONT-FAMILY",
      .arg_data_type = DATA_TYPE_STR,
      .arg_dest = &(args->font_family),
    });
  },
  [COMMON_OPTION_FONT_NAME] = ^ struct optparse_opt (struct args_t *args)                                   {
    return((struct optparse_opt)                                                                            {
      .short_name = 'n',
      .long_name = "name",
      .description = "Font Name",
      .arg_name = "FONT-NAME",
      .arg_data_type = DATA_TYPE_STR,
      .arg_dest = &(args->font_name),
    });
  },
  [COMMON_OPTION_CONTENT] = ^ struct optparse_opt (struct args_t *args)                                     {
    return((struct optparse_opt)                                                                            {
      .short_name = 'c',
      .long_name = "content",
      .description = "Content",
      .arg_name = "CONTENT",
      .arg_data_type = DATA_TYPE_STR,
      .arg_dest = &(args->content),
    });
  },
  [COMMON_OPTION_RETRIES] = ^ struct optparse_opt (struct args_t *args)                                     {
    return((struct optparse_opt)                                                                            {
      .short_name = 'r',
      .long_name = "retries",
      .description = "Retries",
      .arg_name = "RETRIES",
      .arg_data_type = DATA_TYPE_INT,
      .arg_dest = &(args->retries),
    });
  },
  [COMMON_OPTION_WINDOW_Y] = ^ struct optparse_opt (struct args_t *args)                                    {
    return((struct optparse_opt)                                                                            {
      .short_name = 'y',
      .long_name = "window-y",
      .description = "Window y",
      .arg_name = "WINDOW-Y",
      .arg_data_type = DATA_TYPE_UINT16,
      .arg_dest = &(args->y),
    });
  },
  [COMMON_OPTION_WINDOW_WIDTH] = ^ struct optparse_opt (struct args_t *args)                                {
    return((struct optparse_opt)                                                                            {
      .short_name = 'W',
      .long_name = "window-width",
      .description = "Window Width",
      .arg_name = "WINDOW-WIDTH",
      .arg_data_type = check_cmds[CHECK_COMMAND_WIDTH].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_WIDTH].fxn,
      .arg_dest = &(args->width),
    });
  },
  [COMMON_OPTION_WINDOW_HEIGHT] = ^ struct optparse_opt (struct args_t *args)                               {
    return((struct optparse_opt)                                                                            {
      .short_name = 'H',
      .long_name = "window-height",
      .description = "Window Height",
      .arg_name = "WINDOW-HEIGHT",
      .arg_data_type = DATA_TYPE_UINT16,
      .function = check_cmds[CHECK_COMMAND_HEIGHT].fxn,
      .arg_dest = &(args->height),
    });
  },
  [COMMON_OPTION_WINDOW_WIDTH_GROUP] = ^ struct optparse_opt (struct args_t *args)                          {
    return((struct optparse_opt)                                                                            {
      .short_name = 'W',
      .long_name = "window-width",
      .description = "Window Width",
      .arg_name = "WINDOW-WIDTH",
      .group = COMMON_OPTION_GROUP_WIDTH_OR_HEIGHT,
      .arg_data_type = check_cmds[CHECK_COMMAND_WIDTH_GROUP].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_WIDTH_GROUP].fxn,
      .arg_dest = &(args->width_or_height_group),
    });
  },
  [COMMON_OPTION_FRAME_RATE] = ^ struct optparse_opt (struct args_t *args)                                  {
    return((struct optparse_opt)                                                                            {
      .short_name = 'F',
      .long_name = "frame-rate",
      .description = "Frame Rate",
      .arg_name = "FRAME-RATE",
      .arg_dest = &(args->frame_rate),
      .arg_data_type = DATA_TYPE_INT,
    });
  },
  [COMMON_OPTION_WINDOW_HEIGHT_GROUP] = ^ struct optparse_opt (struct args_t *args)                         {
    return((struct optparse_opt)                                                                            {
      .short_name = 'H',
      .long_name = "window-height",
      .description = "Window Height",
      .arg_name = "WINDOW-HEIGHT",
      .group = COMMON_OPTION_GROUP_WIDTH_OR_HEIGHT,
      .arg_data_type = check_cmds[CHECK_COMMAND_HEIGHT_GROUP].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_HEIGHT_GROUP].fxn,
      .arg_dest = &(args->width_or_height_group),
    });
  },
  [COMMON_OPTION_DISPLAY_ID] = ^ struct optparse_opt (struct args_t *args)                                  {
    return((struct optparse_opt)                                                                            {
      .short_name = 'd',
      .long_name = "display-id",
      .description = "Display ID",
      .arg_name = "DISPLAY-ID",
      .arg_data_type = DATA_TYPE_INT32,
      .arg_dest = &(args->display_id),
    });
  },
  [COMMON_OPTION_SPACE_ID] = ^ struct optparse_opt (struct args_t *args)                                    {
    return((struct optparse_opt)                                                                            {
      .short_name = 's',
      .long_name = "space-id",
      .description = "space id",
      .arg_name = "SPACE-ID",
      .arg_data_type = DATA_TYPE_UINT64,
      .arg_dest = &(args->space_id),
    });
  },
};
////////////////////////////////////////////
struct check_cmd_t check_cmds[CHECK_COMMAND_TYPES_QTY + 1] = {
  [CHECK_COMMAND_LIMIT] =               {
    .fxn           = (void (*)(void))(*_check_limit),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_CONCURRENCY] =         {
    .fxn           = (void (*)(void))(*_check_concurrency),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_HEIGHT_LESS] =         {
    .fxn           = (void (*)(void))(*_check_height_less),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_HEIGHT_GREATER] =      {
    .fxn           = (void (*)(void))(*_check_height_greater),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_WIDTH_LESS] =          {
    .fxn           = (void (*)(void))(*_check_width_less),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_WIDTH_GREATER] =       {
    .fxn           = (void (*)(void))(*_check_width_greater),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_RANDOM_WINDOW_ID] =    {
    .fxn           = (void (*)(void))(*_check_random_window_id),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_PID] =                 {
    .fxn           = (void (*)(void))(*_check_pid),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_WINDOW_ID] =           {
    .fxn           = (void (*)(void))(*_check_window_id),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_ICON_SIZE] =           {
    .fxn           = (void (*)(void))(*_check_icon_size),
    .arg_data_type = DATA_TYPE_UINT64,
  },
  [CHECK_COMMAND_APPLICATION_NAME] =    {
    .fxn           = (void (*)(void))(*_check_application_name),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_APPLICATION_PATH] =    {
    .fxn           = (void (*)(void))(*_check_application_path),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_SORT_DIRECTION_DESC] = {
    .fxn = (void (*)(void))(*_check_sort_direction_desc),
  },
  [CHECK_COMMAND_SORT_DIRECTION_ASC] =  {
    .fxn = (void (*)(void))(*_check_sort_direction_asc),
  },
  [CHECK_COMMAND_WIDTH_GROUP] =         {
    .fxn           = (void (*)(void))(*_check_width_group),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_HEIGHT_GROUP] =        {
    .fxn           = (void (*)(void))(*_check_height_group),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_RESIZE_FACTOR] =       {
    .fxn           = (void (*)(void))(*_check_resize_factor),
    .arg_data_type = DATA_TYPE_DBL,
  },
  [CHECK_COMMAND_INPUT_ICNS_FILE] =     {
    .fxn           = (void (*)(void))(*_check_input_icns_file),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_IMAGE_FORMAT] =        {
    .fxn           = (void (*)(void))(*_check_image_format),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_INPUT_PNG_FILE] =      {
    .fxn           = (void (*)(void))(*_check_input_png_file),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_OUTPUT_PNG_FILE] =     {
    .fxn           = (void (*)(void))(*_check_output_png_file),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_OUTPUT_ICNS_FILE] =    {
    .fxn           = (void (*)(void))(*_check_output_icns_file),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_OUTPUT_MODE] =         {
    .fxn           = (void (*)(void))(*_check_output_mode),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_XML_FILE] =            {
    .fxn           = (void (*)(void))(*_check_xml_file),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_CLEAR_SCREEN] =        {
    .fxn = (void (*)(void))(*_check_clear_screen),
  },
  [CHECK_COMMAND_OUTPUT_FILE] =         {
    .fxn           = (void (*)(void))(*_check_output_file),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_TYPES_QTY] =           { 0},
};

struct cmd_t       cmds[COMMAND_TYPES_QTY + 1] = {
  [COMMAND_MOVE_WINDOW] =             {
    .name = "move-window",           .icon = "🪟", .color = COLOR_WINDOW, .description = "Move Window",
    .fxn  = (*_command_move_window),
  },
  [COMMAND_RESIZE_WINDOW] =           {
    .name = "resize-window",           .icon = "💡", .color = COLOR_WINDOW, .description = "Resize Window",
    .fxn  = (*_command_resize_window),
  },
  [COMMAND_HOTKEYS] =                 {
    .name = "hotkeys",           .icon = "🔅", .color = COLOR_WINDOW, .description = "Hotkeys Daemon",
    .fxn  = (*_command_hotkeys),
  },
  [COMMAND_WINDOW_LEVEL] =            {
    .name        = "window-level",           .icon = "🔅", .color = COLOR_WINDOW,
    .description = "Window Level",
    .fxn         = (*_command_window_level),
  },
  [COMMAND_WINDOW_LAYER] =            {
    .name        = "window-layer",           .icon = "🔅", .color = COLOR_WINDOW,
    .description = "Window Layer",
    .fxn         = (*_command_window_layer),
  },
  [COMMAND_WINDOW_IS_MINIMIZED] =     {
    .name        = "window-minimized",              .icon = "🔅", .color = COLOR_WINDOW,
    .description = "Window is Minimized",
    .fxn         = (*_command_window_is_minimized),
  },
  [COMMAND_PID_IS_MINIMIZED] =        {
    .name        = "pid-minimized",              .icon = "🔅", .color = COLOR_WINDOW,
    .description = "PID is Minimized",
    .fxn         = (*_command_pid_is_minimized),
  },
  [COMMAND_MINIMIZE_WINDOW] =         {
    .name        = "minimize-window",           .icon = "🔅", .color = COLOR_WINDOW,
    .description = "Minimize Window",
    .fxn         = (*_command_minimize_window),
  },
  [COMMAND_SET_WINDOW_SPACE] =        {
    .fxn = (*_command_set_window_space)
  },
  [COMMAND_FOCUS_SPACE] =             {
    .name = "focus-space",           .icon = "🔅", .color = COLOR_SPACE, .description = "Focus Space",
    .fxn  = (*_command_focus_space),
  },
  [COMMAND_FOCUS_WINDOW] =            {
    .name = "focus-window",           .icon = "🔅", .color = COLOR_WINDOW, .description = "Focus Window",
    .fxn  = (*_command_focus_window),
  },
  [COMMAND_SET_WINDOW_ALL_SPACES] =   {
    .fxn = (*_command_set_window_all_spaces)
  },
  [COMMAND_SET_SPACE] =               {
    .name = "set-space",           .icon = "💣", .color = COLOR_SPACE, .description = "Set Space",
    .fxn  = (*_command_set_space),
  },
  [COMMAND_SET_SPACE_INDEX] =         {
    .fxn = (*_command_set_space_index)
  },
  [COMMAND_WINDOWS] =                 {
    .name = "windows", .icon = "🥑", .color = COLOR_WINDOW, .description = "List Windows",
    .fxn  = (*_command_windows)
  },
  [COMMAND_WINDOW_ID_INFO] =          {
    .name        = "window-id-info",           .icon = "🔅", .color = COLOR_WINDOW,
    .description = "Window ID Info",
    .fxn         = (*_command_window_id_info),
  },
  [COMMAND_SPACES] =                  {
    .name = "spaces", .icon = "🥑", .color = COLOR_WINDOW, .description = "List Spaces",
    .fxn  = (*_command_spaces)
  },
  [COMMAND_SECURITY] =                {
    .name = "security", .icon = "🐾", .color = COLOR_WINDOW, .description = "Open Security",
    .fxn  = (*_command_open_security)
  },
  [COMMAND_DISPLAYS] =                {
    .name = "displays", .icon = "🐾", .color = COLOR_WINDOW, .description = "List Displays",
    .fxn  = (*_command_displays)
  },
  [COMMAND_HTTPSERVER] =              {
    .fxn = (*_command_httpserver)
  },
  [COMMAND_STICKY_WINDOW] =           {
    .name        = "sticky-window",     .icon = "🕰", .color = COLOR_WINDOW,
    .description = "Set Window Sticky",
    .fxn         = (*_command_sticky_window)
  },
  [COMMAND_MENU_BAR] =                {
    .name        = "menu-bar",      .icon = "📀", .color = COLOR_SHOW,
    .description = "Menu Bar Info",
    .fxn         = (*_command_menu_bar)
  },
  [COMMAND_DOCK] =                    {
    .name        = "dock",      .icon = "📡", .color = COLOR_SHOW,
    .description = "Dock Info",
    .fxn         = (*_command_dock)
  },
  [COMMAND_FOCUSED_SPACE] =           {
    .name        = "focused-space", .icon = "💮", .color = COLOR_LIST,
    .description = "Focused Space",
    .fxn         = (*_command_focused_space)
  },
  [COMMAND_FOCUSED_WINDOW] =          {
    .name        = "focused-window", .icon = "💮", .color = COLOR_LIST,
    .description = "Focused Window",
    .fxn         = (*_command_focused_window)
  },
  [COMMAND_FOCUSED_PID] =             {
    .name        = "focused-pid", .icon = "🌈", .color = COLOR_LIST,
    .description = "Focused PID",
    .fxn         = (*_command_focused_pid)
  },
  [COMMAND_APPS] =                    {
    .name        = "apps",         .icon = "📡", .color = COLOR_SHOW,
    .description = "Applications",
    .fxn         = (*_command_list_app)
  },
  [COMMAND_FONTS] =                   {
    .name        = "fonts", .icon = "🦓", .color = COLOR_SHOW,
    .description = "Fonts",
    .fxn         = (*_command_list_font)
  },
  [COMMAND_ANIMATED_CAPTURE_WINDOW] = {
    .name        = "animate",                 .icon = "💤", .color = COLOR_LIST,
    .description = "Animated Window Capture",
    .fxn         = (*_command_animated_capture_window)
  },
  [COMMAND_KITTYS] =                  {
    .name        = "kittys", .icon = "💤", .color = COLOR_LIST,
    .description = "Kittys",
    .fxn         = (*_command_list_kitty)
  },
  [COMMAND_IMAGE_CONVERSIONS] =       {
    .name        = "image-conversions", .icon = "💮", .color = COLOR_LIST,
    .description = "Image Conversions",
    .fxn         = (*_command_image_conversions)
  },
  [COMMAND_PASTE] =                   {
    .name        = "paste",           .icon = "💮", .color = COLOR_LIST,
    .description = "Clipboard Paste",
    .fxn         = (*_command_paste)
  },
  [COMMAND_COPY] =                    {
    .name        = "copy",           .icon = "💮", .color = COLOR_LIST,
    .description = "Clipboard Copy",
    .fxn         = (*_command_copy)
  },
  [COMMAND_LIST_HOTKEYS] =            {
    .name        = "list-hotkeys", .icon = "💮", .color = COLOR_LIST,
    .description = "List Hotkeys",
    .fxn         = (*_command_list_hotkey)
  },
  [COMMAND_ALACRITTYS] =              {
    .name        = "alacrittys", .icon = "💮", .color = COLOR_LIST,
    .description = "Alacrittys",
    .fxn         = (*_command_alacrittys)
  },
  [COMMAND_USB_DEVICES] =             {
    .fxn = (*_command_list_usb)
  },
  [COMMAND_MONITORS] =                {
    .fxn = (*_command_list_monitor)
  },
  [COMMAND_PROCESSES] =               {
    .fxn = (*_command_processes)
  },
  [COMMAND_EXTRACT_WINDOW] =          {
    .fxn = (*_command_extract_window)
  },
  [COMMAND_CAPTURE_WINDOW] =          {
    .fxn = (*_command_capture_window)
  },
  [COMMAND_SAVE_APP_ICON_ICNS] =      {
    .fxn = (*_command_save_app_icon_to_icns)
  },
  [COMMAND_SAVE_APP_ICON_PNG] =       {
    .fxn = (*_command_save_app_icon_to_png)
  },
  [COMMAND_SET_APP_ICON_PNG] =        {
    .fxn = (*_command_write_app_icon_from_png)
  },
  [COMMAND_SET_APP_ICON_ICNS] =       {
    .fxn = (*_command_write_app_icon_icns)
  },
  [COMMAND_ICON_INFO] =               {
    .fxn = (*_command_icon_info)
  },
  [COMMAND_APP_PLIST_INFO_PATH] =     {
    .fxn = (*_command_app_info_plist_path)
  },
  [COMMAND_APP_ICNS_PATH] =           {
    .fxn = (*_command_app_icns_path)
  },
  [COMMAND_PARSE_XML_FILE] =          {
    .fxn = (*_command_parse_xml_file)
  },
  [COMMAND_GRAYSCALE_PNG_FILE] =      {
    .fxn = (*_command_grayscale_png)
  },
  [COMMAND_CLEAR_ICONS_CACHE] =       {
    .fxn = (*_command_clear_icons_cache)
  },
  [COMMAND_TYPES_QTY] =               { 0},
};

////////////////////////////////////////////
static void _check_image_format(char *format){
  args->image_format_type = image_format_type(format);
  if (args->image_format_type < 0) {
    log_error("Invalid Image Format");
    exit(EXIT_FAILURE);
  }
}

static void _check_sort_direction_desc(){
  args->sort_direction = "desc";
  return(EXIT_SUCCESS);
}

static void _check_sort_direction_asc(){
  args->sort_direction = "asc";
  return(EXIT_SUCCESS);
}

static void _check_height_less(int height_less){
  args->height_less = height_less;
  return(EXIT_SUCCESS);
}

static void _check_height_greater(int height_greater){
  args->height_greater = height_greater;
  return(EXIT_SUCCESS);
}

static void _check_width_less(int width_less){
  args->width_less = width_less;
  return(EXIT_SUCCESS);
}

static void _check_width_greater(int width_greater){
  args->width_greater = width_greater;
  return(EXIT_SUCCESS);
}

static void _check_resize_factor(double resize_factor){
  errno = 0;
  log_info("resize factor:%f", resize_factor);
  if (resize_factor <= 0) {
    log_error("Invalid Resize Factor %f", resize_factor);
    exit(EXIT_FAILURE);
  }
  return(EXIT_SUCCESS);
}

static void _check_pid(int pid){
  errno = 0;
  if (pid < 2) {
    log_error("Invalid Pid %d", pid);
    exit(EXIT_FAILURE);
  }
  return(EXIT_SUCCESS);
}

static void _check_icon_size(size_t icon_size){
  errno = 0;
  if (app_icon_size_is_valid(icon_size) == false) {
    log_error("Invalid Icon Size %lu", icon_size);
    exit(EXIT_FAILURE);
  }
  return(EXIT_SUCCESS);
}

static void _check_application_name(char *application_name){
  args->application_name = application_name;
  return(EXIT_SUCCESS);
}

static void _check_application_path(char *application_path){
  if (fsio_dir_exists(application_path) == false) {
    log_error("Invalid Application Path '%s'", application_path);
    exit(EXIT_FAILURE);
  }
  struct StringBuffer *sb = stringbuffer_new();
  stringbuffer_append_string(sb, application_path);
  stringbuffer_append_string(sb, "/");
  stringbuffer_append_string(sb, "Contents");
  if (fsio_dir_exists(stringbuffer_to_string(sb)) == false) {
    log_error("Invalid Application Path '%s' (Non existent directory %s)", application_path, stringbuffer_to_string(sb));
    exit(EXIT_FAILURE);
  }

  stringbuffer_release(sb);
  return(EXIT_SUCCESS);
}

static void _check_output_png_file(char *output_png_file){
  if (!output_png_file || strlen(output_png_file) < 5) {
    log_error("Invalid output PNG File");
    exit(EXIT_FAILURE);
  }
  return(EXIT_SUCCESS);
}

static void _check_clear_screen(void){
  if (args->clear_screen == true) {
    if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
      log_debug("Clearing Screen");
    }
    fprintf(stdout, "%s", AC_CLS);
  }
  return(EXIT_SUCCESS);
}

static void _check_random_window_id(void){
  args->window    = get_random_window_info();
  args->window_id = args->window->window_id;
  if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
    log_debug("%d|%d|%dx%d|",
              args->window_id, args->random_window_id,
              (int)args->window->rect.size.width,
              (int)args->window->rect.size.height
              );
  }
  return(EXIT_SUCCESS);
}

static void _check_input_png_file(char *input_png_file){
  if (fsio_file_exists(input_png_file) == false) {
    log_error("Invalid Input PNG Path '%s'", input_png_file);
    exit(EXIT_FAILURE);
  }
  return(EXIT_SUCCESS);
}

static void _check_limit(int c){
  args->limit = c;
}

static void _check_concurrency(int c){
  args->concurrency = (c > MAX_CONCURRENCY) ? MAX_CONCURRENCY : c;
  args->concurrency = (args->concurrency < 1) ? 1 : args->concurrency;
  log_info("c:%d", args->concurrency);
}

static void _check_xml_file(char *xml_file_path){
  if (fsio_file_exists(xml_file_path) == false) {
    log_error("Invalid XML File '%s'", xml_file_path);
    exit(EXIT_FAILURE);
  }
  return(EXIT_FAILURE);
}

static void _check_input_icns_file(char *input_icns_file){
  if (fsio_file_exists(input_icns_file) == false) {
    log_error("Invalid Input ICNS Path '%s'", input_icns_file);
    exit(EXIT_FAILURE);
  }
  return(EXIT_FAILURE);
}

static void _check_output_icns_file(char *output_icns_file){
  if (!output_icns_file || strlen(output_icns_file) < 5) {
    log_error("Invalid output ICNS File");
    exit(EXIT_FAILURE);
  }
  return(EXIT_SUCCESS);
}

static void _check_output_file(char *output_file){
  if (!output_file || strlen(output_file) < 5) {
    log_error("Invalid output File");
    exit(EXIT_FAILURE);
  }
  return(EXIT_SUCCESS);
}

static void _check_output_mode(char *output_mode){
  for (size_t i = 1; i < OUTPUT_MODES_QTY && output_modes[i] != NULL; i++) {
    if (strcmp(output_mode, output_modes[i]) == 0) {
      args->output_mode = i;
      exit(EXIT_SUCCESS);
    }
  }
  log_error("Invalid Output Mode '%s'", output_mode);
  exit(EXIT_FAILURE);
}

static void _check_height_group(uint16_t height){
  if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
    log_info("Validating Grouped Height %d", height);
  }
  args->width_or_height       = COMMON_OPTION_WIDTH_OR_HEIGHT_HEIGHT;
  args->width_or_height_group = height;
  args->height                = height;
  return(EXIT_SUCCESS);
}

static void _check_width_group(uint16_t width){
  if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
    log_info("Validating Grouped Width %d", width);
  }
  args->width_or_height       = COMMON_OPTION_WIDTH_OR_HEIGHT_WIDTH;
  args->width_or_height_group = width;
  args->width                 = width;
  return(EXIT_SUCCESS);
}

static void _check_window_id(uint16_t window_id){
  if (window_id < 1) {
    log_error("Window ID too small");
    goto do_error;
  }
  args->window_id = (int)window_id;
  args->window    = get_window_id_info((size_t)window_id);

  if (!args->window) {
    if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
      log_error("Window is Null");
    }
  }else{
    if ((size_t)args->window_id != (size_t)window_id) {
      log_error("Window id mismatch: %lu|%lu", (size_t)window_id, args->window->window_id);
      goto do_error;
    }
  }
  return(EXIT_SUCCESS);

do_error:
  log_error("Invalid Window ID %lu", (size_t)window_id);
  exit(EXIT_FAILURE);
}

////////////////////////////////////////////
static int hotkey_callback(char *KEYS){
  char                    *config_path = get_homedir_yaml_config_file_path();
  struct hotkeys_config_t *cfg         = load_yaml_config_file_path(config_path);

  if (DARWIN_LS_COMMANDS_DEBUG_MODE == true) {
    log_info("Keys: " AC_YELLOW "%s" AC_RESETALL "|" "hotkey config %s with %lu hotkeys", KEYS, config_path, cfg->keys_count);
  }
  struct key_t *key = get_hotkey_config_key(cfg, KEYS);

  if (key == NULL) {
    if (DARWIN_LS_COMMANDS_DEBUG_MODE == true) {
      log_debug("Non existent hotkey %s", KEYS);
    }
  }else{
    if (DARWIN_LS_COMMANDS_DEBUG_MODE == true) {
      log_debug("Existent hotkey %s", KEYS);
    }
    int rc = execute_hotkey_config_key(key);
    if (rc != EXIT_SUCCESS) {
      log_error("Failed to execute Action %s", key->action);
    }else{
      if (DARWIN_LS_COMMANDS_DEBUG_MODE == true) {
        log_debug("Executed Action %s", key->action);
      }
      return(EXIT_SUCCESS);
    }
  }
  return(EXIT_FAILURE);
}

static void _command_hotkeys(){
  exit((keylogger_exec_with_callback(hotkey_callback) == true) ? EXIT_SUCCESS : EXIT_FAILURE);
  exit(EXIT_SUCCESS);
}

static void _command_clear_icons_cache(){
  exit((clear_icons_cache() == true) ? EXIT_SUCCESS : EXIT_FAILURE);
  exit(EXIT_SUCCESS);
}

static void _command_open_security(){
  if (args->retries <= 0) {
    args->retries = OPEN_SECURITY_DEFAULT_RETRIES_QTY;
  }
  int ret = EXIT_FAILURE;
  for (int tries = 0; tries <= args->retries && ret == EXIT_FAILURE; tries++) {
    errno = 0;
    ret   = ((tesseract_security_preferences_logic() == true) ? EXIT_SUCCESS: EXIT_FAILURE);
    if (ret == EXIT_SUCCESS || (ret != EXIT_SUCCESS && tries >= args->retries)) {
      break;
    }
    tries++;
    log_error("tesseract_security_preferences_logic Returned %d> Attempt #%d/%d", ret, tries, args->retries);
    usleep(1000 * OPEN_SECURITY_RETRY_INTERVAL_MS);
  }
  if (ret) {
    log_error("tesseract_security_preferences_logic Failed");
  }else{
    log_info(AC_GREEN "tesseract_security_preferences_logic Success"AC_RESETALL);
  }
  exit(ret);
}

static void _command_paste(){
  errno = 0;
  char *pasteboard_content = read_clipboard();
  if (pasteboard_content == NULL) {
    log_error(AC_RESETALL AC_RED "pasteboard read failed." AC_RESETALL);
    exit(EXIT_FAILURE);
  }
  size_t len = (pasteboard_content != NULL) ? strlen(pasteboard_content) : 0;
  if (len < 1) {
    exit(EXIT_FAILURE);
  }
  fprintf(stdout, "%s\n", pasteboard_content);
  exit(EXIT_SUCCESS);
}

static void _command_copy(){
  bool ok = false;

  if (args->content == NULL) {
    asprintf(&(args->content), "%lld", timestamp());
  }
  if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
    log_info("copying '%s'", args->content);
  }
  ok = copy_clipboard(args->content);
  if (ok == false) {
    log_error("Failed to copy %s to clipboard", bytes_to_string(strlen(args->content)));
  }
  exit((ok == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_app_icns_path(){
  bool ok                   = false;
  char *app_plist_info_path = get_app_path_plist_info_path(args->application_path);

  if (DARWIN_LS_COMMANDS_DEBUG_MODE == true) {
    log_info("Application %s has plist file %s", args->application_path, app_plist_info_path);
  }
  char *app_icns_file_path = get_info_plist_icon_file_path(app_plist_info_path);

  if (DARWIN_LS_COMMANDS_DEBUG_MODE == true) {
    log_info("Application %s has icns file name %s", args->application_path, app_icns_file_path);
  }
  char *icns_file_path = get_app_path_icns_file_path_icon_file_path(args->application_path, app_icns_file_path);

  if (DARWIN_LS_COMMANDS_DEBUG_MODE == true) {
    log_info("Application %s has icns file %s", args->application_path, icns_file_path);
  }
  ok = (fsio_file_exists(icns_file_path)) ? true : false;
  if (ok == true) {
    fprintf(stdout, "%s\n", icns_file_path);
  }
  exit((ok == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_app_info_plist_path(){
  char *app_plist_info_path = get_app_path_plist_info_path(args->application_path);

  log_info("Application %s has plist info file %s", args->application_path, app_plist_info_path);
  exit((fsio_file_exists(app_plist_info_path) == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_parse_xml_file(){
  bool ok              = false;
  char *icns_file_path = get_info_plist_icon_file_path(args->xml_file_path);

  log_info("Parsed icns file path %s from xml path %s", icns_file_path, args->xml_file_path);

  exit((ok == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_icon_info(){
  bool ok = get_icon_info(args->input_icns_file);

  exit((ok == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_grayscale_png(){
  log_info("Converting %s to grayscale %s", args->input_file, args->output_file);
  bool       ok              = false;
  FILE       *input_png_file = fopen(args->input_file, "rb");
  CGImageRef png_gs          = png_file_to_grayscale_cgimage_ref_resized(input_png_file, args->resize_factor);
  ok = write_cgimage_ref_to_tif_file_path(png_gs, args->output_file);
  exit((ok == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_write_app_icon_from_png(){
  log_info("setting app icon from app %s from png file %s", args->application_path, args->input_png_file);

  bool ok = false;
  ok = write_app_icon_from_png(args->application_path, args->input_png_file);
  exit((ok == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_image_conversions(){
  log_debug("image conversions using file %s", args->input_file);
  bool ok = image_conversions(args->input_file);
  exit((ok == true) ? EXIT_SUCCESS : EXIT_FAILURE);
  exit(EXIT_SUCCESS);
}

static void _command_save_app_icon_to_png(){
  log_info("getting app icon from app %s and writing to png file %s", args->application_path, args->output_png_file);
  bool ok = write_app_icon_to_png(args->application_path, args->output_png_file, args->icon_size);
  exit((ok == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_save_app_icon_to_icns(){
  log_info("getting app icon from app %s and writing to icns file %s", args->application_path, args->output_icns_file);
  bool ok = false;
  ok = write_app_icon_to_icns(args->application_path, args->output_icns_file);
  exit((ok == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_write_app_icon_icns(){
  if (DARWIN_LS_COMMANDS_DEBUG_MODE == true) {
    log_info("setting app icon for app %s from icns %s", args->application_path, args->input_icns_file);
  }
  bool ok = write_icns_to_app_path(args->input_icns_file, args->application_path);
  if (ok == true) {
    fprintf(stdout, "Set Application %s icon using icns file %s\n", args->application_path, args->input_icns_file);
  }
  if (ok == true && isatty(STDOUT_FILENO) && args->clear_icons_cache == true) {
    ok = clear_app_icon_cache(args->application_path);
    if (ok == false) {
      log_error("Failed to clear icons cache");
    }else{
      fprintf(stderr, "Cleared icons cache\n");
    }
    if (DARWIN_LS_COMMANDS_DEBUG_MODE == true) {
      log_debug("Cleared icons cache");
    }
  }else{
    if (DARWIN_LS_COMMANDS_DEBUG_MODE == true) {
      log_info("not clearing icons cache");
    }
  }
  exit((ok == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_processes(){
  struct list_table_t *filter = &(struct list_table_t){
    .limit = args->limit, .font_family = args->font_family,
  };

  switch (args->output_mode) {
  case OUTPUT_MODE_TABLE: list_process_table(filter); break;
  case OUTPUT_MODE_JSON:
    break;
  case OUTPUT_MODE_TEXT:
    break;
  }
  exit(EXIT_SUCCESS);
}

static void _command_httpserver(){
  log_debug("Starting HTTP Server");
  exit(httpserver_main());
}

static void _command_set_space_index(){
  log_debug("setting space id from %d to %d", get_current_space_id(), args->space_id);
  exit(EXIT_SUCCESS);
}

static void _command_extract_window(){
  struct Vector *v = vector_new();

  if (args->all_windows == true) {
    struct Vector *a = get_window_infos_v();
    for (size_t i = 0; i < vector_size(a); i++) {
      if ((size_t)args->limit > 0 && vector_size(v) >= (size_t)args->limit) {
        continue;
      }
      struct window_info_t *w = (struct window_info_t *)vector_get(a, i);
      vector_push(v, (void *)(w->window_id));
    }
  }else if (args->window_id > 0) {
    vector_push(v, (void *)(size_t)(args->window_id));
  }
  log_info("%lu Windows", vector_size(v));
  struct Vector *r = tesseract_extract_windows(v, args->concurrency);

  log_info("%lu Results", vector_size(r));

  exit(EXIT_SUCCESS);
}

static void _command_animated_capture_window(){
  args->frame_rate       = clamp(args->frame_rate, 1, 50);
  args->width            = clamp(args->width, -1, 4000);
  args->height           = clamp(args->height, -1, 4000);
  args->concurrency      = clamp(args->concurrency, 1, args->limit);
  args->concurrency      = clamp(args->concurrency, 1, MAX_CONCURRENCY);
  args->duration_seconds = clamp(args->duration_seconds, 1, 300);
  log_debug("frame rate :  %d", args->frame_rate);
  log_debug("Duration sec :  %d", args->duration_seconds);
  log_debug("window id :  %d", args->window_id);
  log_debug("display :  %s", args->display_output_file?"Yes":"No");
  log_debug("compress :  %s", args->compress?"Yes":"No");
  log_debug("width :  %d", args->width);
  log_debug("height :  %d", args->height);
  log_debug("concurrency :  %d", args->concurrency);
  unsigned long animation_started = timestamp();
  unsigned long end_ts = animation_started + (args->duration_seconds * 1000);
  unsigned long interval_ms = (unsigned long)(1000 * (float)(((float)1) / (float)(args->frame_rate))), expected_frames_qty = args->duration_seconds * args->frame_rate;
  log_debug("Expected Frames :  %ld", expected_frames_qty);
  log_debug("start ts :  %ld", animation_started);
  log_debug("end ts :  %ld", end_ts);
  log_debug("ms dur :  %ld", end_ts - animation_started);
  log_debug("interval ms :  %ld", interval_ms);
  struct capture_req_t *req = calloc(1, sizeof(struct capture_req_t));
  req->ids          = vector_new();
  req->concurrency  = args->concurrency;
  req->type         = CAPTURE_TYPE_WINDOW;
  req->format       = IMAGE_TYPE_GIF;
  req->width        = args->width > 0 ? args->width : 0;
  req->height       = args->height > 0 ? args->height : 0;
  req->time.started = timestamp();
  req->time.dur     = 0;
  struct animated_capture_t *acap    = init_animated_capture(CAPTURE_TYPE_WINDOW, IMAGE_TYPE_GIF, args->window_id, interval_ms);
  struct Vector             *results = NULL;
  struct window_info_t      *w       = get_window_id_info((size_t)(args->window_id));
  if (w && w->window_id == (size_t)(args->window_id)) {
    vector_push(req->ids, (void *)(size_t)(args->window_id));
  }else{
    log_error("Window ID #%lu not found", (size_t)(args->window_id));
    exit(EXIT_FAILURE);
  }
  unsigned long prev_ts = 0, last_ts = 0, delta_ms = 0;
  while ((unsigned long)timestamp() < (unsigned long)end_ts || expected_frames_qty > vector_size(acap->frames_v)) {
    unsigned long s = timestamp();
    results  = capture_windows(req);
    prev_ts  = last_ts;
    last_ts  = timestamp();
    delta_ms = last_ts - prev_ts;
    for (size_t i = 0; i < vector_size(results); i++) {
      struct capture_result_t *r = (struct capture_result_t *)vector_get(results, i);
      assert(new_animated_frame(acap, r) == true);
      log_info("Frame #%lu/%lu> Received %lu Results in %s"
               "\n\tID:%lu|File:%s|Size:%s|Delta ms:%s|Time left:%lldms|"
               "\n\t" AC_RESETALL AC_YELLOW "Total Frames Size:%s" AC_RESETALL "|" AC_GREEN "Avg Frame Size:%s" AC_RESETALL "|"
               "\n\t%s",
               vector_size(acap->frames_v), expected_frames_qty,
               vector_size(results),
               milliseconds_to_string(last_ts - s),
               r->id,
               r->file,
               bytes_to_string(fsio_file_size(r->file)),
               (vector_size(acap->frames_v) > 1) ? milliseconds_to_string(delta_ms) : "0ms",
               end_ts - timestamp(),
               bytes_to_string(acap->total_size),
               bytes_to_string(acap->total_size / vector_size(acap->frames_v)),
               ""
               );
    }
    while ((unsigned long)timestamp() < ((unsigned long)(last_ts + interval_ms - (delta_ms / 5)))) {
      usleep((interval_ms / 50) * 1000);
    }
  }
  assert(end_animation(acap) == true);
  timg_utils_image(acap->file);

  exit(EXIT_SUCCESS);
} /* _command_animated_capture_window */

static void _command_capture_window(){
  args->width       = clamp(args->width, -1, 4000);
  args->height      = clamp(args->height, -1, 4000);
  args->limit       = clamp(args->limit, 1, 999);
  args->concurrency = clamp(args->concurrency, 1, args->limit);
  args->concurrency = clamp(args->concurrency, 1, MAX_CONCURRENCY);
  if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
    log_debug("all windows:  %s", args->all_windows?"Yes":"No");
    log_debug("display :  %s", args->display_output_file?"Yes":"No");
    log_debug("compress :  %s", args->compress?"Yes":"No");
    log_debug("width :  %d", args->width);
    log_debug("concurrency :  %d", args->concurrency);
    log_debug("height :  %d", args->height);
    log_debug("format :  %s", args->image_format);
    log_debug("format type:  %d", args->image_format_type);
  }
  struct capture_req_t *req = calloc(1, sizeof(struct capture_req_t));
  req->ids          = vector_new();
  req->concurrency  = args->concurrency;
  req->type         = CAPTURE_TYPE_WINDOW;
  req->format       = args->image_format_type;
  req->width        = args->width > 0 ? args->width : 0;
  req->height       = args->height > 0 ? args->height : 0;
  req->time.started = timestamp();
  req->time.dur     = 0;
  struct Vector *all_windows = NULL, *results = NULL;

  if (args->all_windows == true) {
    all_windows = get_window_infos_v();
    for (size_t i = 0; (i < vector_size(all_windows)) && (((size_t)(args->limit) > 0) ? (vector_size(req->ids) < (size_t)(args->limit))  : true); i++) {
      struct window_info_t *w = (struct window_info_t *)vector_get(all_windows, i);
      if (w && w->window_id > 0) {
        vector_push(req->ids, (void *)(w->window_id));
      }
    }
  }else if (args->window_id > 0) {
    struct window_info_t *w = get_window_id_info((size_t)(args->window_id));
    if (w && w->window_id == (size_t)(args->window_id)) {
      vector_push(req->ids, (void *)(size_t)(args->window_id));
    }else{
      log_error("Window ID #%lu not found", (size_t)(args->window_id));
      exit(EXIT_FAILURE);
    }
  }
  results = capture_windows(req);
  log_info("%lu Results", vector_size(results));
  exit(EXIT_SUCCESS);
} /* _command_capture_window */
/*
 * static void __command_capture_window(){
 * if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
 *  log_debug("all windows:  %s", args->all_windows?"Yes":"No");
 *  log_debug("display :  %s", args->display_output_file?"Yes":"No");
 *  log_debug("compress :  %s", args->compress?"Yes":"No");
 *  log_debug("width :  %d", args->width);
 *  log_debug("concurrency :  %d", args->concurrency);
 *  log_debug("height :  %d", args->height);
 *  log_debug("format :  %s", args->image_format);
 *  log_debug("format type:  %d", args->image_format_type);
 * }
 * struct capture_windows_t *cap = calloc(1,sizeof(struct capture_windows_t));
 * cap->window_ids = vector_new();
 * cap->concurrency = args->concurrency;
 * cap->type = args->image_format_type;
 * cap->width = args->width  > 0 ? args->width : 0;
 * cap->height = args->height > 0 ? args->height : 0;
 * struct Vector *all_windows = NULL, *results = NULL;
 *
 * if (args->all_windows == true) {
 *  all_windows = get_window_infos_v();
 *  for (size_t i = 0; (i < vector_size(all_windows)) && (((size_t)(args->limit) > 0) ? (vector_size(cap->window_ids) < (size_t)(args->limit))  : true); i++) {
 *    struct window_info_t *w = (struct window_info_t *)vector_get(all_windows, i);
 *    if (w && w->window_id > 0) {
 *      vector_push(cap->window_ids, (void *)(w->window_id));
 *    }
 *  }
 * }else if (args->window_id > 0) {
 *  struct window_info_t *w = get_window_id_info((size_t)(args->window_id));
 *  if (w && w->window_id == (size_t)(args->window_id)) {
 *    vector_push(cap->window_ids, (void *)(size_t)(args->window_id));
 *  }else{
 *    log_error("Window ID #%lu not found", (size_t)(args->window_id));
 *  }
 * }
 * results = capture_windows(cap);
 * for (size_t i = 0; (i < vector_size(results)); i++) {
 *  struct capture_result_t *r = (struct capture_result_t *)vector_get(results, i);
 *  if (args->output_file && i == 0) {
 *    unsigned char *data = fsio_read_binary_file(r->file);
 *    if (data) {
 *      if (fsio_write_binary_file(args->output_file, data, fsio_file_size(r->file)) == false) {
 *        log_error("Failed to write file %s", args->output_file);
 *      }
 *      free(data);
 *    }
 *  }
 *  if(args->display_output_file)
 *    timg_utils_titled_image(r->file);
 *  else
 *    log_info("Capture Result #%lu/%lu: %s %s (type %s)",
 *           i + 1, vector_size(results),
 *           bytes_to_string(fsio_file_size(r->file)),
 *           r->file,
 *           image_type_name(r->type)
 *           );
 * }
 * exit(EXIT_SUCCESS);
 * } */

static void _command_list_font(){
  struct list_table_t *filter = &(struct list_table_t){
    .sort_key       = stringfn_to_lowercase(args->sort_key),
    .sort_direction = stringfn_to_lowercase(args->sort_direction),
    .limit          = args->limit, .font_family = args->font_family,
    .exact_match    = args->exact_match, .case_sensitive = args->case_sensitive,
    .font_name      = args->font_name, .font_style = args->font_style, .font_type = args->font_type,
    .duplicate      = args->duplicate, .non_duplicate = args->non_duplicate,
  };

  switch (args->output_mode) {
  case OUTPUT_MODE_TABLE:
    list_font_table(filter);
    break;
  case OUTPUT_MODE_JSON:
    break;
  case OUTPUT_MODE_TEXT:
    break;
  }
  exit(EXIT_SUCCESS);
}

static void _command_move_window(){
  struct window_info_t *w = get_window_id_info(args->window_id);

  if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
    log_debug("moving window %lu to %dx%d", w->window_id, args->x, args->y);
  }
  exit((move_window_id(args->window_id, args->x, args->y) == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_resize_window(){
  struct window_info_t *w = get_window_id_info(args->window_id);

  log_debug("resizing window %lu to %dx%d", w->window_id, args->width, args->height);
  resize_window_info(w, args->width, args->height);
  exit(EXIT_SUCCESS);
}

static void _command_dock(){
  bool   is_visible        = dock_is_visible();
  CGSize offset            = dock_offset();
  CGRect rect              = dock_rect();
  int    orientation       = dock_orientation();
  char   *orientation_name = dock_orientation_name();

  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Dock" AC_RESETALL
    "\n\t    Dock Visible?                   %s" AC_RESETALL
    "\n\t           Size:                   %dx%d"
    "\n\t       Position:                   %dx%d"
    "\n\t         Offset:                   %dx%d"
    "\n\t    Orientation:                   %s (%d)"
    "\n",
    (is_visible == true) ? AC_GREEN "Yes" : AC_RED "No",
    (int)rect.size.width, (int)rect.size.height,
    (int)rect.origin.x, (int)rect.origin.y,
    (int)offset.width, (int)offset.height,
    orientation_name, orientation
    );

  exit(EXIT_SUCCESS);
}

static void _command_menu_bar(){
  bool   _is_visible = get_menu_bar_visible();
  int    _space_id   = get_current_space_id();
  int    _display_id = get_display_id_for_space(_space_id);
  CGRect _rect       = get_menu_bar_rect(_display_id);
  int    _height     = get_menu_bar_height();

  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Menu Bar" AC_RESETALL
    "\n\tMenu Bar info is visible?      %s" AC_RESETALL
    "\n\tMenu Bar Size:                 %dx%d"
    "\n\tMenu Bar Position:             %dx%d"
    "\n\tMenu Bar Height:               %dpixels"
    "\n%s",
    (_is_visible == true) ? AC_GREEN "Yes" : AC_RED "No",
    (int)_rect.size.width, (int)_rect.size.height,
    (int)_rect.origin.x, (int)_rect.origin.y,
    _height,
    ""
    );

  exit(EXIT_SUCCESS);
}

static void _command_sticky_window(){
  exit(EXIT_SUCCESS);
}

static void _command_windows(){
  unsigned long started = timestamp();

  switch (args->output_mode) {
  case OUTPUT_MODE_TABLE:
    list_window_table(&(struct list_table_t){
      .sort_key             = args->sort_key, .sort_direction = args->sort_direction,
      .current_space_only   = args->current_space_only,
      .current_display_only = args->current_display_only,
      .space_id             = args->space_id,
      .display_id           = args->display_id,
      .window_id            = args->window_id,
      .pid                  = args->pid,
      .height_greater       = args->height_greater, .height_less = args->height_less,
      .width_greater        = args->width_greater, .width_less = args->width_less,
      .application_name     = args->application_name,
      .width                = args->width, .height = args->height, .limit = args->limit,
    });
    break;
  case OUTPUT_MODE_JSON:
    break;
  case OUTPUT_MODE_TEXT:
    break;
  }
  if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
    log_debug("Window Infos in %s", milliseconds_to_string(timestamp() - started));
  }
  exit(EXIT_SUCCESS);
}

static void _command_window_level(){
  int level = get_window_id_level((size_t)args->window_id);

  log_debug("Window #%d Level: %d", args->window_id, level);
  exit(EXIT_SUCCESS);
}

static void _command_window_layer(){
  int layer = get_window_layer(get_window_id_info((size_t)args->window_id));

  log_debug("Window #%d Layer: %d", args->window_id, layer);
  exit(EXIT_SUCCESS);
}

static void _command_window_is_minimized(){
  bool is_minimized = get_window_id_is_minimized((size_t)args->window_id);

  log_debug("Window #%d is Minimized? %s", args->window_id, (is_minimized == true) ? "Yes" : "No");
  exit(EXIT_SUCCESS);
}

static void _command_window_id_info(){
  struct window_info_t *w = get_window_id_info(args->window_id);

  log_debug("window ID %lu", w->window_id);
  exit(EXIT_SUCCESS);
}

static void _command_pid_is_minimized(){
  print_all_window_items(stdout);
  bool is_minimized = get_pid_is_minimized(args->pid);
  log_debug("PID %d is Minimized? %s", args->pid, is_minimized ? "Yes" : "No");
  exit(EXIT_SUCCESS);
}

static void _command_minimize_window(){
  exit((minimize_window_id(args->window_id) == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_set_window_all_spaces(){
  exit(EXIT_SUCCESS);
}

static void _command_set_window_space(){
  log_info("%d|%d", args->window_id, args->space_id);
  exit(((set_window_id_to_space((size_t)(args->window_id), (int)(args->space_id))) == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_set_space(){
  if (args->space_id == get_current_space_id()) {
    exit(EXIT_SUCCESS);
  }
  struct Vector        *v = get_window_infos_v(), *wins = vector_new();
  struct window_info_t *w;
  for (size_t i = 0; i < vector_size(v); i++) {
    w = (struct window_info_t *)vector_get(v, i);
    if (!w || (w->space_id != (size_t)args->space_id)) {
      vector_remove(v, i);
    }
  }
  uint64_t   set_tags        = 1;
  uint64_t   clear_tags      = 0;
  uint32_t   options         = 0x2;
  uint64_t   space_list[1]   = { (uint64_t)(args->space_id) };
  CFArrayRef space_list_ref  = cfarray_of_cfnumbers(space_list, sizeof(uint64_t), 1, kCFNumberSInt64Type);
  CFArrayRef window_list_ref = SLSCopyWindowsWithOptionsAndTags(g_connection, 0, space_list_ref, options, &set_tags, &clear_tags);
  if (window_list_ref) {
    int       qty      = CFArrayGetCount(window_list_ref);
    CFTypeRef query    = SLSWindowQueryWindows(g_connection, window_list_ref, qty);
    CFTypeRef iterator = SLSWindowQueryResultCopyWindows(query);

    while (SLSWindowIteratorAdvance(iterator)) {
      size_t parent_wid = (size_t)SLSWindowIteratorGetParentID(iterator);
      size_t wid        = (size_t)SLSWindowIteratorGetWindowID(iterator);
      log_debug("%lu|%lu", wid, parent_wid);
      for (size_t i = 0; i < vector_size(v); i++) {
        w = (struct window_info_t *)vector_get(v, i);
        if (w->window_id == wid) {
          vector_push(wins, (void *)w);
        }
      }
    }
  }
  log_debug("now have %lu wids", vector_size(wins));
  for (size_t i = 0; i < vector_size(wins); i++) {
    w = (struct window_info_t *)vector_get(wins, i);
    log_info("setting window #%lu @ Level %d | Layer %d",
             w->window_id,
             get_window_id_level(w->window_id),
             window_layer(w)
             );
    focus_window_id(w->window_id);
    usleep(500 * 1000);
    int cur = get_current_space_id();
    log_debug("%d/%d", cur, args->space_id);
    if (cur == args->space_id) {
      exit(EXIT_SUCCESS);
    }
  }
  exit(EXIT_FAILURE);
} /* _command_set_space */

static void _command_displays(){
  switch (args->output_mode) {
  case OUTPUT_MODE_TABLE:
    list_displays_table(&(struct list_table_t){
      .limit = args->limit,
    });
    break;
  case OUTPUT_MODE_JSON:
    log_info("display json");
    break;
  case OUTPUT_MODE_TEXT:
    print_displays();
    break;
  }
  exit(EXIT_SUCCESS);
}

static void _command_spaces(){
  switch (args->output_mode) {
  case OUTPUT_MODE_TABLE:
    list_spaces_table(&(struct list_table_t){
      .limit = args->limit,
    });
    break;
  case OUTPUT_MODE_JSON:
    break;
  case OUTPUT_MODE_TEXT:
    break;
  }

  exit(EXIT_SUCCESS);
}

static void _command_focus_space(){
  log_info("Focusing space #%d", args->space_id);
  exit(EXIT_SUCCESS);
}

static void _command_focus_window(){
  focus_window_id(args->window_id);
  exit(EXIT_SUCCESS);
}

static void _command_focused_space(){
  unsigned long       started                            = timestamp();
  int                 _space_id                          = get_current_space_id();
  int                 _space_display_id                  = get_display_id_for_space(_space_id);
  int                 _space_type                        = get_space_type(_space_id);
  char                *_space_uuid                       = get_space_uuid(_space_id);
  bool                _space_management_mode             = get_space_management_mode(_space_id);
  bool                _space_is_fullscreen               = get_space_is_fullscreen(_space_id);
  bool                _space_is_visible                  = get_space_is_visible(_space_id);
  bool                _space_is_user                     = get_space_is_user(_space_id);
  bool                _space_is_system                   = get_space_is_system(_space_id);
  bool                _space_is_active                   = get_space_is_active(_space_id);
  bool                _space_can_create_tile             = get_space_can_create_tile(_space_id);
  CGRect              _space_rect                        = get_space_rect(_space_id);
  struct Vector       *_space_window_ids_v               = get_space_window_ids_v(_space_id);
  struct Vector       *_space_minimized_window_ids_v     = get_space_minimized_window_ids_v(_space_id);
  struct Vector       *_space_non_minimized_window_ids_v = get_space_non_minimized_window_ids_v(_space_id);
  struct Vector       *_space_owners                     = get_space_owners(_space_id);
  struct StringBuffer *sb;
  char                *_space_window_ids_csv, *_space_owners_csv;
  {
    sb = stringbuffer_new();
    for (size_t i = 0; i < vector_size(_space_window_ids_v); i++) {
      if (i == 0 || ((i % 6) == 0)) {
        stringbuffer_append_string(sb, "\n                                             ");
      }else{
        stringbuffer_append_string(sb, ", ");
      }
      stringbuffer_append_unsigned_long_long(sb, (unsigned long long)vector_get(_space_window_ids_v, i));
    }
    _space_window_ids_csv = stringbuffer_to_string(sb);
    stringbuffer_release(sb);
  }
  {
    sb = stringbuffer_new();
    for (size_t i = 0; i < vector_size(_space_owners); i++) {
      if (i > 0 && i < vector_size(_space_owners) - 1) {
        stringbuffer_append_string(sb, ", ");
      }
      stringbuffer_append_unsigned_long_long(sb, (unsigned long long)vector_get(_space_owners, i));
    }
    _space_owners_csv = stringbuffer_to_string(sb);
    stringbuffer_release(sb);
  }

  unsigned long dur = timestamp() - started;

  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Focused Space" AC_RESETALL
    "\n\tSpace ID                    :       %d"
    "\n\tType                        :       %d"
    "\n\tUUID                        :       %s"
    "\n\t# Window IDs                :       %lu" AC_RESETALL AC_BLUE "%s" AC_RESETALL "\n"
    "\n\t# Minimized Window IDs      :       %lu"
    "\n\t# Non Minimized Window IDs  :       %lu"
    "\n\t# Owners                    :       %lu" AC_RESETALL "\t" AC_CYAN "%s" AC_RESETALL
    "\n\tDisplay ID                  :       %d"
    "\n\tIs Fullscreen               :       %s" AC_RESETALL
    "\n\tIs Visible                  :       %s" AC_RESETALL
    "\n\tIs User                     :       %s" AC_RESETALL
    "\n\tIs System                   :       %s" AC_RESETALL
    "\n\tIs Active                   :       %s" AC_RESETALL
    "\n\tCan Create Tile             :       %s" AC_RESETALL
    "\n\tManagement Mode             :       %d"
    "\n\tSize                        :       %dx%d" AC_RESETALL
    "\n\tPosition                    :       %dx%d" AC_RESETALL
    "\n\tDuration                    :       %s" AC_RESETALL
    "\n%s",
    _space_id,
    _space_type,
    _space_uuid,
    vector_size(_space_window_ids_v), _space_window_ids_csv,
    vector_size(_space_minimized_window_ids_v),
    vector_size(_space_non_minimized_window_ids_v),
    vector_size(_space_owners), _space_owners_csv,
    _space_display_id,
    (_space_is_fullscreen == true) ? AC_GREEN "Yes" : AC_RED "No",
    (_space_is_visible == true) ? AC_GREEN "Yes" : AC_RED "No",
    (_space_is_user == true) ? AC_GREEN "Yes" : AC_RED "No",
    (_space_is_system == true) ? AC_GREEN "Yes" : AC_RED "No",
    (_space_is_active == true) ? AC_GREEN "Yes" : AC_RED "No",
    (_space_can_create_tile == true) ? AC_GREEN "Yes" : AC_RED "No",
    _space_management_mode,
    (int)_space_rect.size.width, (int)_space_rect.size.height,
    (int)_space_rect.origin.x, (int)_space_rect.origin.y,
    milliseconds_to_string(dur),
    ""
    );

  exit(EXIT_SUCCESS);
} /* _command_focused_space */

static void _command_focused_pid(){
  int pid = get_focused_pid();

  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Focused PID" AC_RESETALL
    "     PID:              %d"
    "\n%s",
    pid,
    ""
    );

  exit(EXIT_SUCCESS);
}

static void _command_focused_window(){
  struct window_info_t *w = get_focused_window();

  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Focused Window" AC_RESETALL
    "\n\tWindow ID    :       %lu"
    "\n\tPID          :       %d"
    "\n\tApplication  :       %s"
    "\n\tSpace ID     :       %lu"
    "\n\tDisplay ID   :       %lu"
    "\n",
    w->window_id,
    w->pid,
    w->name,
    w->space_id,
    w->display_id
    );

  exit(EXIT_SUCCESS);
}

static void _command_alacrittys(){
  struct Vector *_alacritty_pids = get_alacritty_pids();

  fprintf(stdout,
          "\t" AC_YELLOW AC_UNDERLINE "Alacrittys" AC_RESETALL
          "\n\t# Alacritty Terminals      :       %lu"
          "%s",
          vector_size(_alacritty_pids),
          "\n"
          );
  for (size_t i = 0; i < vector_size(_alacritty_pids); i++) {
    fprintf(stdout,
            "\t\t" AC_CYAN AC_BOLD "%lu" AC_RESETALL
            "%s",
            (size_t)vector_get(_alacritty_pids, i),
            "\n"
            );
  }
  vector_release(_alacritty_pids);

  exit(EXIT_SUCCESS);
}

char *common_option_width_or_height_name(enum common_option_width_or_height_t width_or_height){
  switch (width_or_height) {
  case COMMON_OPTION_WIDTH_OR_HEIGHT_HEIGHT: return("height"); break;
  case COMMON_OPTION_WIDTH_OR_HEIGHT_WIDTH: return("width"); break;
  default: return("UNKNOWN"); break;
  }
}

char *get_command_about(enum command_type_t COMMAND_ID){
  char *about;

  asprintf(&about, AC_RESETALL "%s\t%s%s" AC_RESETALL,
           cmds[COMMAND_ID].icon,
           cmds[COMMAND_ID].color,
           cmds[COMMAND_ID].description
           );
  return(about);
}
static void __attribute__((constructor)) __constructor__darwin_ls_commands(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_DARWIN_LS_COMMANDS") != NULL) {
    log_debug("Enabling Darwin Ls Debug Mode");
    DARWIN_LS_COMMANDS_DEBUG_MODE = true;
  }
}
#define LIST_HANDLER(NAME)                                                \
  static void _command_list_ ## NAME(){                                   \
    struct list_table_t *filter = &(struct list_table_t){                 \
      .limit    = args->limit,                                            \
      .sort_key = args->sort_key, .sort_direction = args->sort_direction, \
    };                                                                    \
    switch (args->output_mode) {                                          \
    case OUTPUT_MODE_TABLE: list_ ## NAME ## _table(filter); break;       \
    case OUTPUT_MODE_JSON:                                                \
      break;                                                              \
    case OUTPUT_MODE_TEXT:                                                \
      break;                                                              \
    }                                                                     \
    exit(EXIT_SUCCESS);                                                   \
  }

LIST_HANDLER(usb)
LIST_HANDLER(kitty)
LIST_HANDLER(app)
LIST_HANDLER(hotkey)
LIST_HANDLER(monitor)
#endif
