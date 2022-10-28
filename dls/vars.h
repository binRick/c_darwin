#pragma once
#ifndef DLS_VARS_H
#define DLS_VARS_H
#include "dls/types.h"
const enum output_mode_type_t DEFAULT_OUTPUT_MODE;
const char                    *output_modes[OUTPUT_MODES_QTY + 1];
struct args_t                 *args;
struct whereami_report_t      *whereami;
char                          *DLS_EXECUTABLE, **DLS_EXECUTABLE_ARGV; int DLS_EXECUTABLE_ARGC;
//extern struct optparse_cmd *dls_cmd;
char                          **dls_cmd_argv;
#endif
