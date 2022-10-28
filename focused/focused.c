////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app/utils/utils.h"
#include "base64_simple/src/base64simple.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "display/utils/utils.h"
#include "dock/utils/utils.h"
#include "focused/focused.h"
#include "log/log.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "ms/ms.h"
#include "msgbox/msgbox/msgbox.h"
#include "msgpack_c/include/msgpack.h"
#include "space/utils/utils.h"
#include "timestamp/timestamp.h"
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#define UNPACKED_BUFFER_SIZE    2048
#define REQ_ARR_QTY             5
struct focused_msg_t {
  size_t number;
  size_t ts;
  bool   debug, verbose;
};
static msg_Conn *server_listening_conn = NULL;
////////////////////////////////////////////
pthread_mutex_t *unpack_mutex, *pack_mutex;
static size_t   pack_retries           = 30;
static size_t   pack_retry_interval_ns = 1000 * 500;
static void init_mutexes();

static bool FOCUSED_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__focused(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_FOCUSED") != NULL) {
    log_debug("Enabling Focused Debug Mode");
    FOCUSED_DEBUG_MODE = true;
  }
  init_mutexes();
}

static char *event_name(int event){
  switch (event) {
  case msg_listening_ended: return("LISTENING_ENDED"); break;
  case msg_connection_lost: return("CONNECTION-LOST"); break;
  case msg_connection_closed: return("CONNECTION-CLOSED"); break;
  case msg_connection_ready: return("CONNECTION-READY"); break;
  case msg_error: return("ERROR"); break;
  case msg_request: return("REQUEST"); break;
  case msg_listening: return("LISTENING"); break;
  case msg_message: return("MESSAGE"); break;
  case msg_reply: return("REPLY"); break;
  default: return("UNKNOWN"); break;
  }
}

msg_Data *focused_msg_pack(struct focused_msg_t *msg, char *sender);
struct focused_msg_t *focused_msg_init();
struct focused_msg_t *focused_msg_unpack(msg_Data *data);

struct focused_msg_t *focused_msg_init(){
  struct focused_msg_t *msg = calloc(1, (sizeof(struct focused_msg_t)));

  return(msg);
}

static void init_mutexes(){
  pack_mutex   = calloc(1, sizeof(pthread_mutex_t));
  unpack_mutex = calloc(1, sizeof(pthread_mutex_t));
  assert(pthread_mutex_init(unpack_mutex, NULL) == 0);
  assert(pthread_mutex_init(pack_mutex, NULL) == 0);
}

struct focused_msg_t *focused_msg_unpack(msg_Data *tmp_data){
  /*
   * size_t retried = 0; bool have_lock = false;
   * while(retried<pack_retries && have_lock == false){
   * if(pthread_mutex_lock(unpack_mutex) != EXIT_SUCCESS){
   *  log_error("%lu/%lu> Failed to lock unpack mutex",retried,pack_retries);
   *  retried++;
   *  usleep(pack_retry_interval_ns);
   * }else{
   *  have_lock = true;
   *  log_debug("locked unpack mutex");
   * }
   * }
   */
  msg_Data data = (msg_Data){
    .num_bytes = tmp_data->num_bytes,
    .bytes     = strdup(tmp_data->bytes),
  };
  struct focused_msg_t *msg = calloc(1, sizeof(struct focused_msg_t));
  char                 *decoded_msg;
  size_t               rsize = 0;

  decoded_msg = base64simple_decode(msg_as_str(data), strlen(msg_as_str(data)), &rsize);
  log_info("decoded msg size: %lu", rsize);

  msgpack_unpacked      result;
  size_t                off = 0;
  msgpack_unpack_return ret;
  char                  unpacked_buffer[UNPACKED_BUFFER_SIZE];

  msgpack_unpacked_init(&result);
  ret = msgpack_unpack_next(&result, decoded_msg, rsize, &off);
  for (size_t i = 0; ret == MSGPACK_UNPACK_SUCCESS; i++) {
    msgpack_object obj = result.data;
    msgpack_object_print_buffer(unpacked_buffer, UNPACKED_BUFFER_SIZE, obj);
    log_debug("Object #%lu>  %s (type %d)", i, unpacked_buffer, obj.type);

    switch (i) {
    case 0: msg->number  = obj.via.i64; break;
    case 1: msg->ts      = obj.via.i64; break;
    case 2: msg->debug   = obj.via.boolean; break;
    case 3: msg->verbose = obj.via.boolean; break;
      /*
       * case 4: msg->arr_qty = obj.via.i64; break;
       * case 5:
       * msg->arr = calloc(obj.via.array.size+1,sizeof(int));
       * for(size_t idx=0;idx < obj.via.array.size; idx++){
       * msg->arr[idx] = obj.via.array.ptr[idx].via.i64;
       * }
       * msg->arr[obj.via.array.size] = '\0';
       * break;
       * case 6:  msg->string = stringfn_substring(obj.via.str.ptr,0,obj.via.str.size); break;
       * case 7:  msg->string2 = stringfn_substring(obj.via.str.ptr,0,obj.via.str.size); break;
       * case 8:  msg->sender = stringfn_substring(obj.via.str.ptr,0,obj.via.str.size); break;
       */
    }
    ret = msgpack_unpack_next(&result, decoded_msg, rsize, &off);
  }
  // msgpack_unpacked_destroy(&result);
  if (ret == MSGPACK_UNPACK_CONTINUE)
    log_debug(
      AC_GREEN AC_INVERSE "Unpacked Message" AC_RESETALL
      "\n\tnumber        :          %lu"
      "\n\tts            :          %lu"
      "\n\tdebug         :          %s"
      "\n\tverbose       :          %s"
      "\n%s",
      msg->number,
      msg->ts,
      (msg->debug == true) ? "Yes" : "No",
      (msg->verbose == true) ? "Yes" : "No",
      ""
      );
  else if (ret == MSGPACK_UNPACK_PARSE_ERROR)
    log_error("The data in the buf is invalid format.");

//    if(decoded_msg)
//    free(decoded_msg);

  // pthread_mutex_unlock(unpack_mutex);
  log_debug("unlocked unpack mutex");
  return(msg);
} /* focused_msg_unpack */

msg_Data *focused_msg_pack(struct focused_msg_t *msg, char *sender){
//  pthread_mutex_lock(pack_mutex);
  char            *encoded_msg;
  msg_Data        _data, *data = &(msg_Data){ .num_bytes = 0, .bytes = NULL };
  msgpack_sbuffer sbuf;
  msgpack_packer  pk;

  msgpack_sbuffer_init(&sbuf);
  msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
  msgpack_pack_int64(&pk, msg->number);
  msgpack_pack_int64(&pk, msg->ts);
  msgpack_pack_int(&pk, msg->debug);
  msgpack_pack_int(&pk, msg->verbose);
  encoded_msg     = base64simple_encode(sbuf.data, sbuf.size);
  _data           = msg_new_data(encoded_msg);
  data->num_bytes = _data.num_bytes;
  data->bytes     = strdup(_data.bytes);
  msgpack_sbuffer_destroy(&sbuf);
  log_info("<%d> " AC_BLUE AC_UNDERLINE AC_INVERSE "%s" AC_RESETALL " Message Packed :: " AC_GREEN AC_INVERSE "%s" AC_RESETALL "%s",
           getpid(),
           sender,
           bytes_to_string(data->num_bytes),
           ""
           );
  // msg_delete_data(_data);
//        if(encoded_msg)
//        free(encoded_msg);
//  pthread_mutex_unlock(pack_mutex);
  return(data);
}

static void msg_update_server(msg_Conn *conn, msg_Event event, msg_Data data) {
  log_debug("<%d> [%lld] Focused Server Update :: event %s (%d) :: ", getpid(), timestamp(), event_name(event), event);
  struct focused_config_t *c = NULL;
  if (event == msg_connection_ready) {
  }else if (event == msg_message || event == msg_request || event == msg_connection_closed) {
//    c = (struct focused_config_t *)conn->conn_context;
    //  log_debug("got context %ld", c->started);
  }

  switch (event) {
  case msg_connection_ready:
    conn->conn_context = NULL;
    log_debug("<%d> Focused Server Connection Ready ", getpid());
    break;
  case msg_listening_ended:
    log_debug("<%d> Focused Server Connection Ended ", getpid());
    break;
  case msg_connection_lost:
    log_debug("<%d> Focused Server Connection Lost ", getpid());
    break;
  case msg_connection_closed:
    log_debug("<%d> Focused Server Connection Closed ", getpid());
    // if(conn && conn->conn_context)
    free(conn->conn_context);
    break;
  case msg_reply:
    log_debug("<%d> Focused Server Message Reply ", getpid());
  case msg_message:
    log_debug("<%d> Focused Server Message ", getpid());
  case msg_request:
    log_debug(
      "<%d> [%lld] Focused Server ::"
      " %s Request Message from %s:%d :: Event Type: %s"
      "%s",
      getpid(),
      timestamp(),
      bytes_to_string(data.num_bytes),
      msg_ip_str(conn),
      conn->remote_port,
      event_name(event),
      ""
      );
    if (focused_msg_unpack(&data) == NULL) {
      log_error("Failed to unpack msg");
      //exit(EXIT_FAILURE);
    }
    log_debug("unpacked OK");
    struct focused_msg_t *res = focused_msg_init();
    res->number  = timestamp();
    res->ts      = (size_t)timestamp();
    res->debug   = true;
    res->verbose = false;
    msg_Data *res_data = focused_msg_pack(res, "server");
    msg_send(conn, *res_data);
    //free(res);
    log_debug("<%d> Focused Server Response Message Sent", getpid());
    break;
  case msg_listening:
    log_debug("<%d> Focused Server Listening ", getpid());
    server_listening_conn = conn;
    break;
  case msg_error: log_error("<%d> Server Server Error: %s ", getpid(), msg_as_str(data)); break;
  default:
    log_error("Unknown event");
    break;
  } /* switch */
}   /* msg_update_server */

static void msg_update_client(msg_Conn *conn, msg_Event event, msg_Data data) {
  log_info("<%d> Focused Client Update :: %s", getpid(), event_name(event));
  unsigned long           started = timestamp();
  struct focused_config_t *c      = NULL;
  if (event == msg_connection_ready)
    c = (struct focused_config_t *)conn->conn_context;
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
  case msg_message:
    log_info("<%d> Focused Client Received Message",
             getpid()
             );
    if (focused_msg_unpack(&data) == NULL) {
      log_error("Failed to unpack msg");
//      exit(EXIT_FAILURE);
    }
    // msg_delete_data(data);
    break;
  case msg_reply:
    log_info("<%d> Focused Client Received Reply",
             getpid()
             );
    log_info("Got server response #%lu/%lu", c->server->cl_recv_msgs, c->server->cl_send_msgs_qty);
    //     c->server->cl_recv_msgs++;
//    c->server->cl_recv_bytes += data.num_bytes;
    if (focused_msg_unpack(&data) == NULL) {
      log_error("Failed to unpack msg");
      //    exit(EXIT_FAILURE);
    }
    msg_delete_data(data);
    break;
  case msg_request:
    log_info("<%d> Focused Client Received Request",
             getpid()
             );
    if (focused_msg_unpack(&data) == NULL) {
      log_error("Failed to unpack msg");
      //  exit(EXIT_FAILURE);
    }
    //  if (c->server->cl_recv_msgs == c->server->cl_send_msgs_qty) {
    //log_info("Sent all messages. disconnecting");
//      msg_disconnect(conn);
    //}
    break;
  case msg_listening:
    log_info("<%d> Focused Client Listening",
             getpid()
             );
    break;
  case msg_connection_ready:
    for (size_t i = 0; i < c->server->cl_send_msgs_qty; i++) {
      struct focused_msg_t *req = focused_msg_init();
      req->number  = timestamp();
      req->ts      = (size_t)timestamp();
      req->debug   = true;
      req->verbose = false;
      msg_Data *req_data = focused_msg_pack(req, "client");
      size_t   num_bytes = req_data->num_bytes;
      msg_send(conn, *req_data);
      //, (void*)c);
      //c->server->cl_sent_bytes += num_bytes;
      //c->server->cl_sent_msgs++;
      //msg_delete_data(*req_data);
      usleep(c->server->cl_send_msg_interval_ns);
    }
    break;
  case msg_error: log_error("<%d> Client Server Error: %s ", getpid(), msg_as_str(data)); break;
  default:
    log_error("Unknown event");
    break;
  } /* switch */
}   /* msg_update_client */

static int focus_client(void *VOID){
  unsigned long           started = timestamp();
  struct focused_config_t *c      = (struct focused_config_t *)VOID;

  if (c->server->cl_debug_mode == true)
    log_info("<%d> Focused Client Connecting to %s", getpid(), c->server->uri);
  msg_connect(c->server->uri, msg_update_client, (void *)c);
  if (c->server->cl_debug_mode == true)
    log_info("<%d> Focused Client Connected to %s in %s", getpid(), c->server->uri, milliseconds_to_string(timestamp() - started));
  while (c->server->cl_recv_msgs < c->server->cl_send_msgs_qty || 1 == 1)
    msg_runloop(10);
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
  while (c->server->svr_recv_msgs < 100)
    msg_runloop(10);
  msg_unlisten(server_listening_conn);
  msg_runloop(10);
  log_debug("Server Ended");
  return(EXIT_SUCCESS);
}

static void on_space_id_changed(void *event_data, void *VOID){
  struct focused_config_t *c       = (struct focused_config_t *)VOID;
  size_t                  space_id = (size_t)event_data;

  log_info("on_space_id_changed, spaceid: %lu|focused window ids qty:%lu", space_id, vector_size(c->focused_window_ids));
  for (size_t i = 0; vector_size(c->focused_window_ids); i++) {
    size_t window_id = (size_t)vector_get(c->focused_window_ids, i);
    if (window_id == 0)
      break;
    log_debug("moving window %lu space %lu", window_id, space_id);
    window_id_send_to_space(window_id, (int)space_id);
    struct window_info_t *w = get_window_id_info(window_id);
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

  if (event_flags & kCGEventFlagMaskCommand)
    stringbuffer_append_string(sb, "command+");
  if (event_flags & kCGEventFlagMaskControl)
    stringbuffer_append_string(sb, "control+");
  stringbuffer_append_string(sb, kc);
  char *keys = stringbuffer_to_string(sb);

  stringbuffer_release(sb);
  size_t space_id = (size_t)get_current_space_id();

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
  c->cur_space_id       = (size_t)get_current_space_id();
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
