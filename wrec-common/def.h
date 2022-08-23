#pragma once
#ifndef WREC_COMMON_DEF
#define WREC_COMMON_DEF
///////////////////////////////////////////////////////////////////////////////
#define DEBUG_WINDOWS                          (getenv("DEBUG") != NULL)
#define DEFAULT_MODE_DEBUG_ARGS                false
#define DEBUG_PID_ENV                          false
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define MINIMUM_PNG_FILE_SIZE                  67
#define MINIMUM_PNG_WIDTH                      10
#define MINIMUM_PNG_HEIGHT                     10
///////////////////////////////////////////////////////////////////////////////
#define DEFAULT_VERBOSE                        false
#define DEFAULT_MODE                           "list"
#define DEFAULT_WINDOW_ID                      0
#define DEFAULT_MAX_RECORD_FRAMES              100
#define DEFAULT_MAX_RECORD_DURATION_SECONDS    30
#define DEFAULT_FRAMES_PER_SECOND              3
#define DEFAULT_MODE_LIST                      false
#define DEFAULT_MODE_CAPTURE                   false
#define DEFAULT_RESIZE_TYPE                    RESIZE_BY_NONE
#define DEFAULT_RESIZE_VALUE                   1
#define DEFAULT_APPLICATION_NAME_GLOB          "*"
//////////////////////////////////////////////////////////////////////////////////
#define STR_ENDS_WITH(S, E)    (strcmp(S + strlen(S) - (sizeof(E) - 1), E) == 0)
#define RECORDED_FRAMES_QTY                    vector_size(capture_config->recorded_frames_v)
//////////////////////////////////////////////////////////////////////////////////
#endif
