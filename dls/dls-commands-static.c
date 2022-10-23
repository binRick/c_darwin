#pragma once
#ifndef DLS_COMMANDS_STATIC_C
#define DLS_COMMANDS_STATIC_C
#include "dls/dls-commands.h"
static void __attribute__((constructor)) __constructor__dls(void);
static void __attribute__((destructor)) __destructor__dls(void);
static bool dls_normalize_arguments(int *argc, char *argv[]);
static struct Vector *dls_argv_to_arg_v(int argc, char *argv[]);
static void *dls_print_arg_v(char *title, char *color, int argc, char *argv[]);
static bool                   DARWIN_LS_DEBUG_MODE = false;
static void __at_exit(void);
static bool initialized;
static bool exited = false, was_icanon = false;
static void run_main_cmd(int argc, char *argv);
#endif
