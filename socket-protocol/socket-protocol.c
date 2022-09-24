#pragma once
#ifndef SOCKET_PROTOCOL_C
#define SOCKET_PROTOCOL_C
////////////////////////////////////////////
#include "socket-protocol/socket-protocol.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "config-utils/config-utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "process/utils/utils.h"
#include "timestamp/timestamp.h"
#include "window/utils/utils.h"
////////////////////////////////////////////
static bool SOCKET_PROTOCOL_DEBUG_MODE = false;

static JSON_Value *bool_to_json_value(bool b){
  return(json_value_init_boolean(b));
}

static JSON_Value *json_value_to_json_value(JSON_Value *j){
  return(j);
}

static JSON_Value *vector_to_json_array(struct Vector *v){
  JSON_Value *j = json_value_init_array();

  return(j);
}
const char                                   *socket_protocol_request_type_keys[SOCKET_PROTOCOL_REQUEST_TYPES_QTY + 1] = {
  [SOCKET_PROTOCOL_REQUEST_GET_CONFIG]      = "get_config",
  [SOCKET_PROTOCOL_REQUEST_GET_PIDS]        = "get_pids",
  [SOCKET_PROTOCOL_REQUEST_GET_WINDOWS]     = "get_windows",
  [SOCKET_PROTOCOL_REQUEST_FOCUS_WINDOW_ID] = "focus_window_id",
};
const struct socket_protocol_event_handler_t socket_protocol_event_handlers[SOCKET_PROTOCOL_REQUEST_TYPES_QTY + 1] = {
  [SOCKET_PROTOCOL_REQUEST_FOCUS_WINDOW_ID] = {
    .handler = (socket_protocol_event_handler_b) ^ bool(void *request_param){
      focus_window_id((size_t)request_param);
      return(((size_t)get_focused_window_id() == (size_t)request_param) ? true : false);
    },
    .encoder = (socket_protocol_encoder_b)bool_to_json_value,
  },
  [SOCKET_PROTOCOL_REQUEST_GET_CONFIG] =      {
    .handler = (socket_protocol_event_handler_b) ^ JSON_Value * (void){ return(get_json_config());                         },
    .encoder = (socket_protocol_encoder_b)json_value_to_json_value,
  },
  [SOCKET_PROTOCOL_REQUEST_GET_WINDOWS] =     {
    .handler = (socket_protocol_event_handler_b) ^ struct Vector *(void){ return(get_window_infos_v());                      },
    .encoder = (socket_protocol_encoder_b)vector_to_json_array,
  },
  [SOCKET_PROTOCOL_REQUEST_GET_PIDS] =        {
    .handler = (socket_protocol_event_handler_b) ^ struct Vector *(void){ return(get_all_process_infos_v());                 },
    .encoder = (socket_protocol_encoder_b)vector_to_json_array,
  },
};

char *handle_socket_protocol_request(enum socket_protocol_request_type_id_t type, void *request_params){
  return(json_serialize_to_string(socket_protocol_event_handlers[type].handler(request_params)));
}

////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__socket_protocol(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_socket_protocol") != NULL) {
    log_debug("Enabling socket-protocol Debug Mode");
    SOCKET_PROTOCOL_DEBUG_MODE = true;
  }
}
#endif
#include "window/info/info.h"
