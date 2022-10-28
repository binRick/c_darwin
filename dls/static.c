#pragma once
#ifndef DLS_STATIC_C
#define DLS_STATIC_C
#include "dls/types.h"
static void __attribute__((constructor)) __constructor__dls(void);
static void __attribute__((destructor)) __destructor__dls(void);
static bool dls_normalize_arguments(int *argc, char *argv[]);
static struct Vector *dls_argv_to_arg_v(int argc, char *argv[]);
static void *dls_print_arg_v(char *title, char *color, int argc, char *argv[]);
static bool DARWIN_LS_DEBUG_MODE = false;
static void __at_exit(void);
static bool initialized;
static bool exited = false, was_icanon = false;
void run_main_cmd(int argc, char *argv[]);
//static enum output_mode_type_t DEFAULT_OUTPUT_MODE  = OUTPUT_MODE_TABLE;

#endif
