#pragma once
#ifndef DLS_CAPTURE_COMMANDS_H
#define DLS_CAPTURE_COMMANDS_H
#include "capture/type/type.h"
#include "core/core.h"
#include "dls/meta.h"
#include "layout/utils/utils.h"
typedef void (^capture_command_type_b)(enum capture_type_id_t type);
typedef void (^capture_command_b)(capture_command_type_b);
static capture_command_b capture_commands[] = {
  [CAPTURE_TYPE_WINDOW] = ^ void (capture_command_type_b cb){
    cb(CAPTURE_TYPE_WINDOW);
    return;
  },
};
#define CommandCapture_IFACE                  \
  vfuncDefault(int, id, CommandCapture cc)    \
  vfuncDefault(int, index, CommandCapture cc) \
  vfunc(bool, capture, const VSelf)
interface(CommandCapture);
struct CaptureRequest {
  int                    id, width, height, index;
  enum capture_type_id_t type;
};
struct CaptureResult {
  int           len;
  unsigned char *pixels;
  unsigned long dur;
};
typedef struct {
  struct CaptureResult  res;
  struct CaptureRequest req;
} CaptureSpace;
typedef struct {
  struct CaptureRequest req;
  struct CaptureResult  res;
} CaptureDisplay;
typedef struct {
  struct CaptureRequest req;
  struct CaptureResult  res;
} CaptureWindow;
bool CaptureWindow_capture(const VSelf);
bool CaptureDisplay_capture(const VSelf);
bool CaptureSpace_capture(const VSelf);
declImplExtern(CommandCapture, CaptureWindow);
declImplExtern(CommandCapture, CaptureSpace);
declImplExtern(CommandCapture, CaptureDisplay);

#define COMMON_OPTIONS_CAPTURE_TYPE                           \
  common_options_b[COMMON_OPTION_CAPTURE_WINDOW_MODE](args),  \
  common_options_b[COMMON_OPTION_CAPTURE_SPACE_MODE](args),   \
  common_options_b[COMMON_OPTION_CAPTURE_DISPLAY_MODE](args), \
  COMMON_OPTIONS_ID
#define COMMON_OPTIONS_CAPTURE_RESULT_FILE_OPTIONS   \
  common_options_b[COMMON_OPTION_OUTPUT_FILE](args), \
  common_options_b[COMMON_OPTION_DISPLAY_OUTPUT_FILE](args),
#define COMMON_OPTIONS_CAPTURE_RESULT_OPTIONS                               \
  common_options_b[COMMON_OPTION_PURGE_WRITE_DIRECTORY_BEFORE_WRITE](args), \
  common_options_b[COMMON_OPTION_WRITE_DIRECTORY](args),                    \
  COMMON_OPTIONS_CAPTURE_RESULT_FILE_OPTIONS                                \
  COMMON_OPTIONS_CAPTURE_RESULT_IMAGE_OPTIONS
#define COMMON_OPTIONS_CAPTURE_SIZE_FILTERS             \
  common_options_b[COMMON_OPTION_HEIGHT_GREATER](args), \
  common_options_b[COMMON_OPTION_HEIGHT_LESS](args),    \
  common_options_b[COMMON_OPTION_WIDTH_GREATER](args),  \
  common_options_b[COMMON_OPTION_WIDTH_LESS](args),
#define COMMON_OPTIONS_CAPTURE_RESULT_FILTERS                \
  common_options_b[COMMON_OPTION_CURRENT_SPACE](args),       \
  common_options_b[COMMON_OPTION_NOT_CURRENT_SPACE](args),   \
  common_options_b[COMMON_OPTION_CURRENT_DISPLAY](args),     \
  common_options_b[COMMON_OPTION_NOT_CURRENT_DISPLAY](args), \
  common_options_b[COMMON_OPTION_SPACE_ID](args),            \
  common_options_b[COMMON_OPTION_CURRENT_ID_MODE](args),     \
  common_options_b[COMMON_OPTION_PID](args),                 \
  common_options_b[COMMON_OPTION_APPLICATION_NAME](args),    \
  common_options_b[COMMON_OPTION_DISPLAY_ID](args),          \
  common_options_b[COMMON_OPTION_PID](args),                 \
  common_options_b[COMMON_OPTION_APPLICATION_NAME](args),    \
  common_options_b[COMMON_OPTION_NOT_MINIMIZED](args),       \
  common_options_b[COMMON_OPTION_MINIMIZED](args),           \
  COMMON_OPTIONS_CAPTURE_SIZE_FILTERS
#define COMMON_OPTIONS_CAPTURE_RESULT_IMAGE_OPTIONS \
  common_options_b[COMMON_OPTION_WRITE_IMAGES_MODE](args),
#define COMMON_OPTIONS_CAPTURE_COMMON                            \
  COMMON_OPTIONS_BASE                                            \
  COMMON_OPTIONS_UI                                              \
  COMMON_OPTIONS_CAPTURE_RESULT_OPTIONS                          \
  COMMON_OPTIONS_CAPTURE_OPTIONS                                 \
  COMMON_OPTIONS_ID                                              \
  COMMON_OPTIONS_SIZE                                            \
  common_options_b[COMMON_OPTION_DISPLAY_SIZE_PERCENTAGE](args), \
  common_options_b[COMMON_OPTION_FULLSCREEN_MODE](args),         \
  COMMON_OPTIONS_DB_SAVE
#define COMMON_OPTIONS_CAPTURE \
  COMMON_OPTIONS_BASE
#define COMMON_OPTIONS_IMAGE_CAPTURE_OPTIONS            \
  common_options_b[COMMON_OPTION_GRAYSCALE_MODE](args), \
  common_options_b[COMMON_OPTION_IMAGE_FORMATS](args),
#define COMMON_OPTIONS_CAPTURE_OPTIONS                   \
  common_options_b[COMMON_OPTION_CURRENT_ID_MODE](args), \
  common_options_b[COMMON_OPTION_COMPRESS](args),        \
  common_options_b[COMMON_OPTION_QUANTIZE_MODE](args),   \
  common_options_b[COMMON_OPTION_CONCURRENCY](args),     \
  COMMON_OPTIONS_LIMIT_OPTIONS                           \
  COMMON_OPTIONS_IMAGE_CAPTURE_OPTIONS
#define COMMON_OPTIONS_CAPTURE_WINDOW \
  COMMON_OPTIONS_CAPTURE_COMMON
#define COMMON_OPTIONS_CAPTURE_SPACE \
  COMMON_OPTIONS_CAPTURE_COMMON
#define COMMON_OPTIONS_CAPTURE_DISPLAY_SIZE_PERCENTAGE \
  COMMON_OPTIONS_CAPTURE_COMMON
#define COMMON_OPTIONS_CAPTURE_DISPLAY \
  COMMON_OPTIONS_CAPTURE_COMMON
#define SUBCOMMANDS_CAPTURE             \
  CREATE_SUBCOMMAND(CAPTURE_WINDOW, ),  \
  CREATE_SUBCOMMAND(CAPTURE_DISPLAY, ), \
  CREATE_SUBCOMMAND(CAPTURE_SPACE, ),
#define ADD_CAPTURE_COMMAND_OPTIONS()                                                                                 \
  CREATE_FLOAT_COMMAND_OPTION(DISPLAY_SIZE_PERCENTAGE, 0, "size", "display size percentage", display_size_percentage) \
  CREATE_BOOLEAN_COMMAND_OPTION(FULLSCREEN_MODE, 0, "full", "Full", fullscreen_mode)

#define ADD_CAPTURE_SUBCOMMANDS() \
  CREATE_SUBCOMMAND(CAPTURE, SUBCOMMANDS_CAPTURE),
#define ADD_CAPTURE_COMMANDS()                                                                                  \
  COMMAND(ICON_CAPTURE, CAPTURE, "capture", COLOR_CAPTURE, "Capture Image", 0)                                  \
  COMMAND(ICON_WINDOW, CAPTURE_WINDOW, "window", COLOR_WINDOW, "Capture Window", *_command_capture_window)      \
  COMMAND(ICON_DISPLAY, CAPTURE_DISPLAY, "display", COLOR_WINDOW, "Capture Display", *_command_capture_display) \
  COMMAND(ICON_SPACE, CAPTURE_SPACE, "space", COLOR_SPACE, "Capture Space", *_command_capture_space)            \

#endif
