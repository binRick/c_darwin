#pragma once
#ifndef SOCKET_PROTOCOL_H
#define SOCKET_PROTOCOL_H
//////////////////////////////////////
#include "parson/parson.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
enum socket_protocol_request_type_id_t {
  SOCKET_PROTOCOL_REQUEST_FOCUS_WINDOW_ID,
  SOCKET_PROTOCOL_REQUEST_GET_CONFIG,
  SOCKET_PROTOCOL_REQUEST_GET_HOTKEYS,
  SOCKET_PROTOCOL_REQUEST_ENABLE_HOTKEY,
  SOCKET_PROTOCOL_REQUEST_DISABLE_HOTKEY,
  SOCKET_PROTOCOL_REQUEST_GET_PIDS,
  SOCKET_PROTOCOL_REQUEST_GET_WINDOWS,
  SOCKET_PROTOCOL_REQUEST_GET_SPACES,
  SOCKET_PROTOCOL_REQUEST_GET_DISPLAYS,
  SOCKET_PROTOCOL_REQUEST_FOCUS_WINDOW,
  SOCKET_PROTOCOL_REQUEST_CAPTURE_WINDOW,
  SOCKET_PROTOCOL_REQUEST_EXTRACT_WINDOW,
  SOCKET_PROTOCOL_REQUEST_TYPES_QTY,
};
enum socket_protocol_response_type_id_t {
  SOCKET_PROTOCOL_RESPONSE_TYPE_ARRAY,
  SOCKET_PROTOCOL_RESPONSE_TYPE_OBJECT,
  SOCKET_PROTOCOL_RESPONSE_TYPE_NUMBER,
  SOCKET_PROTOCOL_RESPONSE_TYPE_STRING,
  SOCKET_PROTOCOL_RESPONSE_TYPE_BOOLEAN,
  SOCKET_PROTOCOL_RESPONSE_TYPE_BINARY,
  SOCKET_PROTOCOL_RESPONSE_TYPE_BASE64,
  SOCKET_PROTOCOL_RESPONSE_TYPES_QTY,
};
struct socket_protocol_request_t {
  enum socket_protocol_request_type_id_t type;
  const char                             *data;
  size_t                                 size;
  unsigned long                          ts;
};
struct socket_protocol_response_t {
  enum socket_protocol_response_type_id_t type;
  const char                              *data;
  size_t                                  size;
  unsigned long                           ts, dur;
  bool                                    sent, success;
};
struct socket_protocol_event_type_t {
  const char                              *key;
  enum socket_protocol_request_type_id_t  request_type_id;
  enum socket_protocol_response_type_id_t response_type_id;
  struct socket_protocol_response_t       *response;
  struct socket_protocol_request_t        *request;
};
typedef void *(^socket_protocol_event_handler_b)(void *);
typedef JSON_Value *(^socket_protocol_encoder_b)(void *);
struct socket_protocol_event_handler_t {
  socket_protocol_event_handler_b handler;
  socket_protocol_encoder_b       encoder;
};
const struct socket_protocol_event_handler_t socket_protocol_event_handlers[SOCKET_PROTOCOL_REQUEST_TYPES_QTY + 1];
const char                                   *socket_protocol_request_type_keys[SOCKET_PROTOCOL_REQUEST_TYPES_QTY + 1];
#endif
