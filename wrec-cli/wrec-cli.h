#pragma once
#include "cargs/include/cargs.h"
#include "wrec/wrec.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
///////////////////////////////////
#define DEFAULT_VERBOSE    false
#define DEFAULT_MODE       "test"
///////////////////////////////////
static int parse_args(int argc, char *argv[]);
int debug_args();

///////////////////////////////////
struct modes_t {
  char *name;
  char *description;
  int  (*handler)();
} modes_t;

struct args_t {
  char *mode;
  bool verbose;
};

static struct args_t args = {
  DEFAULT_MODE,
  DEFAULT_VERBOSE,
};


static struct modes_t    modes[] = {
  { .name = "debug_args", .description = "Debug Arguments", .handler = debug_args   },
  { .name = "wrec0",      .description = "Wrec0",           .handler = wrec0        },
  { .name = "list",       .description = "List Windows",    .handler = list_windows },
  { NULL },
};

static struct cag_option options[] = {
  { .identifier     = 'm',
    .access_letters = "m",
    .access_name    = "mode",
    .value_name     = "MODE",
    .description    = "Mode" },
  { .identifier     = 'v',
    .access_letters = "v",
    .access_name    = "verbose",
    .value_name     = NULL,
    .description    = "Verbose Mode" },
  { .identifier     = 'h',
    .access_letters = "h",
    .access_name    = "help",
    .description    = "Shows the command help" }
};

