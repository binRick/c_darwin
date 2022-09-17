#pragma once
#ifndef LS_WIN_HTTPSERVER_H
#define LS_WIN_HTTPSERVER_H
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#define CONTENT_TYPE_JAVASCRIPT    "application/javascript"
#define CONTENT_TYPE_CSS           "text/css"
int httpserver_main();
#endif
