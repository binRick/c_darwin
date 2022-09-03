#pragma once
//////////////////////////////////////
#include "c_eventemitter/include/eventemitter.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "osx-keys/osx-keys.h"
#include <assert.h>
#include <ctype.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CoreFoundation.h>
#include <err.h>
#include <errno.h>
#include <inttypes.h>
#include <libgen.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <poll.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
//////////////////////////////////////
enum focused_server_proto_t {
  FOCUSED_SERVER_PROTO_TCP,
  FOCUSED_SERVER_PROTO_UDP,
  FOCUSED_SERVER_PROTOS_QTY,
};
static char *focused_server_proto_names[FOCUSED_SERVER_PROTOS_QTY] = {
  [FOCUSED_SERVER_PROTO_UDP] = "udp",
  [FOCUSED_SERVER_PROTO_TCP] = "tcp",
};
enum focused_events_t {
  EVENT_START  = 100,
  EVENT_MIDDLE = 200,
  EVENT_END    = 300,
  EVENT_SPACE_ID_CHANGED = 400,
};
struct focused_server_t {
  enum focused_server_proto_t proto;
  int                         port;
  char                        *host;
  pthread_t                   server_thread, client_thread;
  char                        *uri;
  size_t                      svr_recv_msgs, cl_recv_msgs, svr_events_qty, cl_events_qty, svr_recv_bytes, cl_recv_bytes, svr_sent_bytes, cl_sent_bytes, cl_sent_msgs, svr_sent_msgs;
  bool                        enabled, svr_debug_mode, cl_debug_mode;
  size_t                      cl_send_msgs_qty, cl_send_msg_interval_ns;
};
static const struct focused_server_t *DEFAULT_SERVER_CONFIG = &(struct focused_server_t){
  .port             = 44112,
  .host             = "127.0.0.1",
  .proto            = FOCUSED_SERVER_PROTO_TCP,
  .uri              = NULL,
  .enabled          = true,
  .svr_recv_msgs    = 0, .svr_events_qty = 0, .svr_debug_mode = false,
  .cl_recv_msgs     = 0, .cl_events_qty = 0, .cl_debug_mode = false,
  .cl_send_msgs_qty = 5, .cl_send_msg_interval_ns = 50 * 1,
};
struct focused_config_t {
  unsigned long           started;
  bool                    enabled, focus_all_space_ids;
  size_t                  focused_window_width;
  size_t                  cur_space_id;
  struct Vector           *focused_space_ids, *focused_window_ids;
  struct EventEmitter     *ee;
  CFMachPortRef           event_tap;
  pthread_t               loop_thread;
  struct focused_server_t *server;
};
bool add_focused_window_id(struct focused_config_t *cfg, size_t WINDOW_ID);
struct focused_config_t *init_focused_config(void);
bool start_server(struct focused_config_t *cfg);
bool stop_server(struct focused_config_t *cfg);
bool run_client(struct focused_config_t *cfg);
