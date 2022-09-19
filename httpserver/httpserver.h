#pragma once
#ifndef __HTTPSERVER_H
#define __HTTPSERVER_H
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
#define DARWIN_LS_HTTPSERVER_PORT        49225
#define DARWIN_LS_HTTPSERVER_HOST        "127.0.0.1"
#define DARWIN_LS_HTTPSERVER_PROTOCOL    "http"
//////////////////////////////////////
struct http_server_s *server;
///////////////////////////////////////////////////////////////////////////////////////////////////////
int httpserver_main();
#endif
