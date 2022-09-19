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
//////////////////////////////////////
struct http_server_s *server;
///////////////////////////////////////////////////////////////////////////////////////////////////////
int httpserver_main();
#endif
