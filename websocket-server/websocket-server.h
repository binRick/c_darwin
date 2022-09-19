#pragma once
#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H
//////////////////////////////////////
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
int websocket_server(void *WEBSOCKET_SERVER_PORT);
#endif
