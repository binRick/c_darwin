#pragma once
#ifndef DLS_DEFINES_H
#define DLS_DEFINES_H
#define OPTPARSE_HELP_USAGE_STYLE              1
#define OPTPARSE_HELP_MAX_LINE_WIDTH           120
#define OPTPARSE_HELP_FLOATING_DESCRIPTIONS    0
#define OPTPARSE_HELP_LETTER_CASE              2
#define NDEBUG                                 1
#define DEBUG_ARGV                             false
#define NORMALIZE_ARGV                         false
#define DEFAULT_PROGRESS_BAR_ENABLED           true
#define DEFAULT_CAPTURE_TYPE                   CAPTURE_TYPE_WINDOW
#define MAX_SUBCOMMANDS                        1024
#define DLS_NAME                               "dls"
#define DLS_VERSION                            "1.00"
#define DLS_DESC                               "This program lists Darwin Objects"
#define DLS_ABOUT                              DLS_NAME "v" DLS_VERSION " - " DLS_DESC
#define DLS_OPERANDS                           "[COMMAND...]"

#endif
