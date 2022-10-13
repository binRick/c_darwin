#pragma once
#ifndef LS_WIN_COMMANDS_C
#define LS_WIN_COMMANDS_C
#define OPEN_SECURITY_RETRY_INTERVAL_MS      1000
#define OPEN_SECURITY_DEFAULT_RETRIES_QTY    3
#include "c_vector/vector/vector.h"
#include "tinydir/tinydir.h"
#include "capture/utils/utils.h"
#include "image/utils/utils.h"
#include "capture/animate/animate.h"
#include "capture/type/type.h"
#include "system/utils/utils.h"
#include "process/utils/utils.h"
#include "clamp/clamp.h"
#include "dls/dls-commands.h"
#include "dls/dls.h"
#include "hotkey-utils/hotkey-utils.h"
#include "httpserver-utils/httpserver-utils.h"
#include "httpserver/httpserver.h"
#include "keylogger/keylogger.h"
#include "kitty/msg/msg.h"
#include "pasteboard/pasteboard.h"
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
#include "table/sort/sort.h"
#include "table/utils/utils.h"
#include "tesseract/utils/utils.h"
#include "vips/vips.h"
#include "wildcardcmp/wildcardcmp.h"
#include "window/utils/utils.h"
#include "capture/save/save.h"
#define MAX_CONCURRENCY   25
#define MAX_FRAME_RATE    30
#define MAX_LIMIT         99
#define MAX_DURATION      300
#define MAX_WINDOW_SIZE   4000
#define MAX_DURATION      300
#define IS_COMMAND_VERBOSE_MODE (args->verbose_mode||DARWIN_LS_COMMANDS_DEBUG_MODE)
#define IS_COMMAND_DEBUG_MODE (args->debug_mode||DARWIN_LS_COMMANDS_DEBUG_MODE)
#define IS_COMMAND_VERBOSE_OR_DEBUG_MODE (IS_COMMAND_DEBUG_MODE||IS_COMMAND_VERBOSE_MODE)
#define CAPTURE_IMAGE_TERMINAL_DISPLAY_SIZE 300
#define MIN_FILE_EXTENSION_LENGTH 3
#define debug(M, ...)    {            \
      do {                              \
        if (IS_COMMAND_VERBOSE_OR_DEBUG_MODE) {\
          log_debug(M, ## __VA_ARGS__); \
        }                               \
      } while (0); }
struct capture_mode_t;
enum arg_clamp_type_t {
  ARG_CLAMP_TYPE_WINDOW_SIZE,
  ARG_CLAMP_TYPE_FRAME_RATE,
  ARG_CLAMP_TYPE_LIMIT,
  ARG_CLAMP_TYPE_CONCURRENCY,
  ARG_CLAMP_TYPE_DURATION,
  ARG_CLAMP_TYPES_QTY,
};
static const struct arg_clamp_t {
  signed long min, max;
} arg_clamps[] = {
 [ARG_CLAMP_TYPE_WINDOW_SIZE] = {.min = -1, .max = MAX_WINDOW_SIZE},
 [ARG_CLAMP_TYPE_FRAME_RATE] =  {.min =  1, .max = MAX_FRAME_RATE},
 [ARG_CLAMP_TYPE_CONCURRENCY] = {.min =  1, .max = MAX_CONCURRENCY},
 [ARG_CLAMP_TYPE_DURATION] =    {.min =  1, .max = MAX_DURATION},
 [ARG_CLAMP_TYPE_LIMIT] =       {.min =  1, .max = MAX_LIMIT},
};
#define CLAMP_ARG_TYPE(ARGS,ARG,TYPE)\
  clamp(ARGS->ARG,arg_clamps[TYPE].min,arg_clamps[TYPE].max)
bool initialize_args(struct args_t *ARGS){
  ARGS->concurrency      = CLAMP_ARG_TYPE(ARGS,concurrency,ARG_CLAMP_TYPE_CONCURRENCY);
  ARGS->duration_seconds = CLAMP_ARG_TYPE(ARGS,duration_seconds,ARG_CLAMP_TYPE_DURATION);
  ARGS->frame_rate       = CLAMP_ARG_TYPE(ARGS,frame_rate, ARG_CLAMP_TYPE_FRAME_RATE);
  ARGS->width       = CLAMP_ARG_TYPE(ARGS,width, ARG_CLAMP_TYPE_WINDOW_SIZE);
  ARGS->height       = CLAMP_ARG_TYPE(ARGS,height, ARG_CLAMP_TYPE_WINDOW_SIZE);
  ARGS->limit      = CLAMP_ARG_TYPE(ARGS,limit, ARG_CLAMP_TYPE_LIMIT);
  ARGS->concurrency      = clamp(args->concurrency, 1, args->limit);
  if(!ARGS->format_ids_v)
    ARGS->format_ids_v = vector_new();
  if(!ARGS->formats_v)
    ARGS->formats_v = vector_new();
  if(vector_size(ARGS->format_ids_v)==0){
    vector_push(ARGS->format_ids_v,(void*)(IMAGE_TYPE_PNG));
  }
  return(true);
}
static bool DARWIN_LS_COMMANDS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__darwin_ls_commands(void);
static void debug_dls_arguments(){
  if (!IS_COMMAND_DEBUG_MODE)
    return;
  log_debug("All Mode:  %s", args->all_mode?"Yes":"No");
  log_debug("display :  %s", args->display_mode?"Yes":"No");
  log_debug("Write Images? :  %s", args->write_images_mode?"Yes":"No");
  log_debug("purge write dir :  %s", args->purge_write_directory_before_write?"Yes":"No");
  log_debug("write dir :  %s", args->write_directory);
  log_debug("compress :  %s", args->compress?"Yes":"No");
  log_debug("width :  %d", args->width);
  log_debug("height :  %d", args->height);
  log_debug("concurrency :  %d", args->concurrency);
  log_debug("write dir :  %s", args->write_directory);
  log_debug("progress bar enabled:  %s", args->progress_bar_mode?"Yes":"No");
  log_debug("Format IDs :  %lu", vector_size(args->format_ids_v));
  log_debug("Formats :  %lu", vector_size(args->formats_v));
  log_debug("limit :  %d", args->limit);
  log_debug("frame rate :  %d", args->frame_rate);
  log_debug("Duration sec :  %d", args->duration_seconds);
  log_debug("window id :  %d", args->id);
  log_debug("display :  %s", args->display_mode?"Yes":"No");
  log_debug("compress :  %s", args->compress?"Yes":"No");
}
static const struct {
  struct Vector *(*get_structs_v_function)(void);
  size_t (^get_struct_index_pk)(struct Vector *structs, size_t index);
} capture_type_getters[] = {
  [CAPTURE_TYPE_WINDOW] = { 
    .get_structs_v_function = get_window_infos_v, 
    .get_struct_index_pk = ^size_t (struct Vector *structs, size_t index){
      return( ((struct window_info_t*)vector_get(structs,index))->window_id );
    },
  },
  [CAPTURE_TYPE_SPACE] = { 
    .get_structs_v_function = get_spaces_v, 
    .get_struct_index_pk = ^size_t (struct Vector *structs, size_t index){
      return( ((struct space_t*)vector_get(structs,index))->id );
    },
  },
  [CAPTURE_TYPE_DISPLAY] = { 
    .get_structs_v_function = get_displays_v, 
    .get_struct_index_pk = ^size_t (struct Vector *structs, size_t index){
      return( ((struct display_t*)vector_get(structs,index))->display_id );
    },
  },
};
static struct Vector *get_all_capture_type_ids(enum capture_type_id_t id, size_t limit){
  struct Vector *structs = capture_type_getters[id].get_structs_v_function(), *ids = vector_new();
  for(size_t i = 0; i <vector_size(structs) && vector_size(ids) < limit;i++)
    vector_push(ids,(void*)capture_type_getters[id].get_struct_index_pk(structs,i));
  vector_release(structs);
  return(ids);
}
static struct Vector *get_ids(enum capture_type_id_t type, bool all, size_t limit, bool random, size_t id){
  struct Vector *ids = NULL;
  if (all) {
    ids = get_all_capture_type_ids(type, limit);
  }else if (args->id > 0) {
    ids = vector_new();
    vector_push(ids, (void*)(size_t)(id));
  }
  return(ids);
}
static const struct capture_mode_t {
  unsigned long started, dur;
  struct Vector *ids, *requests, *results;
  union mode_request_t {
    struct capture_image_request_t capture;
    struct capture_animation_request_t animation;
  } request;
  union mode_result_t {
    struct capture_image_result_t capture;
    struct capture_animation_result_t animation;
  } result;
  struct mode_result_handled_t {
    unsigned long started, dur;
  } handled;
  union mode_request_t *(^request_creator)(enum capture_type_id_t type, struct Vector *ids_v);
  union mode_result_t *(^request_handler)(union mode_request_t *req);
  struct mode_result_handled_t *(^result_handler)(union mode_result_t *res);
} capture_modes[CAPTURE_MODE_TYPES_QTY] = {
  [CAPTURE_MODE_TYPE_IMAGE] = {
      .request_creator = ^union mode_request_t *(enum capture_type_id_t type, struct Vector *ids_v){
          struct capture_image_request_t *req = calloc(1, sizeof(struct capture_image_request_t));
          req->ids          = ids_v;
          req->concurrency  = args->concurrency;
          req->type         = CAPTURE_TYPE_WINDOW;
          req->compress         = args->compress;
          req->progress_bar_mode = true;
          req->format       = (enum image_type_id_t)(size_t)vector_get(args->format_ids_v,0);
          req->width        = args->width > 0 ? args->width : 0;
          req->height       = args->height > 0 ? args->height : 0;
          req->compress = args->compress;
          req->time.started = timestamp();
          req->time.dur     = 0;
        return(req);
      },
      .request_handler = ^union mode_result_t *(union mode_request_t *req){
        return(capture_image(req));
      },
      .result_handler = ^struct mode_result_handled_t *(union mode_result_t *res){
        struct mode_result_handled_t *handled = calloc(1, sizeof(struct mode_result_handled_t));
      //    struct capture_image_result_t *r;
        for (size_t i = 0; i < vector_size(res); i++) {
      //    r = (struct capture_image_result_t *)vector_get(res, i);
        }
        return(handled);
      },
  },
  [CAPTURE_MODE_TYPE_ANIMATION] = {
      .request_creator = ^union mode_request_t *(enum capture_type_id_t type, struct Vector *ids_v){
          struct capture_animation_request_t *req = calloc(1, sizeof(struct capture_animation_request_t));
          struct capture_image_request_t *img = calloc(1, sizeof(struct capture_image_request_t));
          img->ids        = ids_v;
          img->concurrency  = args->concurrency;
          img->type         = CAPTURE_TYPE_WINDOW;
          img->progress_bar_mode = false;
          img->compress = args->compress;
          img->format       = IMAGE_TYPE_GIF;
          img->width        = args->width > 0 ? args->width : 0;
          img->height       = args->height > 0 ? args->height : 0;
          img->time.started = timestamp();
          img->time.dur     = 0;
        return(req);
      },
      .request_handler = ^union mode_result_t *(union mode_request_t *req){
        struct capture_animation_result_t *res = calloc(1, sizeof(struct capture_animation_result_t));
        return(res);
      },
      .result_handler = ^struct mode_result_handled_t *(union mode_result_t *res){
        struct mode_result_handled_t *handled = calloc(1, sizeof(struct mode_result_handled_t));
        return(handled);
      },
  },
};
////////////////////////////////////////////
static void _command_move_window();
static void _command_window_id_info();
static void _command_resize_window();
static void _command_minimize_window();
static void _command_set_window_space();
static void _command_set_space();
static void _check_write_directory(void);
static void _command_copy();
static void _command_paste();
static void _command_image_conversions();
static void _command_set_space_index();
static void _command_set_window_all_spaces();
static void _command_list_window();
static void _command_list_display();
static void _command_focus_window();
static void _command_focus_space();
static void _command_focused_window();
static void _command_focused_pid();
static void _command_focused_space();
static void _command_list_space();
static void _command_create_space(void);
static void _command_sticky_window();
static void _command_menu_bar();
static void _command_list_usb();
static void _command_httpserver();
static void _command_dock();
static void _command_list_process();
static void _command_list_app();
static void _command_list_monitor();
static void _command_list_font();
static void _command_list_kitty();
static void _command_list_alacritty();
static void _command_capture();
static void _command_animate();
static void _command_extract();
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
static void _command_list_hotkey();
////////////////////////////////////////////
static void _check_run_hotkeys(void);
static void _check_id(size_t id);
static void _check_clear_screen(void);
static void _check_formats(char *formats);
static void _check_sort_direction_desc(void);
static void _check_capture_window_mode(void);
static void _check_capture_space_mode(void);
static void _check_capture_display_mode(void);
static void _check_sort_direction_asc(void);
static void _check_random_id(void);
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
/////////////////////////////////////////////////////
#define COMMON_OPTION_CAPTURE_MODE(TYPE,NAME,UCFIRST)\
  [COMMON_OPTION_CAPTURE_##TYPE##_MODE] = ^ struct optparse_opt (struct args_t *args){\
    return((struct optparse_opt){\
      .long_name = NAME,\
      .group = COMMON_OPTION_GROUP_CAPTURE_MODE,\
      .flag_type = FLAG_TYPE_SET_TRUE,\
      .flag = &(args->capture_mode[CAPTURE_TYPE_##TYPE]),\
      .function = check_cmds[CHECK_COMMAND_CAPTURE_##TYPE##_MODE].fxn,\
      .description =   COLOR_CAPTURE_MODE "Capture" AC_RESETALL " " \
               COLOR_CAPTURE_##TYPE##_MODE UCFIRST AC_RESETALL,\
    });\
  }\
/////////////////////////////////////////////////////
  COMMON_OPTION_CAPTURE_MODE(DISPLAY,"display","Display"),
  COMMON_OPTION_CAPTURE_MODE(SPACE,"space","Space"),
  COMMON_OPTION_CAPTURE_MODE(WINDOW,"window","Window"),
#undef COMMON_OPTION_CAPTURE_MODE
/////////////////////////////////////////////////////
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
      .description = get_sort_type_by_description(SORT_TYPE_APP),
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
  [COMMON_OPTION_ALL_MODE] = ^ struct optparse_opt (struct args_t *args)                                 {
    return((struct optparse_opt)                                                                            {
      .short_name = 'A',
      .long_name = "all",
      .description = "All IDs",
      .group = COMMON_OPTION_GROUP_ID,
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->all_mode),
    });
  },
  [COMMON_OPTION_NOT_MINIMIZED] = ^ struct optparse_opt (struct args_t *args)                               {
    return((struct optparse_opt)                                                                            {
      .long_name = "non-minimized",
      .description = "Show Non Minimized Only",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->not_minimized_only),
    });
  },
  [COMMON_OPTION_MINIMIZED] = ^ struct optparse_opt (struct args_t *args)                                   {
    return((struct optparse_opt)                                                                            {
      .long_name = "minimized",
      .description = "Show Minimized Only",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->minimized_only),
    });
  },
  [COMMON_OPTION_RANDOM_ID] = ^ struct optparse_opt (struct args_t *args)                            {
    return((struct optparse_opt)                                                                            {
      .short_name = 'r',
      .long_name = "random-id",
      .description = "Random Capture ID",
      .function = check_cmds[CHECK_COMMAND_RANDOM_ID].fxn,
      .group = COMMON_OPTION_GROUP_ID,
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
  [COMMON_OPTION_NOT_CURRENT_DISPLAY] = ^ struct optparse_opt (struct args_t *args)                         {
    return((struct optparse_opt)                                                                            {
      .long_name = "not-current-display",
      .description = "Windows not on Currently Focused Display only",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->not_current_display_only),
    });
  },
  [COMMON_OPTION_CURRENT_DISPLAY] = ^ struct optparse_opt (struct args_t *args)                             {
    return((struct optparse_opt)                                                                            {
      .long_name = "current-display",
      .description = "Windows on Currently Focused Display only",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->current_display_only),
    });
  },
  [COMMON_OPTION_NOT_CURRENT_SPACE] = ^ struct optparse_opt (struct args_t *args)                           {
    return((struct optparse_opt)                                                                            {
      .long_name = "not-current-space",
      .description = "Windows not on Currently Focused Space only",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->not_current_space_only),
    });
  },
  [COMMON_OPTION_CURRENT_SPACE] = ^ struct optparse_opt (struct args_t *args)                               {
    return((struct optparse_opt)                                                                            {
      .long_name = "current-space",
      .description = "Windows on Currently Focused Space only",
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
  [COMMON_OPTION_WRITE_DIRECTORY] = ^ struct optparse_opt (struct args_t *args)                            {
    return((struct optparse_opt)                                                                            {
      .long_name = "dir",
      .description = "Write Files to Specified Directory",
      .arg_name = "DIRECTORY",
      .arg_data_type = check_cmds[CHECK_COMMAND_WRITE_DIRECTORY].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_WRITE_DIRECTORY].fxn,
      .arg_dest = &(args->write_directory),
    });
  },
  [COMMON_OPTION_PURGE_WRITE_DIRECTORY_BEFORE_WRITE] = ^ struct optparse_opt (struct args_t *args)                            {
    return((struct optparse_opt)                                                                            {
      .long_name = "purge",
      .description = "Purge Contents of Write Directory Before any Writes. Must Prefix --dir",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->purge_write_directory_before_write),
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
      .group = COMMON_OPTION_GROUP_ID,
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
  [COMMON_OPTION_IMAGE_FORMATS] = ^ struct optparse_opt (struct args_t *args)                                {
    return((struct optparse_opt)                                                                            {
      .short_name = 'f',
      .long_name = "formats",
      .description = get_image_format_names_csv(),
      .arg_name = "IMAGE-FORMATS-CSV",
      .arg_data_type = check_cmds[CHECK_COMMAND_IMAGE_FORMATS].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_IMAGE_FORMATS].fxn,
      .arg_dest = &(args->formats),
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
  [COMMON_OPTION_NOT_DUPLICATE] = ^ struct optparse_opt (struct args_t *args)                               {
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
  [COMMON_OPTION_DEBUG_MODE] = ^ struct optparse_opt (struct args_t *args)                                     {
    return((struct optparse_opt)                                                                            {
      .short_name = 'd',
      .long_name = "debug",
      .description = "Debug Mode",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->debug_mode),
    });
  },
  [COMMON_OPTION_VERBOSE_MODE] = ^ struct optparse_opt (struct args_t *args)                                     {
    return((struct optparse_opt)                                                                            {
      .short_name = 'v',
      .long_name = "verbose",
      .description = "Verbose Mode",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->verbose_mode),
    });
  },
  [COMMON_OPTION_ID] = ^ struct optparse_opt (struct args_t *args)                                   {
    return((struct optparse_opt)                                                                            {
      .short_name = 'i',
      .long_name = "id",
      .description = "ID",
      .arg_name = "ID",
      .arg_data_type = DATA_TYPE_UINT64,
      .arg_dest = &(args->id),
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
  [COMMON_OPTION_GRAYSCALE_MODE] = ^ struct optparse_opt (struct args_t *args)                            {
    return((struct optparse_opt)                                                                            {
      .long_name = "grayscale",
      .description = "Grayscale",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->grayscale_mode),
    });
  },
  [COMMON_OPTION_DISPLAY_OUTPUT_FILE] = ^ struct optparse_opt (struct args_t *args)                         {
    return((struct optparse_opt)                                                                            {
      .short_name = 'D',
      .long_name = "display-output-file",
      .description = "Display Output File",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->display_mode),
    });
  },
  [COMMON_OPTION_DISABLE_PROGRESS_BAR_MODE] = ^ struct optparse_opt (struct args_t *args){
    return((struct optparse_opt){
      .long_name = "no-progress",
      .description = "Disable Progress Bar",
      .group = COMMON_OPTION_GROUP_PROGRESS_BAR_MODE,
      .flag_type = FLAG_TYPE_SET_FALSE,
      .flag = &(args->progress_bar_mode),
    });
  },
  [COMMON_OPTION_WRITE_IMAGES_MODE] = ^ struct optparse_opt (struct args_t *args){
    return((struct optparse_opt)                                                                            {
      .long_name = "write",
      .description = "Write Images",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->write_images_mode),
    });
  },
  [COMMON_OPTION_ENABLE_PROGRESS_BAR_MODE] = ^ struct optparse_opt (struct args_t *args){
    return((struct optparse_opt)                                                                            {
      .short_name = 'p',
      .long_name = "progress",
      .description = "Enable Progress Bar",
      .group = COMMON_OPTION_GROUP_PROGRESS_BAR_MODE,
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->progress_bar_mode),
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
  [COMMON_OPTION_RUN_HOTKEYS] = ^ struct optparse_opt (struct args_t *args)                                 {
    return((struct optparse_opt)                                                                            {
      .short_name = 'r',
      .long_name = "run",
      .description = "Run Hotkeys Daemon",
      .function = check_cmds[CHECK_COMMAND_RUN_HOTKEYS].fxn,
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
  [CHECK_COMMAND_WRITE_DIRECTORY] =               {
    .fxn           = (void (*)(void))(*_check_write_directory),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_RANDOM_ID] =               {
    .fxn           = (void (*)(void))(*_check_random_id),
  },
  [CHECK_COMMAND_LIMIT] =               {
    .fxn           = (void (*)(void))(*_check_limit),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_CAPTURE_WINDOW_MODE] = { .fxn = (void (*)(void))(*_check_capture_window_mode), },
  [CHECK_COMMAND_CAPTURE_SPACE_MODE] = { .fxn = (void (*)(void))(*_check_capture_space_mode), },
  [CHECK_COMMAND_CAPTURE_DISPLAY_MODE] = { .fxn = (void (*)(void))(*_check_capture_display_mode), },
  [CHECK_COMMAND_RUN_HOTKEYS] =         {
    .fxn = (void (*)(void))(*_check_run_hotkeys),
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
  [CHECK_COMMAND_ID] =    {
    .fxn           = (void (*)(void))(*_check_id),
    .arg_data_type = DATA_TYPE_UINT64,
  },
  [CHECK_COMMAND_PID] =                 {
    .fxn           = (void (*)(void))(*_check_pid),
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
  [CHECK_COMMAND_PROGRESS_BAR_MODE] =        {
    .arg_data_type = FLAG_TYPE_SET_TRUE,
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
  [CHECK_COMMAND_IMAGE_FORMATS] =        {
    .fxn           = (void (*)(void))(*_check_formats),
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
  [COMMAND_MOVE_WINDOW] =           {
    .name = "move-window",           .icon = "🪟", .color = COLOR_WINDOW, .description = "Move Window",
    .fxn  = (*_command_move_window),
  },
  [COMMAND_RESIZE_WINDOW] =         {
    .name = "resize-window",           .icon = "💡", .color = COLOR_WINDOW, .description = "Resize Window",
    .fxn  = (*_command_resize_window),
  },
  [COMMAND_WINDOW_LEVEL] =          {
    .name        = "window-level",           .icon = "🔅", .color = COLOR_WINDOW,
    .description = "Window Level",
    .fxn         = (*_command_window_level),
  },
  [COMMAND_WINDOW_LAYER] =          {
    .name        = "window-layer",           .icon = "🔅", .color = COLOR_WINDOW,
    .description = "Window Layer",
    .fxn         = (*_command_window_layer),
  },
  [COMMAND_WINDOW_IS_MINIMIZED] =   {
    .name        = "window-minimized",              .icon = "🔅", .color = COLOR_WINDOW,
    .description = "Window is Minimized",
    .fxn         = (*_command_window_is_minimized),
  },
  [COMMAND_PID_IS_MINIMIZED] =      {
    .name        = "pid-minimized",              .icon = "🔅", .color = COLOR_WINDOW,
    .description = "PID is Minimized",
    .fxn         = (*_command_pid_is_minimized),
  },
  [COMMAND_MINIMIZE_WINDOW] =       {
    .name        = "minimize-window",           .icon = "🔅", .color = COLOR_WINDOW,
    .description = "Minimize Window",
    .fxn         = (*_command_minimize_window),
  },
  [COMMAND_SET_WINDOW_SPACE] =      {
    .fxn = (*_command_set_window_space)
  },
  [COMMAND_FOCUS_SPACE] =           {
    .name = "focus-space",           .icon = "🔅", .color = COLOR_SPACE, .description = "Focus Space",
    .fxn  = (*_command_focus_space),
  },
  [COMMAND_FOCUS_WINDOW] =          {
    .name = "focus-window",           .icon = "🔅", .color = COLOR_WINDOW, .description = "Focus Window",
    .fxn  = (*_command_focus_window),
  },
  [COMMAND_SET_WINDOW_ALL_SPACES] = {
    .fxn = (*_command_set_window_all_spaces)
  },
  [COMMAND_SET_SPACE] =             {
    .name = "set-space",           .icon = "💣", .color = COLOR_SPACE, .description = "Set Space",
    .fxn  = (*_command_set_space),
  },
  [COMMAND_SET_SPACE_INDEX] =       {
    .fxn = (*_command_set_space_index)
  },
  [COMMAND_WINDOW_ID_INFO] =        {
    .name        = "window-id-info",           .icon = "🔅", .color = COLOR_WINDOW,
    .description = "Window ID Info",
    .fxn         = (*_command_window_id_info),
  },
  [COMMAND_SECURITY] =              {
    .name = "security", .icon = "🐾", .color = COLOR_WINDOW, .description = "Open Security",
    .fxn  = (*_command_open_security)
  },
  [COMMAND_HTTPSERVER] =            {
    .fxn = (*_command_httpserver)
  },
  [COMMAND_STICKY_WINDOW] =         {
    .name        = "sticky-window",     .icon = "🕰", .color = COLOR_WINDOW,
    .description = "Set Window Sticky",
    .fxn         = (*_command_sticky_window)
  },
  [COMMAND_MENU_BAR] =              {
    .name        = "menu-bar",      .icon = "📀", .color = COLOR_SHOW,
    .description = "Menu Bar Info",
    .fxn         = (*_command_menu_bar)
  },
  [COMMAND_DOCK] =                  {
    .name        = "dock",      .icon = "📡", .color = COLOR_SHOW,
    .description = "Dock Info",
    .fxn         = (*_command_dock)
  },
  [COMMAND_CREATE_SPACE] =         {
    .name        = "create-space", .icon = "💮", .color = COLOR_LIST,
    .description = "Create Space",
    .fxn         = (*_command_create_space)
  },
  [COMMAND_FOCUSED_SPACE] =         {
    .name        = "focused-space", .icon = "💮", .color = COLOR_LIST,
    .description = "Focused Space",
    .fxn         = (*_command_focused_space)
  },
  [COMMAND_FOCUSED_WINDOW] =        {
    .name        = "focused-window", .icon = "💮", .color = COLOR_LIST,
    .description = "Focused Window",
    .fxn         = (*_command_focused_window)
  },
  [COMMAND_FOCUSED_PID] =           {
    .name        = "focused-pid", .icon = "🌈", .color = COLOR_LIST,
    .description = "Focused PID",
    .fxn         = (*_command_focused_pid)
  },
  [COMMAND_CAPTURE] =               {
    .name        = "capture",          .icon = "💤", .color = COLOR_CAPTURE,
    .description = "Capture Screenshot",
    .fxn = (*_command_capture)
  },
  [COMMAND_ANIMATE] =      {
    .name        = "animate",          .icon = "💤", .color = COLOR_CAPTURE,
    .description = "Animated Capture",
    .fxn         = (*_command_animate)
  },  
  [COMMAND_EXTRACT] =        {
    .name        = "extract",          .icon = "🙀", .color = COLOR_CAPTURE,
    .description = "Extract Capture",
    .fxn = (*_command_extract)
  },
  [COMMAND_IMAGE_CONVERSIONS] =     {
    .name        = "image-conversions", .icon = "💮", .color = COLOR_LIST,
    .description = "Image Conversions",
    .fxn         = (*_command_image_conversions)
  },
  [COMMAND_PASTE] =                 {
    .name        = "paste",           .icon = "💮", .color = COLOR_LIST,
    .description = "Clipboard Paste",
    .fxn         = (*_command_paste)
  },
  [COMMAND_COPY] =                  {
    .name        = "copy",           .icon = "💮", .color = COLOR_LIST,
    .description = "Clipboard Copy",
    .fxn         = (*_command_copy)
  },
  [COMMAND_SAVE_APP_ICON_ICNS] =    {
    .fxn = (*_command_save_app_icon_to_icns)
  },
  [COMMAND_SAVE_APP_ICON_PNG] =     {
    .fxn = (*_command_save_app_icon_to_png)
  },
  [COMMAND_SET_APP_ICON_PNG] =      {
    .fxn = (*_command_write_app_icon_from_png)
  },
  [COMMAND_SET_APP_ICON_ICNS] =     {
    .fxn = (*_command_write_app_icon_icns)
  },
  [COMMAND_ICON_INFO] =             {
    .fxn = (*_command_icon_info)
  },
  [COMMAND_APP_PLIST_INFO_PATH] =   {
    .fxn = (*_command_app_info_plist_path)
  },
  [COMMAND_APP_ICNS_PATH] =         {
    .fxn = (*_command_app_icns_path)
  },
  [COMMAND_PARSE_XML_FILE] =        {
    .fxn = (*_command_parse_xml_file)
  },
  [COMMAND_GRAYSCALE_PNG_FILE] =    {
    .fxn = (*_command_grayscale_png)
  },
  [COMMAND_CLEAR_ICONS_CACHE] =     {
    .fxn = (*_command_clear_icons_cache)
  },
//////////////////////////////////////////////////////////  
#define LIST_SUBCOMMAND(CAPS,LOWER,PLURAL,FXN)\
  [COMMAND_##CAPS] = { .name = LOWER, .icon = "🥑", .color = COLOR_LIST, .description = "List " PLURAL, .fxn  = *FXN, }\
//////////////////////////////////////////////////////////  
  LIST_SUBCOMMAND(SPACES,"spaces","Spaces",_command_list_space),
  LIST_SUBCOMMAND(DISPLAYS,"displays","Displays",_command_list_display),
  LIST_SUBCOMMAND(APPS,"apps","Applications",_command_list_app),
  LIST_SUBCOMMAND(FONTS,"fonts","Fonts",_command_list_font),
  LIST_SUBCOMMAND(PROCESSES,"processes","Processes",_command_list_process),
  LIST_SUBCOMMAND(KITTYS,"kittys","Kittys",_command_list_kitty),
  LIST_SUBCOMMAND(USBS,"usbs","USB Devices",_command_list_usb),
  LIST_SUBCOMMAND(MONITORS,"monitors","Monitors",_command_list_monitor),
  LIST_SUBCOMMAND(HOTKEYS,"hotkeys","Hot Keys",_command_list_hotkey),
  LIST_SUBCOMMAND(WINDOWS,"windows","Windows",_command_list_window),
  LIST_SUBCOMMAND(ALACRITTYS,"alacrittys","Alacrittys",_command_list_alacritty),
#undef LIST_SUBCOMMAND
  [COMMAND_TYPES_QTY] =             { 0},
};

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

////////////////////////////////////////////
static void _check_formats(char *formats){
  if(formats)
    formats = stringfn_mut_trim(formats);
  if(strcmp(formats,"all")==0){
    formats = get_image_format_names_csv();
  }

  args->formats_v = vector_new();
  args->format_ids_v = vector_new();
  char *msg[2] = { 0 };
  struct StringBuffer *sb[2];
  sb[0] = stringbuffer_new();
  sb[1] = stringbuffer_new();
  struct StringFNStrings split = stringfn_split(formats,',');
  char *line=NULL;
  for(int i=0;i<split.count;i++){
    line = stringfn_mut_trim(split.strings[i]);
    if(strcmp(line,"cgimage")==0)
      continue;
    if(strcmp(line,"rgb")==0)
      continue;
    if(strcmp(line,"bmp")==0)
      continue;
    debug(" - %s",line);
    vector_push(args->formats_v,(void*)line);
    vector_push(args->format_ids_v,(void*)get_format_name(line));
  }
  for(size_t i=0;i<vector_size(args->formats_v);i++){
    if(i<vector_size(args->formats_v) && i > 0){
      stringbuffer_append_string(sb[0],", ");
      stringbuffer_append_string(sb[1],", ");
    }
    stringbuffer_append_string(sb[0],(char*)(vector_get(args->formats_v,i)));
    stringbuffer_append_int(sb[1],(int)(size_t)(vector_get(args->format_ids_v,i)));
  }
  msg[0] = stringbuffer_to_string(sb[0]);
  msg[1] = stringbuffer_to_string(sb[1]);
  stringbuffer_release(sb[0]);
  stringfn_release_strings_struct(split);
  debug("Checking %lu formats: %s, %lu format ids: %s", 
      vector_size(args->formats_v),msg[0],
      vector_size(args->format_ids_v), msg[1]
      );
  if(msg[0])free(msg[0]);
  if(msg[1])free(msg[1]);
}

static void _check_run_hotkeys(){
  exit((hotkeys_exec_with_callback(hotkey_callback) == true) ? EXIT_SUCCESS : EXIT_FAILURE);
  return(EXIT_SUCCESS);
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

static void _check_random_id(void){
  switch(args->capture_type){
    case CAPTURE_TYPE_WINDOW: args->id = get_random_window_info()->window_id; break;
    default: log_error("Invalid capture type"); return(EXIT_FAILURE); break;
  }
  if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
    log_debug("random id:%d|",
              args->id
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

static void _check_capture_mode(enum capture_type_id_t type){
  if(IS_COMMAND_DEBUG_MODE)
    log_info("Setting capture mode to %d|%s", type, get_capture_type_name(type));
  args->capture_type = type;
  return;
}
static void _check_capture_space_mode(void){ return(_check_capture_mode(CAPTURE_TYPE_SPACE)); }
static void _check_capture_display_mode(void){ return(_check_capture_mode(CAPTURE_TYPE_DISPLAY)); }
static void _check_capture_window_mode(void){ return(_check_capture_mode(CAPTURE_TYPE_WINDOW)); }
static void _check_concurrency(int c){
  args->concurrency = (c > MAX_CONCURRENCY) ? MAX_CONCURRENCY : c;
  args->concurrency = (args->concurrency < 1) ? 1 : args->concurrency;
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

static void _check_write_directory(void){
  char *test_files[3], *new_dir;

  if(args->purge_write_directory_before_write && fsio_dir_exists(args->write_directory)){
    asprintf(&new_dir,"%s/.%s-%lld-dls-purged-dir",
        dirname(args->write_directory),
        basename(args->write_directory),
        timestamp()
        );
    if(DARWIN_LS_COMMANDS_DEBUG_MODE)
      log_info("moving %s to %s",args->write_directory,new_dir);
    errno=0;
    if(!process_utils_move_directory_contents(args->write_directory,new_dir)){
      log_error("Failed to move \"%s\" to \"%s\"",args->write_directory,new_dir);
      exit(EXIT_FAILURE);
    }
  }
  if(!stringfn_starts_with(args->write_directory,"/")){
    asprintf(&(args->write_directory),"%s/%s",
        gettempdir(),
        args->write_directory
        );
  }
  asprintf(&test_files[0],".dls-test-file-%lld-%d.%s",timestamp(),getpid(),"txt");
  asprintf(&test_files[1],"%s/%s",args->write_directory,test_files[0]);
  asprintf(&test_files[2],"%d",getpid());

  errno=0;
  if(!fsio_dir_exists(args->write_directory) && 
    (!fsio_mkdirs(args->write_directory,S_IRWXU | S_IRWXG | S_IRWXO)||!fsio_dir_exists(args->write_directory)))
     goto fail;
  if(!fsio_write_text_file(test_files[1],test_files[2])|| (atoi(fsio_read_text_file(test_files[1]))!=getpid())|| !fsio_remove(test_files[1]))
     goto fail;

  char td[PATH_MAX];
  asprintf(&td,"%s/",args->write_directory);
  return(EXIT_SUCCESS);
fail:
  fprintf(stderr, "Invalid \"Write Directory\" "AC_YELLOW "%s"AC_RESETALL": "AC_RED"%s"AC_RESETALL
      "\n",  
      args->write_directory,
      strerror(errno) ? strerror(errno) : "Unknown Error"
      );
  exit(EXIT_FAILURE);
}
static void _check_id(size_t id){
  log_info("id:%lu",id);
  if (id < 1) {
    log_error("ID too small");
    goto do_error;
  }
  args->id = id;
  log_info("id:%d",args->id);
  return(EXIT_SUCCESS);

do_error:
  log_error("Invalid ID %lu", (size_t)id);
  exit(EXIT_FAILURE);
}

////////////////////////////////////////////

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
    ret   = ((tesseract_security_preferences_logic(args->space_id) == true) ? EXIT_SUCCESS: EXIT_FAILURE);
    if (ret == EXIT_SUCCESS || (ret != EXIT_SUCCESS && tries >= args->retries)) {
      break;
    }
    tries++;
    log_warn("tesseract_security_preferences_logic Returned %d> Attempt #%d/%d", ret, tries, args->retries);
    usleep(1000 * OPEN_SECURITY_RETRY_INTERVAL_MS);
  }
  if (ret) {
    log_error("tesseract_security_preferences_logic Failed");
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


static void _command_httpserver(){
  log_debug("Starting HTTP Server");
  exit(httpserver_main());
}

static void _command_set_space_index(){
  log_debug("setting space id from %d to %d", get_current_space_id(), args->space_id);
  exit(EXIT_SUCCESS);
}


static void _command_animate(){
  initialize_args(args);
  debug_dls_arguments();
  unsigned long end_ts = 0;
  unsigned long interval_ms = (unsigned long)(1000 * (float)(((float)1) / (float)(args->frame_rate))), expected_frames_qty = args->duration_seconds * args->frame_rate;
  struct Vector *results = NULL, *ids = get_ids(args->capture_type, args->all_mode, args->limit, args->random_ids_mode, args->id);
  for (size_t x = 0; x < vector_size(ids); x++) {
    struct capture_image_request_t *req = calloc(1, sizeof(struct capture_image_request_t));
    unsigned long             prev_ts = 0, last_ts = 0, delta_ms = 0;
    req->ids        = vector_new();
    args->id = (size_t)vector_get(ids, x);
    vector_push(req->ids, (void *)(size_t)vector_get(ids, x));
    req->concurrency  = args->concurrency;
    req->type         = CAPTURE_TYPE_WINDOW;
    req->progress_bar_mode = args->progress_bar_mode;
    req->compress = args->compress;
    req->format       = IMAGE_TYPE_GIF;
    req->width =    args->width;
    req->height =    args->height;
    req->time.dur     = 0;
    req->time.started = timestamp();
    struct capture_animation_result_t *acap = init_animated_capture(CAPTURE_TYPE_WINDOW, req->format, args->id, interval_ms, args->progress_bar_mode);
    acap->expected_frames_qty = expected_frames_qty;
    size_t                    qty = vector_size(acap->frames_v);
  end_ts = timestamp() + (args->duration_seconds * 1000);
    while ((unsigned long)timestamp() < (unsigned long)end_ts){// || expected_frames_qty > qty) {
      unsigned long s = timestamp();
      req->progress_bar_mode = false;
//      req->format = IMAGE_TYPE_CGIMAGE;
      results  = capture_image(req);
      prev_ts  = last_ts;
      last_ts  = timestamp();
      delta_ms = last_ts - prev_ts;
      size_t q = vector_size(results);
      for (size_t i = 0; i < q; i++) {
        struct capture_image_result_t *r = (struct capture_image_result_t *)vector_get(results, i);
        r->delta_ms = delta_ms;
        req->time.started = i == 0 ? timestamp() : req->time.started;
        chan_send(acap->chan, (void *)r);
//        get_cputime();
        if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
          log_info("Frame #%lu/%lu> Received %lu Results in %s"
                   "\n\tID:%lu|Size:%s|Delta ms:%s|Time left:%lldms|"
                   "\n\t%s",
                   qty, expected_frames_qty,
                   vector_size(results),
                   milliseconds_to_string(last_ts - s),
                   r->id,
                   bytes_to_string(fsio_file_size(r->file)),
                   (qty > 1) ? milliseconds_to_string(r->delta_ms) : "0ms",
                   end_ts - timestamp(),
                   ""
                   );
        }
      }
      while ((unsigned long)timestamp() < ((unsigned long)(last_ts + interval_ms - (delta_ms / 5)))) {
        usleep((interval_ms / 10) * 1000);
      }
      pthread_mutex_lock(acap->mutex);
      qty = vector_size(acap->frames_v);
      pthread_mutex_unlock(acap->mutex);
    }
    chan_close(acap->chan);
    chan_recv(acap->done, NULL);
    pthread_join(acap->thread, NULL);
    assert(end_animation(acap) == true);
    assert(fsio_file_exists(acap->file));
    if (args->output_file) {
      fsio_copy_file(acap->file, args->output_file);
    }
    if (args->display_mode) {
      kitty_display_image_path(acap->file);
      printf("\n");
    }
  }

  exit(EXIT_SUCCESS);
} /* _command_animated_capture*/


static char *get_type_format_output_file(size_t id, char *dir, enum capture_type_id_t type, enum image_type_id_t format_id){
  char *output_file = NULL;
  asprintf(&output_file,"%s/%s-%lu.%s", 
    dir ? dir : gettempdir(),
    stringfn_to_lowercase(get_capture_type_name(type)),
    id,
    image_type_extension(format_id)
  );
  return(output_file);
}

static bool set_result_filenames(char *dir, enum capture_type_id_t type, enum image_type_id_t format_id, struct Vector *results_v){
  struct capture_image_result_t *r = NULL;
  for (size_t i = 0; i < vector_size(results_v); i++) {
    r = (struct capture_image_result_t *)vector_get(results_v, i);
    if(r)
      r->file = get_type_format_output_file(r->id, dir, type, format_id);
  }
  return(true);
}

static bool save_results(char *dir, enum capture_type_id_t type, enum image_type_id_t format_id, struct Vector *results_v, bool concurrency, bool progress_bar_mode){
    struct save_capture_result_t *res = save_capture_type_results(type, format_id, results_v, concurrency, dir, progress_bar_mode);
    fprintf(stderr," ✅ Wrote"
        " "
        AC_CYAN "%2lu"AC_RESETALL
        " "
        AC_BLUE"%4s"AC_RESETALL 
        " "
        "Files totaling"
        " "
        AC_GREEN"%6s"AC_RESETALL 
        " "
        "to"
        " "
AC_DOTTED_UNDERLINE AC_ITALIC AC_YELLOW "%s" AC_RESETALL
        " "
        "in"
        " "
AC_BOLD AC_YELLOW"%6s"AC_RESETALL 
        "\n",
                res->qty,
                image_type_name(format_id),
                bytes_to_string(res->bytes),
                args->write_directory,
                milliseconds_to_string(res->dur)
    );
//  if(res)
  //  free(res);
  return(true);
}

static bool display_results(struct Vector *results_v){
    struct capture_image_result_t *r = NULL;
    for (size_t i = 0; i < vector_size(results_v); i++) {
      r = (struct capture_image_result_t *)vector_get(results_v, i);
      if (r->len > 0 && r->pixels) {
        debug("Displaying %lux%lu %s File", r->width,r->height,bytes_to_string(r->len));
        if(kitty_display_image_buffer_resized_width(r->pixels, r->len,CAPTURE_IMAGE_TERMINAL_DISPLAY_SIZE))
          printf("\n");
      }
    }
    if(r)free(r);
    return(true);
}


static struct Vector *capture(enum image_type_id_t format_id, struct Vector *ids, int concurrency, bool compress, enum capture_type_id_t capture_type, bool progress_bar_mode, int width, int height){
  struct Vector *results_v;
  struct capture_image_request_t *req = calloc(1, sizeof(struct capture_image_request_t));
  req->ids = ids;
  req->concurrency  = clamp(concurrency, 1, vector_size(req->ids));
  req->format         = format_id;
  req->compress         = compress;
  req->type = capture_type;
  req->progress_bar_mode = progress_bar_mode;     
  req->width        = width > 0 ? width : 0;
  req->height       = height > 0 ? height : 0;
  req->time.dur     = 0;
  req->time.started = timestamp();
  results_v = capture_image(req);
  //free(req);
  return(results_v);
}


static void _command_extract(){
  if(DARWIN_LS_COMMANDS_DEBUG_MODE)
    log_info("Capturing using mode %d|%s", args->capture_type,get_capture_type_name(args->capture_type));
  initialize_args(args);
  debug_dls_arguments();
  struct Vector *results = NULL, *ids = get_ids(args->capture_type, args->all_mode, args->limit, args->random_ids_mode, args->id);
  debug("Extracting %lu Items", vector_size(ids));
  results = tesseract_extract_items(ids, args->concurrency);
  log_info("Extracted %lu Results", vector_size(results));
  exit(EXIT_SUCCESS);
}
static void _command_capture(){
  initialize_args(args);
  debug_dls_arguments();
  struct list_table_t *filter = &(struct list_table_t){
    .sort_key       = stringfn_to_lowercase(args->sort_key),
    .sort_direction = stringfn_to_lowercase(args->sort_direction),
    .limit          = args->limit, 
  };
  struct Vector *ids =  get_ids(args->capture_type, args->all_mode, args->limit, args->random_ids_mode, args->id);
  for(size_t i = 0; i <vector_size(args->format_ids_v);i++){
    struct Vector *results_v = capture(
        (enum image_type_id_t)(size_t)vector_get(args->format_ids_v,i), 
        ids, 
        args->concurrency, 
        args->compress, 
        args->capture_type, 
        args->progress_bar_mode, 
        args->width, 
        args->height
    );
    set_result_filenames(args->write_directory, args->capture_type, (enum image_type_id_t)(size_t)vector_get(args->format_ids_v,i), results_v);
    if(args->write_images_mode){
      errno=0;
      if(!save_results(args->write_directory, args->capture_type, (enum image_type_id_t)(size_t)vector_get(args->format_ids_v,i), results_v, args->concurrency, args->progress_bar_mode)){
        log_error("Failed to save results");
      }
    }
    if (args->display_mode){
      errno=0;
      if(!display_results(results_v)){
        log_error("Failed to display results");
      }
    }

    switch (args->output_mode) {
    case OUTPUT_MODE_TABLE:
      if(false)
        list_captured_window_table(filter);
      break;
    case OUTPUT_MODE_JSON:
      break;
    case OUTPUT_MODE_TEXT:
      break;
    }
}


  exit(EXIT_SUCCESS);
} /* _command_capture*/


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
  struct window_info_t *w = get_window_id_info(args->id);

  if (DARWIN_LS_COMMANDS_DEBUG_MODE) {
    log_debug("moving window %lu to %dx%d", w->window_id, args->x, args->y);
  }
  exit((move_window_id(args->id, args->x, args->y) == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_resize_window(){
  struct window_info_t *w = get_window_id_info(args->id);

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

static void _command_list_window(){
  unsigned long started = timestamp();

  switch (args->output_mode) {
  case OUTPUT_MODE_TABLE:
    list_window_table(&(struct list_table_t){
      .sort_key                 = args->sort_key, .sort_direction = args->sort_direction,
      .current_space_only       = args->current_space_only,
      .current_display_only     = args->current_display_only,
      .not_minimized_only       = args->not_minimized_only,
      .minimized_only           = args->minimized_only,
      .not_current_display_only = args->not_current_display_only,
      .not_current_space_only   = args->not_current_space_only,
      .space_id                 = args->space_id,
      .display_id               = args->display_id,
      .window_id                = args->id,
      .pid                      = args->pid,
      .height_greater           = args->height_greater, .height_less = args->height_less,
      .width_greater            = args->width_greater, .width_less = args->width_less,
      .application_name         = args->application_name,
      .width                    = args->width, .height = args->height, .limit = args->limit,
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
  int level = get_window_id_level((size_t)args->id);

  log_debug("Window #%d Level: %d", args->id, level);
  exit(EXIT_SUCCESS);
}

static void _command_window_layer(){
  int layer = get_window_layer(get_window_id_info((size_t)args->id));

  log_debug("Window #%d Layer: %d", args->id, layer);
  exit(EXIT_SUCCESS);
}

static void _command_window_is_minimized(){
  bool is_minimized = get_window_id_is_minimized((size_t)args->id);

  log_debug("Window #%d is Minimized? %s", args->id, (is_minimized == true) ? "Yes" : "No");
  exit(EXIT_SUCCESS);
}

static void _command_window_id_info(){
  struct window_info_t *w = get_window_id_info(args->id);

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
  exit((minimize_window_id(args->id) == true) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void _command_set_window_all_spaces(){
  exit(EXIT_SUCCESS);
}

static void _command_set_window_space(){
  exit(((set_window_id_to_space((size_t)(args->id), (int)(args->space_id))) == true) ? EXIT_SUCCESS : EXIT_FAILURE);
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


static void _command_focus_space(){
  log_info("Focusing space #%d", args->space_id);
  exit(EXIT_SUCCESS);
}

static void _command_focus_window(){
  focus_window_id(args->id);
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


static void _command_create_space(){

  uint64_t space_id = (uint64_t)(get_current_space_id());
  int display_space = (int)(get_current_display_id());
  log_info("creating space on %llu|%d.....", space_id, display_space);
  uint64_t add_space_fp = 0x230000;

  exit(EXIT_SUCCESS);
}
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

static void _command_list_alacritty(){
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
LIST_HANDLER(process)
LIST_HANDLER(space)
LIST_HANDLER(display)
#undef LIST_HANDLER
#undef LOCAL_DEBUG_MODE
#undef debug  
#endif
