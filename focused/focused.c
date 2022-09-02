////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "display-utils/display-utils.h"
#include "dock-utils/dock-utils.h"
#include "log.h/log.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "ms/ms.h"
#include "space-utils/space-utils.h"
#include "timestamp/timestamp.h"
#include <pthread.h>
////////////////////////////////////////////
#include "focused/focused.h"

static void on_space_id_changed(void *event_data, void *VOID){
  struct focused_config_t *c       = (struct focused_config_t *)VOID;
  size_t                  space_id = (size_t)event_data;

  log_info("on_space_id_changed, spaceid: %lu|focused window ids qty:%lu", space_id, vector_size(c->focused_window_ids));
  for (size_t i = 0; vector_size(c->focused_window_ids); i++) {
    size_t window_id = (size_t)vector_get(c->focused_window_ids, i);
    if (window_id == 0) {
      break;
    }
    log_debug("moving window %lu space %lu", window_id, space_id);
    window_id_send_to_space(window_id, (int)space_id);
    struct window_t *w = get_window_id(window_id);
    focus_window(w);
  }
  log_info("on_space_id_changed end");
}

static volatile CGEventMask focused_events = (
  CGEventMaskBit(kCGEventKeyUp)
  |
  CGEventMaskBit(kCGEventKeyDown)
  |
  CGEventMaskBit(kCGEventFlagsChanged)
  );

static CGEventRef focused_event_handler(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *VOID) {
  struct focused_config_t *c          = (struct focused_config_t *)VOID;
  CGKeyCode               keyCode     = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
  char                    *kc         = convertKeyboardCode(keyCode);
  unsigned long           event_flags = (int)CGEventGetFlags(event);
  struct StringBuffer     *sb         = stringbuffer_new();

  if (event_flags & kCGEventFlagMaskCommand) {
    stringbuffer_append_string(sb, "command+");
  }
  if (event_flags & kCGEventFlagMaskControl) {
    stringbuffer_append_string(sb, "control+");
  }
  stringbuffer_append_string(sb, kc);
  char *keys = stringbuffer_to_string(sb);

  stringbuffer_release(sb);
  size_t space_id = (size_t)get_space_id();

  log_info("key: %s|%ld|%lu:%lu", keys, c->started, space_id, c->cur_space_id);
  unsigned long started = timestamp();

  if (false) {
    if (c->cur_space_id != space_id) {
      eventemitter_emit(c->ee, EVENT_SPACE_ID_CHANGED, (void *)space_id);
      c->cur_space_id = (size_t)space_id;
    }
    unsigned long dur = timestamp() - started;

    log_info("emitted in %s", milliseconds_to_string(dur));
  }
  return(event);
}

bool add_focused_window_id(struct focused_config_t *cfg, size_t WINDOW_ID){
  vector_push(cfg->focused_window_ids, (void *)WINDOW_ID);
  return(true);
}

struct focused_config_t *init_focused_config(void){
  assert(is_authorized_for_accessibility() == true);
  struct focused_config_t *c = calloc(1, sizeof(struct focused_config_t));

  c->enabled            = false;
  c->focused_space_ids  = vector_new();
  c->focused_window_ids = vector_new();
  c->ee                 = eventemitter_new();
  c->cur_space_id       = (size_t)get_space_id();
  eventemitter_add_listener(c->ee, EVENT_SPACE_ID_CHANGED, on_space_id_changed, (void *)c);
  return(c);
}

static int run_loop(void *VOID){
  struct focused_config_t *c = (struct focused_config_t *)VOID;

  c->event_tap = CGEventTapCreate(
    kCGSessionEventTap,
    kCGHeadInsertEventTap,
    0,
    focused_events,
    focused_event_handler, (void *)c
    );

  if (!c->event_tap) {
    log_error("ERROR: Unable to create keyboard event tap.");
    exit(1);
  }
  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, c->event_tap, 0);

  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
  CGEventTapEnable(c->event_tap, true);
  CFRunLoopRun();
}

bool start_focused(struct focused_config_t *cfg){
  cfg->started = timestamp();
  cfg->enabled = true;
//  pthread_create(&(cfg->loop_thread), NULL, run_loop, (void *)cfg);
}

bool stop_focused(struct focused_config_t *cfg){
  cfg->enabled = false;
  CGEventTapEnable(cfg->event_tap, false);
  //pthread_join(&(cfg->loop_thread), NULL);
  eventemitter_release(cfg->ee);
}
