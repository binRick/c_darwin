#pragma once
#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H
//////////////////////////////////////
#include "chan/src/chan.h"
#include "httpserver/httpserver.h"
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
enum websocketserver_chan_type_id_t {
  WEBSOCKET_SERVER_CONNECTION,
  WEBSOCKET_SERVER_NEW_SESSION,
  WEBSOCKET_SERVER_MESSAGE,
  WEBSOCKET_SERVERS_QTY,
};
struct websocketserver_chan_t {
  chan_t *chan;
  size_t buffer_qty;
};
struct libforks_param_t {
  int socket_fd;
  int websocket_server_port;
};
//int websocket_server(void *WEBSOCKET_SERVER_PORT);
int websocket_server(void *P);
#endif
