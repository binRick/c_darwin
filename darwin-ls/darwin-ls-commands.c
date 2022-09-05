#pragma once
#ifndef LS_WIN_COMMANDS_C
#define LS_WIN_COMMANDS_C
#include "darwin-ls/darwin-ls-commands.h"
#include "darwin-ls/darwin-ls-httpserver.h"
#include "darwin-ls/darwin-ls.h"
////////////////////////////////////////////
static void _command_move_window();
static void _command_resize_window();
static void _command_focus_window();
static void _command_set_window_space();
static void _command_set_space();
static void _command_set_space_index();
static void _command_set_window_all_spaces();
static void _command_windows();
static void _command_displays();
static void _command_spaces();
static void _command_debug_args();
static void _command_focused_server();
static void _command_focused_client();
static void _command_sticky_window();
static void _command_menu_bar();
static void _command_focused_window();
static void _command_focused_pid();
static void _command_focused_space();
static void _command_usb_devices();
static void _command_httpserver();
static void _command_dock();
static void _command_apps();
static void _command_monitors();
static void _command_fonts();
////////////////////////////////////////////
static void _check_window_id(uint16_t window_id);
static void _check_width(uint16_t window_id);
static void _check_height(uint16_t window_id);
static void _check_output_mode(char *output_mode);
////////////////////////////////////////////
common_option_b    common_options_b[COMMON_OPTION_NAMES_QTY + 1] = {
  [COMMON_OPTION_CURRENT_SPACE] = ^ struct optparse_opt (struct args_t *args) {
    return((struct optparse_opt)                                              {
      .short_name = 'c',
      .long_name = "current-space",
      .description = "Windows in current space only",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->current_space_only),
    });
  },
  [COMMON_OPTION_HELP] = ^ struct optparse_opt (struct args_t *args)          {
    return((struct optparse_opt)                                              {
      .short_name = 'h',
      .long_name = "help",
      .description = "Print help information and quit",
      .function = optparse_print_help,
    });
  },
  [COMMON_OPTION_OUTPUT_MODE] = ^ struct optparse_opt (struct args_t *args)   {
    return((struct optparse_opt)                                              {
      .short_name = 'm',
      .long_name = "mode",
      .description = "Output Mode- text, json, or table",
      .arg_name = "OUTPUT-MODE",
      .arg_data_type = check_cmds[CHECK_COMMAND_OUTPUT_MODE].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_OUTPUT_MODE].fxn,
      .arg_dest = &(args->output_mode_s),
    });
  },
  [COMMON_OPTION_VERBOSE] = ^ struct optparse_opt (struct args_t *args)       {
    return((struct optparse_opt)                                              {
      .short_name = 'v',
      .long_name = "verbose",
      .description = "Increase verbosity",
      .flag_type = FLAG_TYPE_SET_TRUE,
      .flag = &(args->verbose),
    });
  },
  [COMMON_OPTION_WINDOW_ID] = ^ struct optparse_opt (struct args_t *args)     {
    return((struct optparse_opt)                                              {
      .short_name = 'w',
      .long_name = "window-id",
      .description = "window id",
      .arg_name = "WINDOW-ID",
      .arg_dest = &(args->window_id),
      .arg_data_type = check_cmds[CHECK_COMMAND_WINDOW_ID].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_WINDOW_ID].fxn,
    });
  },
  [COMMON_OPTION_WINDOW_X] = ^ struct optparse_opt (struct args_t *args)      {
    return((struct optparse_opt)                                              {
      .short_name = 'x',
      .long_name = "window-x",
      .description = "Window X",
      .arg_name = "WINDOW-X",
      .arg_data_type = DATA_TYPE_UINT16,
      .arg_dest = &(args->x),
    });
  },
  [COMMON_OPTION_WINDOW_Y] = ^ struct optparse_opt (struct args_t *args)      {
    return((struct optparse_opt)                                              {
      .short_name = 'y',
      .long_name = "window-y",
      .description = "Window y",
      .arg_name = "WINDOW-Y",
      .arg_data_type = DATA_TYPE_UINT16,
      .arg_dest = &(args->y),
    });
  },
  [COMMON_OPTION_WINDOW_WIDTH] = ^ struct optparse_opt (struct args_t *args)  {
    return((struct optparse_opt)                                              {
      .short_name = 'W',
      .long_name = "window-width",
      .description = "Window Width",
      .arg_name = "WINDOW-WIDTH",
      .arg_data_type = check_cmds[CHECK_COMMAND_WIDTH].arg_data_type,
      .function = check_cmds[CHECK_COMMAND_WIDTH].fxn,
      .arg_dest = &(args->width),
    });
  },
  [COMMON_OPTION_WINDOW_HEIGHT] = ^ struct optparse_opt (struct args_t *args) {
    return((struct optparse_opt)                                              {
      .short_name = 'H',
      .long_name = "window-height",
      .description = "Window Height",
      .arg_name = "WINDOW-HEIGHT",
      .arg_data_type = DATA_TYPE_UINT16,
      .arg_dest = &(args->height),
    });
  },
  [COMMON_OPTION_SPACE_ID] = ^ struct optparse_opt (struct args_t *args)      {
    return((struct optparse_opt)                                              {
      .short_name = 's',
      .long_name = "space-id",
      .description = "space id",
      .arg_name = "SPACE-ID",
      .arg_data_type = DATA_TYPE_UINT16,
      .arg_dest = &(args->space_id),
    });
  },
};
////////////////////////////////////////////
struct check_cmd_t check_cmds[CHECK_COMMAND_TYPES_QTY + 1] = {
  [CHECK_COMMAND_WINDOW_ID] =   {
    .fxn           = (void (*)(void))(*_check_window_id),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_WIDTH] =       {
    .fxn           = (void (*)(void))(*_check_width),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_HEIGHT] =      {
    .fxn           = (void (*)(void))(*_check_height),
    .arg_data_type = DATA_TYPE_INT,
  },
  [CHECK_COMMAND_OUTPUT_MODE] = {
    .fxn           = (void (*)(void))(*_check_output_mode),
    .arg_data_type = DATA_TYPE_STR,
  },
  [CHECK_COMMAND_TYPES_QTY] =   { 0},
};
const char         *output_modes[OUTPUT_MODES_QTY + 1] = {
  [OUTPUT_MODE_TEXT]  = "text",
  [OUTPUT_MODE_JSON]  = "json",
  [OUTPUT_MODE_TABLE] = "table",
  [OUTPUT_MODES_QTY]  = NULL,
};
struct cmd_t       cmds[COMMAND_TYPES_QTY + 1] = {
  [COMMAND_MOVE_WINDOW]           = { .fxn = (*_command_move_window)           },
  [COMMAND_RESIZE_WINDOW]         = { .fxn = (*_command_resize_window)         },
  [COMMAND_FOCUS_WINDOW]          = { .fxn = (*_command_focus_window)          },
  [COMMAND_SET_WINDOW_SPACE]      = { .fxn = (*_command_set_window_space)      },
  [COMMAND_SET_WINDOW_ALL_SPACES] = { .fxn = (*_command_set_window_all_spaces) },
  [COMMAND_SET_SPACE]             = { .fxn = (*_command_set_space)             },
  [COMMAND_SET_SPACE_INDEX]       = { .fxn = (*_command_set_space_index)       },
  [COMMAND_WINDOWS]               = { .fxn = (*_command_windows)               },
  [COMMAND_SPACES]                = { .fxn = (*_command_spaces)                },
  [COMMAND_DISPLAYS]              = { .fxn = (*_command_displays)              },
  [COMMAND_DEBUG_ARGS]            = { .fxn = (*_command_debug_args)            },
  [COMMAND_FOCUSED_SERVER]        = { .fxn = (*_command_focused_server)        },
  [COMMAND_FOCUSED_CLIENT]        = { .fxn = (*_command_focused_client)        },
  [COMMAND_HTTPSERVER]            = { .fxn = (*_command_httpserver)            },
  [COMMAND_STICKY_WINDOW]         = { .fxn = (*_command_sticky_window)         },
  [COMMAND_MENU_BAR]              = { .fxn = (*_command_menu_bar)              },
  [COMMAND_DOCK]                  = { .fxn = (*_command_dock)                  },
  [COMMAND_APPS]                  = { .fxn = (*_command_apps)                  },
  [COMMAND_FONTS]                 = { .fxn = (*_command_fonts)                 },
  [COMMAND_FOCUSED_WINDOW]        = { .fxn = (*_command_focused_window)        },
  [COMMAND_FOCUSED_PID]           = { .fxn = (*_command_focused_pid)           },
  [COMMAND_USB_DEVICES]           = { .fxn = (*_command_usb_devices)           },
  [COMMAND_MONITORS]              = { .fxn = (*_command_monitors)              },
  [COMMAND_FOCUSED_SPACE]         = { .fxn = (*_command_focused_space)         },
  [COMMAND_TYPES_QTY]             = { 0 },
};

////////////////////////////////////////////
static void _command_monitors(){
  print_monitors();
  return(EXIT_SUCCESS);
}

static void _command_usb_devices(){
  struct Vector *_usb_devices_v = get_usb_devices_v();

  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "USB Devices" AC_RESETALL
    "\n\t# USB Devices      :       %lu"
    "\n%s",
    vector_size(_usb_devices_v),
    ""
    );
  return(EXIT_SUCCESS);
}

static void _command_httpserver(){
  log_debug("Starting HTTP Server");
  return(httpserver_main());
}

static void _command_set_space_index(){
  log_debug("setting space id from %d to %d", get_space_id(), args->space_id);
  return(EXIT_SUCCESS);
}

static void _check_output_mode(char *output_mode){
  log_info("validing output mode %s", output_mode);
  for (size_t i = 1; i < OUTPUT_MODES_QTY && output_modes[i] != NULL; i++) {
    if (strcmp(output_mode, output_modes[i]) == 0) {
      args->output_mode = i;
      return(EXIT_SUCCESS);
    }
  }
  log_error("Invalid Output Mode '%s'", output_mode);
  exit(EXIT_FAILURE);
}

static void _check_height(uint16_t width){
  log_info("validing width %d", width);
}

static void _check_width(uint16_t width){
  log_info("validing width %d", width);
}

static void _check_window_id(uint16_t window_id){
  log_info("validing window id %lu", (size_t)window_id);

  if (window_id < 1) {
    log_error("Window ID too small");
    goto do_error;
  }
  struct window_t *w = get_window_id((size_t)window_id);

  if (!w) {
    log_error("Window is Null");
    goto do_error;
  }
  if ((size_t)w->window_id != (size_t)window_id) {
    log_error("Window id mismatch: %lu|%lu", (size_t)window_id, w->window_id);
    goto do_error;
  }
  if (w) {
    free(w);
  }
  return(EXIT_SUCCESS);

do_error:
  log_error("Invalid Window ID %lu", (size_t)window_id);
  exit(EXIT_FAILURE);
}

////////////////////////////////////////////
static void _command_debug_args(){
  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Debug args" AC_RESETALL
    );
  log_info("Verbose:                 %s", args->verbose == true ? "Yes" : "No");
  log_info("Current Space Only:      %s", args->current_space_only == true ? "Yes" : "No");
  log_info("Space ID:                %d", args->space_id);
  log_info("Window ID:               %d", args->window_id);
  log_info("Output Mode:             %s (%d)", args->output_mode_s, args->output_mode);
  exit(EXIT_SUCCESS);
}

static void _command_fonts(){
  struct Vector *_installed_fonts_v = get_installed_fonts_v();

  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Fonts" AC_RESETALL
    "\n\t# Installed Fonts      :       %lu"
    "\n%s",
    vector_size(_installed_fonts_v),
    ""
    );

  exit(EXIT_SUCCESS);
}

static void _command_apps(){
  struct Vector *_running_apps_v   = get_running_apps_v();
  struct Vector *_installed_apps_v = get_installed_apps_v();

  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Applications" AC_RESETALL
    "\n\t# Running Applications        :       %lu"
    "\n\t# Installed Applications      :       %lu"
    "\n%s",
    vector_size(_running_apps_v),
    vector_size(_installed_apps_v),
    ""
    );

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

static void _command_focused_space(){
  unsigned long       started                            = timestamp();
  int                 _space_id                          = get_space_id();
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

static void _command_focused_window(){
  struct window_t *w = get_focused_window();

  log_info(
    "\t" AC_YELLOW AC_UNDERLINE "Focused Window" AC_RESETALL
    "\n\tWindow ID    :       %lu"
    "\n\tPID          :       %d"
    "\n\tApplication  :       %s"
    "\n\tSpace ID     :       %d"
    "\n\tDisplay ID   :       %d"
    "\n",
    w->window_id,
    w->pid,
    w->app_name,
    w->space_id,
    w->display_id
    );

  exit(EXIT_SUCCESS);
}

static void _command_focused_client(){
  struct focused_config_t *cfg = init_focused_config();

  run_client(cfg);
  exit(EXIT_SUCCESS);
}

static void _command_focused_server(){
  struct focused_config_t *cfg = init_focused_config();
  size_t                  wid  = 129;

  add_focused_window_id(cfg, wid);

  start_server(cfg);
  int i = 1000, on = 0;

  while (on < i) {
    sleep(1);
    on++;
  }
  log_info("done");
  stop_server(cfg);
  exit(EXIT_SUCCESS);
}

static void _command_move_window(){
  struct window_t *w = get_window_id(args->window_id);

  log_debug("moving window %lu to %dx%d", w->window_id, args->x, args->y);
  move_window(w, args->x, args->y);
  exit(EXIT_SUCCESS);
}

static void _command_resize_window(){
  struct window_t *w = get_window_id(args->window_id);

  log_debug("resizing window %lu to %dx%d", w->window_id, args->width, args->height);
  resize_window(w, args->width, args->height);
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
  int    _space_id   = get_space_id();
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
  struct window_t *w = get_window_id(args->window_id);

  log_debug("setting sticky on window %lu on space %d", w->window_id, w->space_id);
  get_window_tags(w);
  set_window_tags(w);
  get_window_tags(w);
  fade_window(w);
  exit(EXIT_SUCCESS);
}

static void _command_focus_window(){
  struct window_t *w = get_window_id(args->window_id);

  log_debug("focusing window %lu on space %d", w->window_id, w->space_id);
  focus_window(w);
  exit(EXIT_SUCCESS);
}

static void _command_set_window_all_spaces(){
  struct window_t *w = get_window_id(args->window_id);

  log_debug("moving window %lu from space %d to all spaces", w->window_id, w->space_id);
  set_window_active_on_all_spaces(w);
  exit(EXIT_SUCCESS);
}

static void _command_set_window_space(){
  struct window_t *w = get_window_id(args->window_id);

  log_debug("moving window %lu from space %d space %d", w->window_id, w->space_id, args->space_id);
  window_send_to_space(w, args->space_id);
  w = get_window_id(args->window_id);

  log_debug("window %lu is now on space %d", w->window_id, w->space_id);

  exit(EXIT_SUCCESS);
}

static void _command_set_space(){
  log_debug("setting space id from %d to %d", get_space_id(), args->space_id);

  int g_connection1 = SLSMainConnectionID();
  printf("SLSMainConnectionID: %d\n", g_connection1);

  int status = 0;
  SLSGetMenuBarAutohideEnabled(g_connection1, &status);
  printf("SLSGetMenuBarAutohideEnabled: %d\n", status);

  int activeSpace = SLSGetActiveSpace(g_connection1);
  printf("SLSGetActiveSpace: %d\n", activeSpace);

  activeSpace = SLSManagedDisplayGetCurrentSpace(g_connection1, CFUUIDCreateString(NULL, CGDisplayCreateUUIDFromDisplayID(CGMainDisplayID())));
  printf("SLSManagedDisplayGetCurrentSpace: %d\n", activeSpace);

  CFArrayRef display_spaces_ref   = SLSCopyManagedDisplaySpaces(g_connection);
  int        display_spaces_count = CFArrayGetCount(display_spaces_ref);
  log_info("spaces qty:%d", display_spaces_count);

  int         display_id       = get_space_display_id(args->space_id);
  CFStringRef display_uuid_ref = get_display_uuid_ref(display_id);

  log_info("space display id: %d | %s", display_id, cfstring_copy(display_uuid_ref));

  CFArrayRef src_spaces = SLSCopyManagedDisplaySpaces(g_connection);
  uint32_t   ds         = args->space_id;
  CFArrayRef dst_spaces = cfarray_of_cfnumbers(&(ds), sizeof(uint32_t), 1, kCFNumberSInt32Type);
  CGSHideSpaces(g_connection, src_spaces);
  CGSShowSpaces(g_connection, dst_spaces);
  uint64_t sid = args->space_id;
  CGSManagedDisplaySetCurrentSpace(g_connection, display_uuid_ref, sid);

  exit(EXIT_SUCCESS);
}

void _command_windows(){
  unsigned long started    = timestamp();
  struct Vector *windows_v = get_windows();

  log_debug(
    "\t" AC_YELLOW AC_UNDERLINE "Windows" AC_RESETALL
    "\n\t# Windows       :          %lu"
    "\n%s",
    vector_size(windows_v),
    ""
    );
  struct list_window_table_t *list_options = &(struct list_window_table_t){
    .current_space_only = args->current_space_only,
    .space_id           = args->space_id,
  };

  list_windows_table((void *)list_options);
  unsigned long dur = timestamp() - started;

  log_info(
    "\n\tDuration                    :       %s" AC_RESETALL
    "%s",
    milliseconds_to_string(dur),
    ""
    );

  exit(EXIT_SUCCESS);
}

static void _command_displays(){
  unsigned long started         = timestamp();
  struct Vector *_display_ids_v = get_display_ids_v();
  int           _id             = get_main_display_id();
  char          *_uuid          = get_display_uuid(_id);
  CGPoint       _center         = get_display_center(_id);
  CGRect        _rect           = get_display_rect(_id);
  int           _width          = get_display_width();
  struct Vector *_space_ids     = get_display_id_space_ids_v(_id);
  int           _height         = get_display_height();

  unsigned long dur = timestamp() - started;

  switch (args->output_mode) {
  case OUTPUT_MODE_TABLE:
    log_info("display table");
    break;
  case OUTPUT_MODE_JSON:
    log_info("display json");
    break;
  case OUTPUT_MODE_TEXT:
    log_info(
      "\t" AC_YELLOW AC_UNDERLINE "Displays" AC_RESETALL
      "\n\t# Displays                  :       %lu" AC_RESETALL
      "\n\tMain ID                     :       %d" AC_RESETALL
      "\n\tUUID                        :       %s" AC_RESETALL
      "\n\tCenter                      :       %dx%d"
      "\n\tSize                        :       %dx%d"
      "\n\tPosition                    :       %dx%d"
      "\n\tHeight                      :       %dpixels"
      "\n\tWidth                       :       %dpixels"
      "\n\tSpace IDs                   :       %lu"
      "\n\tDuration                    :       %s" AC_RESETALL
      "%s",
      vector_size(_display_ids_v),
      _id, _uuid,
      (int)_center.x, (int)_center.y,
      (int)_rect.size.width, (int)_rect.size.height,
      (int)_rect.origin.x, (int)_rect.origin.y,
      _height, _width,
      vector_size(_space_ids),
      milliseconds_to_string(dur),
      ""
      );
    break;
  }

  exit(EXIT_SUCCESS);
} /* _command_displays */

static void _command_spaces(){
  struct Vector *space_ids_v = get_space_ids_v();

  log_debug(
    "\t" AC_YELLOW AC_UNDERLINE "Spaces" AC_RESETALL
    "\n\t# Spaces           :     %lu"
    "\n\tCurrent            :     %d",
    vector_size(space_ids_v), get_space_id()
    );
  for (size_t i = 0; i < vector_size(space_ids_v); i++) {
    size_t        space_id            = (size_t)vector_get(space_ids_v, i);
    struct Vector *space_window_ids_v = get_space_window_ids_v(space_id);
    printf("#%lu- %lu windows\n", space_id, vector_size(space_window_ids_v));
    for (size_t I = 0; I < vector_size(space_window_ids_v); I++) {
      size_t window_id = (size_t)vector_get(space_window_ids_v, I);
      printf("\t%lu\n", window_id);
    }
  }
  exit(EXIT_SUCCESS);
}

#endif
