#include "ls-win/ls-win.h"
#include <pthread.h>
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "core-utils/core-utils.h"
#include "display-utils/display-utils.h"
#include "dock-utils/dock-utils.h"
#include "focused/focused.h"
#include "log.h/log.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "ms/ms.h"
#include "optparse99/optparse99.h"
#include "space-utils/space-utils.h"
#include "timestamp/timestamp.h"
////////////////////////////////////////////
static void _command_kb_events();
static void _command_move_window();
static void _command_resize_window();
static void _command_focus_window();
static void _command_set_window_space();
static void _command_set_space();
static void _command_set_window_all_spaces();
static void _command_windows();
static void _command_displays();
static void _command_spaces();
static void _command_debug_args();
static void _command_focused_start();
static void _command_sticky_window();
static void _command_menu_bar();
static void _command_dock();
////////////////////////////////////////////
enum command_type_t {
  COMMAND_KB_EVENTS,
  COMMAND_MOVE_WINDOW,
  COMMAND_RESIZE_WINDOW,
  COMMAND_FOCUS_WINDOW,
  COMMAND_SET_WINDOW_SPACE,
  COMMAND_SET_WINDOW_ALL_SPACES,
  COMMAND_SET_SPACE,
  COMMAND_WINDOWS,
  COMMAND_SPACES,
  COMMAND_DISPLAYS,
  COMMAND_DEBUG_ARGS,
  COMMAND_FOCUSED_START,
  COMMAND_STICKY_WINDOW,
  COMMAND_MENU_BAR,
  COMMAND_DOCK,
  COMMAND_TYPES_QTY,
};
struct cmd_t {
  void (*fxn)(void);
};

struct cmd_t cmds[COMMAND_TYPES_QTY + 1] = {
  [COMMAND_KB_EVENTS]             = { _command_kb_events             },
  [COMMAND_MOVE_WINDOW]           = { _command_move_window           },
  [COMMAND_RESIZE_WINDOW]         = { _command_resize_window         },
  [COMMAND_FOCUS_WINDOW]          = { _command_focus_window          },
  [COMMAND_SET_WINDOW_SPACE]      = { _command_set_window_space      },
  [COMMAND_SET_WINDOW_ALL_SPACES] = { _command_set_window_all_spaces },
  [COMMAND_SET_SPACE]             = { _command_set_space             },
  [COMMAND_WINDOWS]               = { _command_windows               },
  [COMMAND_SPACES]                = { _command_spaces                },
  [COMMAND_DISPLAYS]              = { _command_displays              },
  [COMMAND_DEBUG_ARGS]            = { _command_debug_args            },
  [COMMAND_FOCUSED_START]         = { _command_focused_start         },
  [COMMAND_STICKY_WINDOW]         = { _command_sticky_window         },
  [COMMAND_MENU_BAR]              = { _command_menu_bar              },
  [COMMAND_DOCK]                  = { _command_dock                  },
  [COMMAND_TYPES_QTY]             = { 0                              },
};

static void _command_focused_start(){
  struct focused_config_t *cfg = init_focused_config();
  size_t                  wid  = 129;

  add_focused_window_id(cfg, wid);

  start_focused(cfg);
  int i = 1000, on = 0;

  while (on < i) {
    sleep(1);
    on++;
  }
  log_info("done");
  stop_focused(cfg);
  exit(0);
}

static void _command_kb_events(){
  bool res = init_kb_events();

  log_debug("waiting for events: %d", res);

  CFRunLoopRun();
  exit(0);
}

static void _command_move_window(){
  struct window_t *w = get_window_id(args->window_id);

  log_debug("moving window %lu to %dx%d", w->window_id, args->x, args->y);
  move_window(w, args->x, args->y);
  exit(0);
}

static void _command_resize_window(){
  struct window_t *w = get_window_id(args->window_id);

  log_debug("resizing window %lu to %dx%d", w->window_id, args->width, args->height);
  resize_window(w, args->width, args->height);
  exit(0);
}

static void _command_dock(){
  bool   is_visible        = dock_is_visible();
  CGSize offset            = dock_offset();
  CGRect rect              = dock_rect();
  int    orientation       = dock_orientation();
  char   *orientation_name = dock_orientation_name();

  log_info(
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
  bool   is_visible = menu_bar_visible();
  CGRect rect       = menu_bar_rect(display_id_for_space(get_space_id()));

  log_info(
    "\n\tMenu Bar info is visible?      %s" AC_RESETALL
    "\n\tMenu Bar Size:                 %dx%d"
    "\n\tMenu Bar Position:             %dx%d"
    "\n\tMenu Bar Height:               %dpixels"
    "\n",
    (is_visible == true) ? AC_GREEN "Yes" : AC_RED "No",
    (int)rect.size.width, (int)rect.size.height,
    (int)rect.origin.x, (int)rect.origin.y,
    get_menu_bar_height()
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
  exit(0);
}

static void _command_focus_window(){
  struct window_t *w = get_window_id(args->window_id);

  log_debug("focusing window %lu on space %d", w->window_id, w->space_id);
  focus_window(w);
  exit(0);
}

static void _command_set_window_all_spaces(){
  struct window_t *w = get_window_id(args->window_id);

  log_debug("moving window %lu from space %d to all spaces", w->window_id, w->space_id);
  set_window_active_on_all_spaces(w);
  exit(0);
}

static void _command_set_window_space(){
  struct window_t *w = get_window_id(args->window_id);

  log_debug("moving window %lu from space %d space %d", w->window_id, w->space_id, args->space_id);
  window_send_to_space(w, args->space_id);
  w = get_window_id(args->window_id);

  log_debug("window %lu is now on space %d", w->window_id, w->space_id);

  exit(0);
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

  int         display_id       = space_display_id(args->space_id);
  CFStringRef display_uuid_ref = display_uuid(display_id);

  log_info("space display id: %d | %s", display_id, cfstring_copy(display_uuid_ref));

  CFArrayRef src_spaces = SLSCopyManagedDisplaySpaces(g_connection);
  uint32_t   ds         = args->space_id;
  CFArrayRef dst_spaces = cfarray_of_cfnumbers(&(ds), sizeof(uint32_t), 1, kCFNumberSInt32Type);
  CGSHideSpaces(g_connection, src_spaces);
  CGSShowSpaces(g_connection, dst_spaces);
  uint64_t sid = args->space_id;
  CGSManagedDisplaySetCurrentSpace(g_connection, display_uuid_ref, sid);

  //set_space_by_index(args->space_id);
  //CGSSetWorkspace(_CGSDefaultConnection(),3);
  exit(0);
}

void _command_windows(){
  struct Vector *windows_v = get_windows();

  log_debug("listing %lu windows", vector_size(windows_v));
  struct list_window_table_t *list_options = &(struct list_window_table_t){
    .current_space_only = args->current_space_only,
  };

  list_windows_table((void *)list_options);
  exit(0);
}

static void _command_displays(){
  struct Vector *display_ids_v = get_display_ids_v();

  log_debug("listing %lu displays", vector_size(display_ids_v));
  exit(0);
}

static void _command_spaces(){
  struct Vector *space_ids_v = get_space_ids_v();

  log_debug("%lu spaces. current space: %d", vector_size(space_ids_v), get_space_id());
  for (size_t i = 0; i < vector_size(space_ids_v); i++) {
    size_t        space_id            = (size_t)vector_get(space_ids_v, i);
    struct Vector *space_window_ids_v = get_space_window_ids_v(space_id);
    printf("#%lu- %lu windows\n", space_id, vector_size(space_window_ids_v));
    for (size_t I = 0; I < vector_size(space_window_ids_v); I++) {
      size_t window_id = (size_t)vector_get(space_window_ids_v, I);
      printf("\t%lu\n", window_id);
    }
  }
  exit(0);
}

static void _command_debug_args(){
  log_info("Verbose:                 %s", args->verbose == true ? "Yes" : "No");
  log_info("Current Space Only:      %s", args->current_space_only == true ? "Yes" : "No");
  log_info("Space ID:                %d", args->space_id);
  log_info("Window ID:               %d", args->window_id);
  exit(0);
}
