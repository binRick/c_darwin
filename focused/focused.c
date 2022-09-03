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
#include "msgbox/msgbox/msgbox.h"
#include "space-utils/space-utils.h"
#include "timestamp/timestamp.h"
#include <pthread.h>
#include <stdbool.h>
////////////////////////////////////////////
#include "focused/focused.h"

static bool FOCUSED_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__focused(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_FOCUSED") != NULL) {
    log_debug("Enabling Focused Debug Mode");
    FOCUSED_DEBUG_MODE = true;
  }
}

static void msg_update_server(msg_Conn *conn, msg_Event event, msg_Data data) {
  struct focused_config_t *c = (struct focused_config_t *)conn->conn_context;

  log_debug("<%d> Focused Server Update :: ", getpid());
  if (event == msg_connection_lost) {
    log_debug("<%d> Focused Server Connection Lost ", getpid());
  }
  if (event == msg_connection_closed) {
    log_debug("<%d> Focused Server Connection Closed ", getpid());
  }
  if (event == msg_message) {
    log_debug("<%d> Focused Server Message ", getpid());
  }
  if (event == msg_error) {
    log_error("Server: Error: %s", msg_as_str(data));
  }
  if (event == msg_request) {
    log_debug("<%d> Focused Server Message Request received", getpid());
    msg_send(conn, data);
    c->server->svr_recv_msgs++;
    log_debug("<%d> Focused Server Message Sent", getpid());
  }
}

static void msg_update_client(msg_Conn *conn, msg_Event event, msg_Data data) {
  struct focused_config_t *c = (struct focused_config_t *)conn->conn_context;

  log_info("<%d> Focused Client Update :: %s", getpid(), c->server->uri);
  if (event == msg_connection_ready) {
    msg_Data data = msg_new_data("hello!");
    log_info("<%d> Focused Client connection ready", getpid());
    msg_get(conn, data, msg_no_context);
    log_info("<%d> Focused Client got data", getpid());
    msg_delete_data(data);
    log_info("<%d> Focused Client deleted data", getpid());
  }
  if (event == msg_reply) {
    c->server->cl_recv_msgs++;
    log_info("<%d> Focused Client Got Reply '%s'", getpid(), msg_as_str(data));
  }
}

static int focus_client(void *VOID){
  struct focused_config_t *c = (struct focused_config_t *)VOID;

  log_info("<%d> Focused Client Connecting to %s", getpid(), c->server->uri);
  msg_connect(c->server->uri, msg_update_client, (void *)c);
  log_info("<%d> Focused Client Connected to %s", getpid(), c->server->uri);
  while (c->server->cl_recv_msgs < 1) {
    msg_runloop(10);
  }
  return(EXIT_SUCCESS);
}

static int focus_server(void *VOID){
  struct focused_config_t *c = (struct focused_config_t *)VOID;

  log_debug("<%d> Focused Server Binding %s", getpid(), c->server->uri);
  msg_listen(c->server->uri, msg_update_server);
  log_debug("<%d> Focused Server Listening on %s", getpid(), c->server->uri);
  while (c->server->svr_recv_msgs < 100) {
    msg_runloop(10);
  }
  return(EXIT_SUCCESS);
}

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
  c->server          = calloc(1, sizeof(struct focused_server_t));
  c->server->port    = DEFAULT_SERVER_CONFIG->port;
  c->server->host    = DEFAULT_SERVER_CONFIG->host;
  c->server->proto   = DEFAULT_SERVER_CONFIG->proto;
  c->server->enabled = DEFAULT_SERVER_CONFIG->enabled;
  asprintf(&c->server->uri, "%s://%s:%d", focused_server_proto_names[c->server->proto], c->server->host, c->server->port);

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

bool run_client(struct focused_config_t *cfg){
  //pthread_create(&(cfg->server->client_thread), NULL, focus_client, (void *)cfg);
  focus_client((void *)cfg);
}

bool start_server(struct focused_config_t *cfg){
  cfg->started = timestamp();
  cfg->enabled = true;
//  pthread_create(&(cfg->loop_thread), NULL, run_loop, (void *)cfg);
  pthread_create(&(cfg->server->server_thread), NULL, focus_server, (void *)cfg);
}

bool stop_server(struct focused_config_t *cfg){
  cfg->enabled = false;
  CGEventTapEnable(cfg->event_tap, false);
  //pthread_join(&(cfg->loop_thread), NULL);
  pthread_join(&(cfg->server->server_thread), NULL);
  eventemitter_release(cfg->ee);
}
