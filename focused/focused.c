////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app-utils/app-utils.h"
#include "base64_simple/src/base64simple.h"
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
#include "msgpack_c/include/msgpack.h"
#include "space-utils/space-utils.h"
#include "timestamp/timestamp.h"
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
////////////////////////////////////////////
#include "focused/focused.h"
#define UNPACKED_BUFFER_SIZE    2048

static bool FOCUSED_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__focused(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_FOCUSED") != NULL) {
    log_debug("Enabling Focused Debug Mode");
    FOCUSED_DEBUG_MODE = true;
  }
}

struct focused_msg_t {
  size_t number;
  size_t ts;
  bool   debug, verbose;
  char   *string;
};

static void msg_update_server(msg_Conn *conn, msg_Event event, msg_Data data) {
  log_debug("<%d> [%lld] Focused Server Update :: event %d :: ", getpid(), timestamp(), event);
  struct focused_config_t *c = NULL;
  char                    *s, **ep;
  if (event == msg_connection_ready) {
    conn->conn_context = NULL;
  }else if (event == msg_message || event == msg_request || event == msg_connection_closed) {
  }
  if (conn->conn_context) {
    c = (struct focused_config_t *)conn->conn_context;
    log_debug("got context %ld", c->started);
  }

  if (event == msg_connection_lost) {
    log_debug("<%d> Focused Server Connection Lost ", getpid());
  }else if (event == msg_connection_closed) {
    log_debug("<%d> Focused Server Connection Closed ", getpid());
    //free(conn->conn_context);
  }else if (event == msg_message) {
    log_debug("<%d> Focused Server Message ", getpid());
  }else if (event == msg_error) {
    log_error("Server: Error: %s", msg_as_str(data));
  }else if (event == msg_request) {
    // if(c->server->svr_debug_mode == true)
    log_debug(
      "<%d> [%lld] Focused Server ::"
      " %s Request Message from %s:%d ::"
      "'" AC_YELLOW AC_INVERSE "%s" AC_RESETALL "'"
      "%s",
      getpid(),
      timestamp(),
      bytes_to_string(data.num_bytes),
      msg_ip_str(conn),
      conn->remote_port,
      msg_as_str(data),
      ""
      );
    struct focused_msg_t *reply = calloc(1, sizeof(struct focused_msg_t));
    struct focused_msg_t *req   = calloc(1, sizeof(struct focused_msg_t));
    char                 *decoded_req;
    size_t               rsize = 0;
    {
      decoded_req = base64simple_decode(msg_as_str(data), strlen(msg_as_str(data)), &rsize);
      log_info("decoded req size: %lu", rsize);
    }

    msgpack_unpacked      result;
    size_t                off = 0;
    msgpack_unpack_return ret;
    char                  unpacked_buffer[UNPACKED_BUFFER_SIZE];
    msgpack_unpacked_init(&result);
    ret = msgpack_unpack_next(&result, decoded_req, rsize, &off);
    for (size_t i = 0; ret == MSGPACK_UNPACK_SUCCESS; i++) {
      msgpack_object obj = result.data;
      msgpack_object_print_buffer(unpacked_buffer, UNPACKED_BUFFER_SIZE, obj);
      switch (i) {
      case 0: req->number  = (size_t)unpacked_buffer; break;
      case 1: req->ts      = (size_t)unpacked_buffer; break;
      case 2: req->debug   = (bool)unpacked_buffer; break;
      case 3: req->verbose = (bool)unpacked_buffer; break;
      case 4:
        req->string = (char *)unpacked_buffer;
        if (req->string[0] == '"' && req->string[strlen(req->string) - 1] == '"') {
          req->string = stringfn_mut_substring(req->string, 1, strlen(req->string) - 2);
        }
        break;
      }
      ret = msgpack_unpack_next(&result, decoded_req, rsize, &off);
    }
    msgpack_unpacked_destroy(&result);
    free(decoded_req);

    if (ret == MSGPACK_UNPACK_CONTINUE) {
      log_debug(
        AC_GREEN AC_INVERSE "Unpacked Message" AC_RESETALL
        "\n\tnumber        :          %lu"
        "\n\tts            :          %lu"
        "\n\tdebug         :          %s"
        "\n\tverbose       :          %s"
        "\n\tstring        :          %s"
        "\n%s",
        req->number,
        req->ts,
        (req->debug == true) ? "Yes" : "No",
        (req->verbose == true) ? "Yes" : "No",
        req->string,
        ""
        );
    }else if (ret == MSGPACK_UNPACK_PARSE_ERROR) {
      printf("The data in the buf is invalid format.\n");
    }

    asprintf(&(reply->string), "%ld", (size_t)strtoimax(((struct focused_msg_t *)&data)->string, &ep, 10) * 4096);
    reply->number = (size_t)((struct focused_msg_t *)&data)->number * 4096;
    reply->ts     = (size_t)timestamp();
    reply->debug  = true;
    //msg_Data reply_data = msg_new_data_space(sizeof(struct focused_msg_t));a
//      reply_data.bytes = (void*)
//  populate_buffer(data.bytes /* type "char *" */, data.num_bytes /* type size_t */);
    data = msg_new_data(reply->string);
    log_debug("reply s:%s", reply->string);
    msg_send(conn, data);
    //  c->server->svr_recv_msgs++;
    log_debug("<%d> Focused Server Message Sent", getpid());
  }
} /* msg_update_server */

static void msg_update_client(msg_Conn *conn, msg_Event event, msg_Data data) {
  unsigned long           started = timestamp();

  struct focused_config_t *c;

  c = (struct focused_config_t *)conn->conn_context;
  char *s;

  c->server->cl_events_qty++;
  if (c->server->cl_debug_mode == true) {
    log_info("<%d> Focused Client Update #%lu :: %d", getpid(), c->server->cl_events_qty, event);
  }
  switch (event) {
  case msg_listening_ended:
    log_info("<%d> Focused Client Listening Ended after %s", getpid(), milliseconds_to_string(timestamp() - started));
    break;
  case msg_connection_lost:
    log_info("<%d> Focused Client Connection Lost after %s", getpid(), milliseconds_to_string(timestamp() - started));
    break;
  case msg_connection_closed:
    log_info("<%d> Focused Client Connection Closed after %s", getpid(), milliseconds_to_string(timestamp() - started));
    break;
  case msg_reply:
    c->server->cl_recv_msgs++;
    c->server->cl_recv_bytes += data.num_bytes;
    if (c->server->cl_debug_mode == true) {
      log_info("<%d> Focused Client Got Reply #%lu> '" AC_YELLOW AC_INVERSE "%s" AC_RESETALL "' in %s", getpid(), c->server->cl_recv_msgs, msg_as_str(data), milliseconds_to_string(timestamp() - started));
    }
    if (c->server->cl_recv_msgs == c->server->cl_send_msgs_qty) {
      msg_disconnect(conn);
    }
    break;
  case msg_connection_ready:
    for (size_t i = 0; i < c->server->cl_send_msgs_qty; i++) {
      {
        //      init message
        struct focused_msg_t *req = calloc(1, (sizeof(struct focused_msg_t)));
        char                 *encoded_req;
        //      prepare message
        {
          req->number  = timestamp();
          req->ts      = (size_t)timestamp();
          req->debug   = true;
          req->verbose = false;
          asprintf(&(req->string), "%lu", (size_t)timestamp());
        }
        //      pack message
        {
          msgpack_sbuffer sbuf;
          msgpack_packer  pk;
          {
            msgpack_sbuffer_init(&sbuf);
            msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
            msgpack_pack_int64(&pk, req->number);
            msgpack_pack_int64(&pk, req->ts);
            msgpack_pack_int(&pk, req->debug);
            msgpack_pack_int(&pk, req->verbose);
            msgpack_pack_str_with_body(&pk, (void *)req->string, strlen(req->string));
          }
          { //      encode message
            encoded_req = base64simple_encode(sbuf.data, sbuf.size);
            //      create data
            data = msg_new_data(encoded_req);
          }
          {
            log_info("<%d> Focused Client Sending " AC_GREEN AC_INVERSE "%s" AC_RESETALL " Encoded Message",
                     getpid(),
                     bytes_to_string(data.num_bytes)
                     );
          }
          {
            msgpack_sbuffer_destroy(&sbuf);
            free(encoded_req);
            free(req);
          }
        }
      }
      {
        //    send message
        msg_get(conn, data, (void *)c);
        c->server->cl_sent_msgs++;
        c->server->cl_sent_bytes += data.num_bytes;
      }
      {
        //    delete message
        msg_delete_data(data);
      }
      {
        if (c->server->cl_debug_mode == true) {
          log_info("<%d> Focused Client deleted data", getpid());
        }
      }
      usleep(c->server->cl_send_msg_interval_ns);
    }
    break;
  } /* switch */
} /* msg_update_client */

static int focus_client(void *VOID){
  unsigned long           started = timestamp();
  struct focused_config_t *c      = (struct focused_config_t *)VOID;

  if (c->server->cl_debug_mode == true) {
    log_info("<%d> Focused Client Connecting to %s", getpid(), c->server->uri);
  }
  msg_connect(c->server->uri, msg_update_client, (void *)c);
  if (c->server->cl_debug_mode == true) {
    log_info("<%d> Focused Client Connected to %s in %s", getpid(), c->server->uri, milliseconds_to_string(timestamp() - started));
  }
  while (c->server->cl_recv_msgs < c->server->cl_send_msgs_qty) {
    msg_runloop(10);
  }
  log_info("<%d> Focused Client Success :: Received %lu messages of %s, Sent %lu messages of %s in %s ",
           getpid(),
           c->server->cl_recv_msgs, bytes_to_string(c->server->cl_recv_bytes),
           c->server->cl_sent_msgs, bytes_to_string(c->server->cl_sent_bytes),
           milliseconds_to_string(timestamp() - started)
           );
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
  {
    c->server          = calloc(1, sizeof(struct focused_server_t));
    c->server->port    = DEFAULT_SERVER_CONFIG->port;
    c->server->host    = DEFAULT_SERVER_CONFIG->host;
    c->server->proto   = DEFAULT_SERVER_CONFIG->proto;
    c->server->enabled = DEFAULT_SERVER_CONFIG->enabled;
    asprintf(&c->server->uri, "%s://%s:%d", focused_server_proto_names[c->server->proto], c->server->host, c->server->port);
    c->server->svr_recv_msgs           = c->server->svr_recv_bytes = c->server->svr_events_qty = c->server->svr_sent_bytes = c->server->svr_sent_msgs = 0;
    c->server->cl_recv_msgs            = c->server->cl_recv_bytes = c->server->cl_events_qty = c->server->cl_sent_bytes = c->server->cl_sent_msgs = 0;
    c->server->cl_debug_mode           = DEFAULT_SERVER_CONFIG->cl_debug_mode;
    c->server->svr_debug_mode          = DEFAULT_SERVER_CONFIG->svr_debug_mode;
    c->server->cl_send_msgs_qty        = DEFAULT_SERVER_CONFIG->cl_send_msgs_qty;
    c->server->cl_send_msg_interval_ns = DEFAULT_SERVER_CONFIG->cl_send_msg_interval_ns;
  }
  if (FOCUSED_DEBUG_MODE == true) {
    c->server->cl_debug_mode  = true;
    c->server->svr_debug_mode = true;
  }

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
