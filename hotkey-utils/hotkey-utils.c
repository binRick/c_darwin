#pragma once
#ifndef HOTKEY_UTILS_C
#define HOTKEY_UTILS_C
////////////////////////////////////////////
#include "hotkey-utils/hotkey-utils-types.h"
#include "hotkey-utils/hotkey-utils.h"
////////////////////////////////////////////
#include "core/core.h"
#include "capture/capture.h"
////////////////////////////////////////////
#define HOTKEY_UTILS_HASH_SEED    212136436
#define RELOAD_CONFIG_MS 10000
#define HOTKEYS_CONFIG_FILE_NAME    "config.yaml"
////////////////////////////////////////////
static bool HOTKEY_UTILS_DEBUG_MODE = false, HOTKEY_UTILS_VERBOSE_DEBUG_MODE;
static char *EXECUTABLE_PATH_DIRNAME;
static libforks_ServerConn      conn;
static pid_t fork_server_pid;
///////////////////////////////////////////////////////////////////////
static struct hotkeys_config_t *hk_get_config();
static void handle_sigterm(int signum) {
  (void)signum;
  log_info("Hotkey Server SIGTERM");
  exit(EXIT_SUCCESS);
}
static CGEventMask kb_events = (
  CGEventMaskBit(kCGEventKeyDown)
  | CGEventMaskBit(kCGEventFlagsChanged)
  );
static CGEventRef event_handler(__attribute__((unused)) CGEventTapProxy proxy, __attribute__((unused)) CGEventType type, CGEventRef event, void *CALLBACK) {
  unsigned long event_flags = (int)CGEventGetFlags(event);

  if (event_flags > 0) {
    CGKeyCode keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    if (keyCode > 0) {
      char *ckc = keylogger_convertKeyboardCode(keyCode);
      if (strlen(ckc) > 0) {
        struct StringBuffer *event_flag_sb = stringbuffer_new();
        if (event_flags & kCGEventFlagMaskAlternate) {
          stringbuffer_append_string(event_flag_sb, "alt+");
        }
        if (event_flags & kCGEventFlagMaskSecondaryFn) {
          stringbuffer_append_string(event_flag_sb, "secondaryfxn+");
        }
        if (event_flags & kCGEventFlagMaskCommand) {
          stringbuffer_append_string(event_flag_sb, "cmd+");
        }
        if (event_flags & kCGEventFlagMaskControl) {
          stringbuffer_append_string(event_flag_sb, "ctrl+");
        }
        if (event_flags & kCGEventFlagMaskShift) {
          stringbuffer_append_string(event_flag_sb, "shift+");
        }
        stringbuffer_append_string(event_flag_sb, ckc);

        char *event_flag = stringbuffer_to_string(event_flag_sb);
        stringbuffer_release(event_flag_sb);

        if (stringfn_ends_with(event_flag, "+")) {
          stringfn_mut_substring(event_flag, 0, strlen(event_flag) - 1);
        }
        stringfn_mut_trim(event_flag);
        if (CALLBACK != NULL) {
          if (((int (*)(char *)) CALLBACK)(event_flag) == EXIT_SUCCESS) {
            return(NULL);
          }
        }
      }
    }
  }
  return(event);
}  
static int setup_event_tap_with_callback(void ( *cb )(char *key)){
  CFMachPortRef event_tap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0, kb_events, event_handler, (void *)cb);

  if (!event_tap) {
    log_error("ERROR: Unable to create keyboard event tap.");
    return(EXIT_FAILURE);
  }
  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, event_tap, 0);

  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
  CGEventTapEnable(event_tap, true);
  return(EXIT_SUCCESS);
}
static int tap_events(){
  CFRunLoopRun();
  return(EXIT_SUCCESS);
}

static int __hotkeys_server(void *P){

}
int hotkeys_exec_with_callback(void ( *cb )(char *)) {
  assert(is_authorized_for_accessibility() == true);
  assert(setup_event_tap_with_callback(cb) == 0);
  log_info("Keylogger Event Tap Setup");
  assert(tap_events() == 0);
  log_info("Keylogger Events Tapped");
  return(EXIT_SUCCESS);
}

int run_hotkeys_server(){
  assert(libforks_start(&conn) == libforks_OK);
  fork_server_pid = libforks_get_server_pid(conn);
  assert(fork_server_pid > 0);
  if (HOTKEY_UTILS_DEBUG_MODE) {
    log_info(AC_YELLOW "Hotkey Utils> Fork server with pid %d created"AC_RESETALL, fork_server_pid);
  }  
  signal(SIGTERM, handle_sigterm);
}
static int hotkeys_server(libforks_ServerConn conn, void *P){
  libforks_free_conn(conn);
  struct hotkeys_libforks_param_t *p = (struct hotkeys_libforks_param_t *)P;
  char                    *msg;
  asprintf(&msg, "Starting Hotkeys Server");
  log_info("%s", msg);

  return(__hotkeys_server((void *)p));
}

size_t get_config_file_hash(char *CONFIG_FILE_PATH){
  char *config_contents = fsio_read_text_file(CONFIG_FILE_PATH);
  size_t config_hash = ((size_t)murmurhash(
           (const char *)config_contents,
           (uint32_t)strlen((const char *)config_contents),
           (uint32_t)HOTKEY_UTILS_HASH_SEED
           )
         );
  if(config_contents)
    free(config_contents);
  return(config_hash);
}

char *get_hotkey_type_action_name(enum action_type_t action){
for(size_t i=0;i<(sizeof(action_type_strings)/sizeof(action_type_strings[0]));i++){
  if(action==action_type_strings[i].val)
    return(action_type_strings[i].str);
}
return NULL;
}

char *get_homedir_yaml_config_file_path(){
  char *path;
  char *home = getenv("HOME");
  if (!home) {
    log_error("HOME Environment variable not set!");
    exit(EXIT_FAILURE);
  }
  asprintf(&path, "%s/.config/dls/dls.hotkeys.yaml", home);
  if (fsio_file_exists(path) == false) {
    fsio_mkdirs_parent(path, 0700);
    log_error("Config File %s is Missing. Create it.", path);
    exit(EXIT_FAILURE);
  }
  return(path);
}

int execute_hotkey_config_key(struct key_t *key){
  return(handle_action(key->action_type, key->action));
}

struct hk_layout_t *hk_get_layout(struct hotkeys_config_t *cfg, char *name){
  for (size_t i = 0; i < cfg->layouts_count; i++) {
    if (strcmp(cfg->layouts[i].name, name) == 0) {
      return(&(cfg->layouts[i]));
    }
  }
  return(NULL);
}

struct key_t *get_hotkey_config_key(struct hotkeys_config_t *cfg, char *key){
  for (size_t i = 0; i < cfg->keys_count; i++) {
    if (strcmp(cfg->keys[i].key, key) == 0) {
      return(&(cfg->keys[i]));
    }
  }
  return(NULL);
}

#define DEBUG_WINDOW_RESIZE(RESIZE_MODE) { do { \
    int cur_display_width = get_display_width(), cur_display_height = get_display_height();\
    log_debug("%s focused app width %f%% & height %f%%|pid:%d|name:%s|windowid:%lu|\n"\
           "                                 |cur size:%dx%d|cur pos:%dx%d|\n"\
           "                                 |new size:%dx%d,new pos:%dx%d|\n"\
           "                                 |cur dis size:%dx%d|\n"\
           "%s",\
           RESIZE_MODE,\
           width_factor,height_factor,\
           focused_window_info->pid,\
           focused_window_info->name,\
           focused_window_info->window_id,\
           (int)focused_window_info->rect.size.width,(int)focused_window_info->rect.size.height,\
           (int)focused_window_info->rect.origin.x,(int)focused_window_info->rect.origin.y,\
           (int)new_rect.size.width,(int)new_rect.size.height,\
           (int)new_rect.origin.x,(int)new_rect.origin.y,\
           cur_display_width,cur_display_height,\
           ""\
           );\
} while(0); }

#define WINDOW_RESIZE()\
  if((int)new_rect.origin.x != (int)focused_window_info->rect.origin.x || (int)new_rect.origin.y != (int)focused_window_info->rect.origin.y){\
    if(HOTKEY_UTILS_VERBOSE_DEBUG_MODE)\
      log_debug("Moving window prior to resize!");\
    move_window_info(focused_window_info, (int)new_rect.origin.x, (int)new_rect.origin.y);\
  }\
  bool ok = resize_window_info(focused_window_info, (int)new_rect.size.width,(int)new_rect.size.height);\
  return((ok==true) ? EXIT_SUCCESS : EXIT_FAILURE);

int decrease_focused_application_height_five_percent(){
  float width_factor = 1.00, height_factor = 0.95;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Decreasing Height")
  WINDOW_RESIZE()
}
int increase_focused_application_height_five_percent(){
  float width_factor = 1.00, height_factor = 1.05;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Increasing Height")
  WINDOW_RESIZE()
}

int increase_focused_application_width_five_percent(){
  float width_factor = 1.05, height_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Increasing Width")
  WINDOW_RESIZE()
}

int decrease_focused_application_width_five_percent(){
  float width_factor = 0.95, height_factor = 1.00;
  struct window_info_t *focused_window_info;

  focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Decreasing Width")
  WINDOW_RESIZE()
}


int decrease_focused_application_height_ten_percent(){
  float width_factor = 1.00, height_factor = 0.90;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Decreasing Height")
  WINDOW_RESIZE()
}
int increase_focused_application_height_ten_percent(){
  float width_factor = 1.00, height_factor = 1.10;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Increasing Height")
  WINDOW_RESIZE()
}

int increase_focused_application_width_ten_percent(){
  float width_factor = 1.10, height_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);

  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Increasing Width")
  WINDOW_RESIZE()
}

int decrease_focused_application_width_ten_percent(){
  float width_factor = 0.90, height_factor = 1.00;
  struct window_info_t *focused_window_info;

  focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Decreasing Width")
  WINDOW_RESIZE()
}

int left_percent_focused_application(float left_factor){
  float width_factor = left_factor, height_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor_left_side(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Left Side 50%")
  WINDOW_RESIZE()
  return(EXIT_SUCCESS);
}

#define GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(DIRECTION, FACTOR_TEXT, FACTOR_FLOAT)\
  int DIRECTION##_##FACTOR_TEXT##_percent_focused_application(){\
    return(DIRECTION##_percent_focused_application(FACTOR_FLOAT));\
  }
///////////////////////////////////////////////////////////////////
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, twenty_five, 0.25);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, twenty_five, 0.25);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, fourty, 0.40);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, fourty, 0.40);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, fifty, 0.50);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, fifty, 0.50);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, sixty, 0.60);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, sixty, 0.60);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, seventy, 0.70);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, seventy, 0.70);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(right, seventy_five, 0.75);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(left, seventy_five, 0.75);
///////////////////////////////////////////////////////////////////
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(top, twenty_five, 0.25);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(bottom, twenty_five, 0.25);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(top, fourty, 0.40);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(bottom, fourty, 0.40);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(top, fifty, 0.50);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(bottom, fifty, 0.50);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(top, sixty, 0.60);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(bottom, sixty, 0.60);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(top, seventy_five, 0.75);
GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION(bottom, seventy_five, 0.75);
///////////////////////////////////////////////////////////////////
#undef GENERATE_PERCENT_FOCUSED_APPLICATION_FUNCTION

int bottom_percent_focused_application(float bottom_factor){
  float height_factor = bottom_factor, width_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor_bottom_side(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Bottom Side 50%")
  WINDOW_RESIZE()
  return(EXIT_SUCCESS);
}

int top_percent_focused_application(float top_factor){
  float height_factor = top_factor, width_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor_top_side(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Top Side 50%")
  WINDOW_RESIZE()
  return(EXIT_SUCCESS);
}

int right_percent_focused_application(float right_factor){
  float width_factor = right_factor, height_factor = 1.00;
  struct window_info_t *focused_window_info = get_focused_window_info();
  CGRect new_rect = get_resized_window_info_rect_by_factor_right_side(focused_window_info, width_factor, height_factor);
  if(HOTKEY_UTILS_DEBUG_MODE)
    DEBUG_WINDOW_RESIZE("Right Side 50%")
  WINDOW_RESIZE()
  return(EXIT_SUCCESS);
}


int minimize_application(void *APPLICATION_NAME){
  log_info("Minimize app %s", (char *)APPLICATION_NAME);
  size_t window_id = get_first_window_id_by_name((char *)APPLICATION_NAME);
  set_window_id_flags(window_id, WINDOW_FLAG_MINIMIZE);
  return(EXIT_SUCCESS);
}

int fullscreen_application(void *APPLICATION_NAME){
  log_info("Fullscreen app %s", (char *)APPLICATION_NAME);
  return(EXIT_SUCCESS);
}

int hk_get_layout_name_index(char *name){
  struct hotkeys_config_t *cfg         = hk_get_config();
  int r=-1;
  for(size_t i = 0; r == -1 && i < cfg->layouts_count;i++)
    if(strcmp(cfg->layouts[i].name,name)==0)
      r=i;
  return(r);
}

struct layout_result_t *hk_render_layout_name(char *name){
  struct hotkeys_config_t *cfg         = hk_get_config();
  int I = hk_get_layout_name_index(name);
  if(I<0){
    log_error("Layout not found");
    return(0);
  }
  struct layout_request_t *req; struct layout_result_t *res;
  req = layout_init_request();
  req->debug = HOTKEY_UTILS_DEBUG_MODE;
  req->mode = LAYOUT_MODE_HORIZONTAL;
  req->max_width = get_display_width();
  req->max_height = get_display_height();
  req->master_width = (int)((float)(req->max_width) * (float)(cfg->layouts[I].width)/100);
  req->master_height = req->max_height;
  req->qty = cfg->layouts[I].apps_count;
  res = layout_request(req);
  free(req);
  return(res);
}

struct hk_dur_t {
  unsigned long dur, started;
};
enum hk_format_t {
  HK_FORMAT_QOI,
  HK_FORMAT_JPEG,
  HK_FORMATS_QTY,
};
enum image_type_id_t hk_formats[] = {
  [HK_FORMAT_JPEG] = IMAGE_TYPE_JPEG,
  [HK_FORMAT_QOI] = IMAGE_TYPE_QOI,
};
enum hk_vip_t {
  HK_VIP_IMAGE,
  HK_VIP_RESIZED,
  HK_VIP_RED_RECT,
  HK_VIP_RESIZED_RED_RECT,
  HK_VIPS_QTY,
};
struct hk_image_t {
  CGImageRef img, resized;
  unsigned char *qoi, *resized_qoi;
  size_t len, qoi_len,resized_qoi_len;
  uint32_t width,height;
  uint32_t qoi_width,qoi_height;
  uint32_t resized_width,resized_height;
  struct hk_dur_t capture, convert, resize;
  VipsImage *vips[HK_VIPS_QTY];
  unsigned char *pixels[HK_FORMATS_QTY];
  size_t sizes[HK_FORMATS_QTY];
};
struct hk_rendered_app_t {
  size_t window_id;
  pid_t pid;
  char *name;
  uint32_t x,y,width,height;
  struct hk_image_t image;
};
struct hk_rendered_area_t {
  size_t index, id;
  uint32_t width,height;
  struct hk_image_t image;
  size_t len;
};

struct hk_render_t {
  struct hk_rendered_area_t *display;
  struct hk_rendered_area_t *space;
  struct hk_rendered_app_t *master;
  struct hk_rendered_app_t **items;
};

#define ENSURE_LAYOUT_INDEX(I){ do{\
  if(I<0){\
    log_error("Layout not found");\
    return(false);\
  }\
} while(0); }

static double    red[]         = { 255, 0, 0, 255 };
static double    transparent[] = { 0, 0, 0, 0 };

//    if (vips_crop(image, &page[i], 0, page_height * i, image->Xsize, page_height, NULL)) {
    /*
  if (!(overlay[0] = vips_image_new_from_image(page[0], red, VIPS_NUMBER(red)))
      || vips_draw_rect(
        overlay[0], transparent, VIPS_NUMBER(transparent),
        10, 10, overlay[0]->Xsize - 20, overlay[0]->Ysize - 20, "fill", TRUE, NULL
        )
      ) {a
         if (vips_composite2(page[i], overlay[0], &annotated[i],
                        VIPS_BLEND_MODE_OVER, NULL)) {
      return(-1);
    }
      */

char *create_master_table_report(){
  ft_table_t *table = ft_create_table();

  ft_set_border_style(table, FT_NICE_STYLE);
  ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);

  ft_u8write_ln(table, "#", "Period Type", "Date", "Closed", "Hours", "Periods", "Avg", "Moving Avg");
  ft_add_separator(table);
  ft_u8write_ln(table, "1", "x", "y", "n-body", "1000", "1.6", "1,500,000", "✔");
  ft_u8write_ln(table, "1", "x", "y", "n-body", "1000", "1.6", "1,500,000", "✖");

  ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
  ft_set_cell_prop(table, 8, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
  ft_set_cell_prop(table, FT_ANY_ROW, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
  ft_set_cell_prop(table, FT_ANY_ROW, 4, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
  ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);

  ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
  ft_set_cell_prop(table, FT_ANY_ROW, 2, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
  ft_set_cell_prop(table, FT_ANY_ROW, 3, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
  ft_set_cell_prop(table, FT_ANY_ROW, 4, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
  ft_set_cell_prop(table, 8, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);

  ft_set_cell_prop(table, 1, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
  ft_set_cell_prop(table, 2, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
  ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);

  ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
  ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 10);

  const char *table_str = ft_to_u8string(table);

  ft_destroy_table(table);
  return(table_str);
}


bool hk_show_rendered_layout_name(char *name){
  struct hotkeys_config_t *cfg = load_yaml_config_file_path(get_homedir_yaml_config_file_path());
  struct layout_result_t *res = hk_render_layout_name(name);
  int I = hk_get_layout_name_index(name);
  ENSURE_LAYOUT_INDEX(I);
  layout_print_result(res);
  struct hk_render_t *r = calloc(1,sizeof(struct hk_render_t));
  r->master = calloc(1,sizeof(struct hk_rendered_app_t));
  r->space = calloc(1,sizeof(struct hk_rendered_area_t));
  r->display = calloc(1,sizeof(struct hk_rendered_area_t));
  r->space->index = cfg->layouts[I].display_space;
  r->display->index = cfg->layouts[I].display;
  r->display->id = get_display_index_id(r->display->index);
  if(r->display->id<1){
    log_error("Failed to find display id for index %d",cfg->layouts[I].display);
  }

  r->display->width = get_display_id_width(r->display->id);
  r->display->height = get_display_id_height(r->display->id);

  r->display->image.capture.started = timestamp();
  r->display->image.img= capture_type_capture(CAPTURE_TYPE_DISPLAY, r->display->id);
  r->display->image.width = CGImageGetWidth(r->display->image.img);
  r->display->image.height = CGImageGetHeight(r->display->image.img);
  r->display->image.capture.dur = timestamp() - r->display->image.capture.started;

  r->display->image.convert.started = timestamp();
  r->display->image.pixels[HK_FORMAT_QOI] = save_cgref_to_qoi_memory(r->display->image.img,&(r->display->image.pixels[HK_FORMAT_QOI]));
  r->display->image.pixels[HK_FORMAT_JPEG] = save_cgref_to_jpeg_memory(r->display->image.img,&(r->display->image.pixels[HK_FORMAT_JPEG]));
  r->display->image.convert.dur = timestamp() - r->display->image.convert.started;

//  r->display->image.vips[HK_VIP_IMAGE] = vips_image_new_from_image(page[0], red, VIPS_NUMBER(red));
  //r->display->vips[HK_VIP_RED_RECT] = vips_image_new_from_image(page[0], red, VIPS_NUMBER(red));

  r->master->name = cfg->layouts[I].app;
  r->master->window_id = get_first_window_id_by_name(r->master->name);
  r->master->image.len = r->master->image.qoi_len;
  if(r->master->window_id<1){
    log_error("Failed to find window id for %s",cfg->layouts[I].app);
  }

  r->master->width = res->master->width;
  r->master->height = res->master->height;
  r->master->x = res->master->x;
  r->master->y = res->master->y;

  r->master->image.capture.started = timestamp();
  r->master->image.img= capture_type_capture(CAPTURE_TYPE_WINDOW, r->master->window_id);
  r->master->image.width = CGImageGetWidth(r->master->image.img);
  r->master->image.height = CGImageGetHeight(r->master->image.img);
  r->master->image.capture.dur = timestamp() - r->master->image.capture.started;

  r->master->image.resize.started = timestamp();
  r->master->image.resized= resize_cgimage(r->master->image.img, r->master->width,r->master->height);
  r->master->image.resized_width = CGImageGetWidth(r->master->image.resized);
  r->master->image.resized_height = CGImageGetHeight(r->master->image.resized);
  r->master->image.resize.dur = timestamp() - r->master->image.resize.started;

  log_debug("..");
  r->master->image.convert.started = timestamp();
  r->master->image.qoi = save_cgref_to_qoi_memory(r->master->image.img,&(r->master->image.qoi_len));
  r->master->image.resized_qoi = save_cgref_to_qoi_memory(r->master->image.resized,&(r->master->image.resized_qoi_len));
  r->master->image.convert.dur = timestamp() - r->master->image.convert.started;
  r->master->image.len = r->master->image.qoi_len;
  log_debug("..");

  size_t len=r->display->image.qoi_len+r->master->image.qoi_len;
  r->items = calloc(1,sizeof(struct hk_rendered_app_t));
  for(size_t i=0;i<res->qty;i++){
    r->items[i] = calloc(1,sizeof(struct hk_rendered_app_t));
    r->items[i]->window_id = get_first_window_id_by_name(cfg->layouts[I].apps[i].name);
    r->items[i]->name = cfg->layouts[I].apps[i].name;
    if(r->items[i]->window_id<1){
      log_error("Failed to find window id for %s",r->items[i]->name);
      exit(EXIT_FAILURE);
    }
    r->items[i]->width = res->items[i]->width;
    r->items[i]->height = res->items[i]->height;
    r->items[i]->x = res->items[i]->x;
    r->items[i]->y = res->items[i]->y;

    r->items[i]->image.capture.started = timestamp();
    r->items[i]->image.img= capture_type_capture(CAPTURE_TYPE_WINDOW, r->items[i]->window_id);
    r->items[i]->image.width = CGImageGetWidth(r->items[i]->image.img);
    r->items[i]->image.height = CGImageGetHeight(r->items[i]->image.img);
    r->items[i]->image.capture.dur = timestamp() - r->items[i]->image.capture.started;

    /*
    r->items[i]->image.resize.started = timestamp();
    r->items[i]->image.resized= resize_cgimage(r->items[i]->image.img, r->items[i]->width,r->items[i]->height);
    r->items[i]->image.resized_width = CGImageGetWidth(r->items[i]->image.resized);
    r->items[i]->image.resized_height = CGImageGetHeight(r->items[i]->image.resized);
    r->items[i]->image.resize.dur = timestamp() - r->items[i]->image.resize.started;
    */

    r->items[i]->image.convert.started = timestamp();
    r->items[i]->image.qoi = save_cgref_to_qoi_memory(r->items[i]->image.img,&(r->items[i]->image.qoi_len));
    //r->items[i]->image.resized_qoi = save_cgref_to_qoi_memory(r->items[i]->image.resized,&(r->items[i]->image.resized_qoi_len));
    r->items[i]->image.convert.dur = timestamp() - r->items[i]->image.convert.started;

    len += r->items[i]->image.qoi_len;
  }
  log_debug("1");
  log_info(
      "Total Size: %s"
      "Master:\n%s"
      "\nRendering Master %s Window ID %lu to %dx%d @ %dx%d"
      "\nResized Master to %dx%d in %s"
      "\n\tMaster Image: %dx%d in %s"
      "\n\tMaster QOI: %s (%s resized) in %s"
      "\nDisplay Image: %dx%d in %s"
      "\n\tDisplay QOI: %s in %s"
      "\nDisplay: Index:%lu|ID:%lu|%dx%d|Space: Index:%lu|ID:%lu|"
      "",
      bytes_to_string(len),
      create_master_table_report(),
      r->master->name,r->master->window_id,r->master->width,r->master->height,r->master->x,r->master->y,
      r->master->image.resized_width,r->master->image.resized_height,milliseconds_to_string(r->master->image.resize.dur),
      r->master->image.width,r->master->image.height,milliseconds_to_string(r->master->image.capture.dur),
      bytes_to_string(r->master->image.qoi_len),bytes_to_string(r->master->image.resized_qoi_len),milliseconds_to_string(r->display->image.convert.dur),
      r->display->image.width,r->display->image.height,milliseconds_to_string(r->display->image.capture.dur),
      bytes_to_string(r->display->image.qoi_len),milliseconds_to_string(r->display->image.convert.dur),
      r->display->index,
      r->display->id,
      r->display->width,
      r->display->height,
      r->space->index,
      r->space->id
        );
  for(size_t i=0;i<res->qty;i++){
    log_info(
      "\tRendering App %s Window ID %lu to %dx%d @ %dx%d"
      "\n\tQOI         : %dx%d %s"
      "\n\tResized QOI : %s"
      "\n\tCapture     : %s"
      "\n\tResize      : %s"
      "\n\tConvert     : %s"
      "",
      r->items[i]->name,r->items[i]->window_id,r->items[i]->width,r->items[i]->height,r->items[i]->x,r->items[i]->y,
      r->items[i]->image.width,r->items[i]->image.height,bytes_to_string(r->items[i]->image.qoi_len),
      bytes_to_string(r->items[i]->image.resized_qoi_len),
      milliseconds_to_string(r->items[i]->image.capture.dur),
      milliseconds_to_string(r->items[i]->image.resize.dur),
      milliseconds_to_string(r->items[i]->image.convert.dur)

      );
  }

  return(true);
}

bool hk_show_layout(char *name){
  struct hotkeys_config_t *cfg         = hk_get_config();
  int I = hk_get_layout_name_index(name);
  if(I<0){
    log_error("Layout not found");
    return(false);
  }
  printf(
       AC_YELLOW "%s\n" AC_RESETALL
       AC_GREEN " Width: %d%%\n" AC_RESETALL
       AC_CYAN " Mode: %s\n" AC_RESETALL
       AC_MAGENTA " Display: %d\n" AC_RESETALL
       AC_BLUE " %lu Apps:\n" AC_RESETALL
        "%s",
      cfg->layouts[I].name,
      (int)(cfg->layouts[I].width),
      cfg->layouts[I].mode,
      cfg->layouts[I].display,
      cfg->layouts[I].apps_count,
        "");
    for(size_t i = 0; i < cfg->layouts[I].apps_count;i++){
      printf("  - %s\n",cfg->layouts[I].apps[i].name);
    }
    struct layout_result_t *res = hk_render_layout_name(name);
    return(layout_print_result(res));
  }

bool hk_print_layout_names(){
  struct hotkeys_config_t *cfg         = hk_get_config();
  for(size_t i = 0; i < cfg->layouts_count;i++){
    printf("%s\n",cfg->layouts[i].name);
  }
  free(cfg);
  return(true);
}

bool hk_list_layouts(){
  struct hotkeys_config_t *cfg         = hk_get_config();
  for(size_t i = 0; i < cfg->layouts_count;i++){
    printf("%s\n",cfg->layouts[i].name);
  }
  free(cfg);
  return(true);
}

static struct hotkeys_config_t *hk_get_config(){
  return(load_yaml_config_file_path(get_homedir_yaml_config_file_path()));
}

int normalize_layout(void *LAYOUT){
  char *n = (char*)LAYOUT;
  int I = hk_get_layout_name_index(n);
  if(I<0){
    log_error("Layout not found");
    return(false);
  }
  struct hotkeys_config_t *cfg         = hk_get_config();
  struct hk_layout_t *l = hk_get_layout(cfg, n);
  log_info("normalizing layout \"%s\" using %s and app %s to width %f",n,l->name,l->app,l->width);
  struct layout_request_t *req; struct layout_result_t *res;
  req = layout_init_request();
  req->debug = true;
  req->mode = LAYOUT_MODE_HORIZONTAL;
  log_debug("%lu",cfg->layouts[I].apps_count);
  for(size_t i=0; i < cfg->layouts[I].apps_count;i++){
    req->qty++;
  }
  req->max_width = get_display_width()/100;
  req->max_height = get_display_height()/1;
  req->master_width = (int)((float)(req->max_width) * (float)l->width);
  res = layout_request(req);
  /*
  for(size_t i=0; i < cfg->layouts[I].apps_count;i++){
    struct layout_content_result_t *c = (struct hk_layout_content_result_t*)        vector_get(res->contents_v,i);
    Dbg(c->x,%lu);
    Dbg(c->y,%lu);
    Dbg(c->width,%lu);
    Dbg(c->height,%lu);
  }
  */
  free(res);
  free(cfg);
  return(EXIT_SUCCESS);
}

int activate_application(void *APPLICATION_NAME){
  if (HOTKEY_UTILS_DEBUG_MODE == true) {
    log_info("Activating Application %s", (char *)APPLICATION_NAME);
  }
  size_t window_id = get_first_window_id_by_name((char *)APPLICATION_NAME);
  if (window_id == 0) {
    log_error("Failed to find window named '%s'", (char *)APPLICATION_NAME);
  }else{
    if (HOTKEY_UTILS_VERBOSE_DEBUG_MODE == true) {
      log_info("Got Window ID %lu", window_id);
    }
    focus_window_id(window_id);
  }
  return(EXIT_SUCCESS);
}

int deactivate_application(void *APPLICATION_NAME){
  log_info("Deactivating Application %s", (char *)APPLICATION_NAME);
  return(EXIT_SUCCESS);
}

int handle_action(enum action_type_t action_type, void *action){
  return(action_type_handlers[action_type].fxn(action));
}

struct Vector *get_config_keys_v(){
  struct Vector *v=vector_new();
  struct hotkeys_config_t *cfg = load_yaml_config_file_path(get_homedir_yaml_config_file_path());
  for(size_t i = 0; i < cfg->keys_count;i++){
    vector_push(v,(void*)(struct key_t*)&(cfg->keys[i]));
  }
  return(v);
}
struct hotkeys_config_t *load_yaml_config_file_path(char *config_file_path){
  static struct hotkeys_config_t *hotkeys_config = NULL;
  char *config_contents = fsio_read_text_file(config_file_path);
  size_t config_hash = get_config_file_hash(config_file_path);
  static size_t loaded_config_hash = 0;
  static unsigned long loaded_config_ts = 0;
  if((hotkeys_config == NULL) || (loaded_config_hash != config_hash) || ((timestamp()-loaded_config_ts) > RELOAD_CONFIG_MS)){
    if(HOTKEY_UTILS_DEBUG_MODE)
      log_info("Loading %s Config with hash %lu", bytes_to_string(strlen(config_contents)), config_hash);
    cyaml_err_t             err = cyaml_load_file(config_file_path, &config, &top_schema, (cyaml_data_t **)&hotkeys_config, NULL);
    if (err != CYAML_OK) {
      log_error("%s", cyaml_strerror(err));
      exit(EXIT_FAILURE);
    }
    loaded_config_hash = config_hash;
    loaded_config_ts = timestamp();
    if(HOTKEY_UTILS_DEBUG_MODE)
      log_info("todo app: %s | width: %d",hotkeys_config->todo_app, hotkeys_config->todo_width);
  }else{
    if(HOTKEY_UTILS_VERBOSE_DEBUG_MODE)
      log_info("Config loaded %s ago", milliseconds_to_string(timestamp()-loaded_config_ts));
  }
  return(hotkeys_config);
}

char *get_yaml_config_file_path(char **argv){
  char EXECUTABLE_PATH[PATH_MAX + 1] = { 0 };

  realpath(argv[0], EXECUTABLE_PATH);
  EXECUTABLE_PATH_DIRNAME = dirname(EXECUTABLE_PATH);
  char *path;

  asprintf(&path, "%s/../../hotkey-utils/%s", EXECUTABLE_PATH_DIRNAME, HOTKEYS_CONFIG_FILE_NAME);
  return(path);
}

bool disable_hotkey_config_key(struct key_t *key){
  log_info("disabling key %s:%s=>%s", key->name,key->key,key->action);
  return(true);
}
bool enable_hotkey_config_key(struct key_t *key){
  log_info("enabling key %s:%s=>%s", key->name,key->key,key->action);
  return(true);
}
////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__hotkey_utils(void){
  if (getenv("DEBUG") != NULL || getenv("VERBOSE_DEBUG_HOTKEY_UTILS") != NULL) {
    log_debug("Enabling hotkey-utils Verbose Debug Mode");
    HOTKEY_UTILS_DEBUG_MODE = true;
    HOTKEY_UTILS_VERBOSE_DEBUG_MODE = true;
  }else if (getenv("DEBUG") != NULL || getenv("DEBUG_HOTKEY_UTILS") != NULL) {
    log_debug("Enabling hotkey-utils Debug Mode");
    HOTKEY_UTILS_DEBUG_MODE = true;
  }
}
#endif
