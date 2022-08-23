#pragma once
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
///////////////////////////////////
#include "cargs/include/cargs.h"
#include "wrec-capture/wrec-capture.h"
#include "wrec-common/wrec-common.h"
#include "wrec-utils/wrec-utils.h"
///////////////////////////////////
#define DEFAULT_VERBOSE                        false
#define DEFAULT_MODE                           "list"
#define DEFAULT_WINDOW_ID                      0
#define DEFAULT_MAX_RECORD_FRAMES              100
#define DEFAULT_MAX_RECORD_DURATION_SECONDS    30
#define DEFAULT_FRAMES_PER_SECOND              3
#define DEFAULT_MODE_LIST                      false
#define DEFAULT_MODE_CAPTURE                   false
#define DEFAULT_MODE_DEBUG_ARGS                false
#define DEFAULT_RESIZE_TYPE                    RESIZE_BY_NONE
#define DEFAULT_RESIZE_VALUE                   1
#define DEFAULT_APPLICATION_NAME_GLOB          "*"
///////////////////////////////////
