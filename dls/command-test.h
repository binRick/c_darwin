#pragma once
#ifndef DLS_TEST_COMMANDS_H
#define DLS_TEST_COMMANDS_H
#include "dls/commands.h"
struct stream_update_t {
  CGRect        rect;
  size_t        buf_len, png_len;
  unsigned char *buf, *png;
  unsigned long ts;
  size_t        width, height, id, seed;
  size_t        start_x, start_y, end_x, end_y;
  size_t        bytes_per_pixel;
  size_t        pixels_qty;
  float         pixels_percent;
  char          *png_file;
  VipsImage     *imgs[10];
  VipsImage     *image;
  VipsImage     *img, *png_img;
  size_t        png_buf_len;
  unsigned char *png_buf;
  VipsPel       *mem;
  ringbuf_t     rb;
  int           bpp, stride;
};
struct stream_setup_t {
  CFRunLoopRef    *loop;
  size_t          delay_ms, width, height, id, monitor_interval_ms, last_monitor_ts;
  bool            ended, debug_mode, verbose_mode;
  pthread_t       threads[5];
  pthread_mutex_t *mutex;
  void            **buffers;
  struct Vector   *heartbeat, *rectangles;
  chan_t          *chan;
  unsigned long   ts;
  char            *png_file;
  unsigned char   *png; size_t png_len;
  VipsImage       *imgs[10];
  VipsImage       *image;
  VipsImage       *img;
};
////////////////////////////////////////////////////////////
#define ADD_DB_COMMAND_PROTOTYPES()
////////////////////////////////////////////////////////////
#define ADD_TEST_COMMAND_PROTOTYPES()       \
  COMMAND_PROTOTYPE(test_test_hash)         \
  COMMAND_PROTOTYPE(test_test_clipboard)    \
  COMMAND_PROTOTYPE(test_test_vector)       \
  COMMAND_PROTOTYPE(test_test_windows)      \
  COMMAND_PROTOTYPE(test_pick)              \
  COMMAND_PROTOTYPE(test_vector)            \
  COMMAND_PROTOTYPE(test_shape)             \
  COMMAND_PROTOTYPE(test_frog)              \
  COMMAND_PROTOTYPE(test_layouts)           \
  COMMAND_PROTOTYPE(test_layout_vertical)   \
  COMMAND_PROTOTYPE(test_layout_horizontal) \
  COMMAND_PROTOTYPE(test_find_window)       \
  COMMAND_PROTOTYPE(test_droid)             \
  COMMAND_PROTOTYPE(test_cap_window)        \
  COMMAND_PROTOTYPE(test_cap_display)       \
  COMMAND_PROTOTYPE(test_stream_window)     \
  COMMAND_PROTOTYPE(test_stream_display)    \
  COMMAND_PROTOTYPE(test_cmd)               \
//////////////////////////////////////////////////
#define ADD_DB_COMMAND_ENUMS()
//////////////////////////////////////////////////
#define ADD_TEST_COMMAND_ENUMS() \
  COMMAND_TEST_WINDOWS,          \
//////////////////////////////////////////////////
#define COMMAND_TEST_ADD_TEST_FUNCTIONS()                                                                         \
  COMMAND_TEST_ADD_TEST_FUNCTION(test, hash, "", "", 2, "Basic Hash Tests")           /*_command_test_hash*/      \
  COMMAND_TEST_ADD_TEST_FUNCTION(test, clipboard, "", "", 2, "Basic Clipboard Tests") /*_command_test_clipboard*/ \
  COMMAND_TEST_ADD_TEST_FUNCTION(test, windows, "", "", 2, "Basic Windows Tests")     /*_command_test_windows*/   \
  COMMAND_TEST_ADD_TEST_FUNCTION(test, vector, "", "", 2, "Basic Vector Tests")       /*_command_test_vector*/    \
///////////////////////////////////////////
#define COMMAND_TEST_ADD_TEST_PROTOTYPES()         \
  COMMAND_TEST_ADD_TEST_PROTOTYPE(test, hash)      \
  COMMAND_TEST_ADD_TEST_PROTOTYPE(test, clipboard) \
  COMMAND_TEST_ADD_TEST_PROTOTYPE(test, windows)   \
  COMMAND_TEST_ADD_TEST_PROTOTYPE(test, vector)    \
///////////////////////////////////////////
#include "dls/dls.h"
int _command_test_terminal(void);
COMMAND_TEST_ADD_TEST_PROTOTYPES()
#undef COMMAND_TEST_ADD_TEST_PROTOTYPES
#define COLOR_CMD            "\x1b[38;2;50;252;50m"
#define COLOR_TERMPAINT      "\x1b[38;2;50;252;50m"
#define COLOR_CSV            "\x1b[38;2;50;252;50m"
#define COLOR_TEST           "\x1b[38;2;243;233;217m"
#define COLOR_CAP            "\x1b[38;2;151;252;50m"
#define COLOR_HASH           "\x1b[38;2;151;50;252m"
#define COLOR_FILE_READER    "\x1b[38;2;151;252;252m"
#define COLOR_FROG           "\x1b[38;2;252;151;50m"
#define COLOR_PICK           "\x1b[38;2;252;151;50m"
#define COLOR_RUN            "\x1b[38;2;252;151;50m"
#define COLOR_SHAPE          "\x1b[38;2;50;252;151m"
#define COLOR_DROID          "\x1b[38;2;151;252;252m"
#define COLOR_STREAM         "\x1b[38;2;151;252;252m"
#define COLOR_VECTOR         "\x1b[38;2;80;100;127m"
#define COLOR_WINDOWS        "\x1b[38;2;80;100;50m"
#define COLOR_CLIPBOARD      "\x1b[38;2;80;100;50m"
#define COLOR_TS      "\x1b[38;2;80;100;50m"
#define ICON_DROID           "👽"
#define ICON_TS           "👽"
#define ICON_CLIPBOARD       "👽"
#define ICON_VECTOR          "👽"
#define ICON_CSV             "👽"
#define ICON_WINDOWS         "🍍"
#define ICON_SHAPE           "💠"
#define ICON_FROG            "🐸"
#define ICON_PICK            "👽"
#define ICON_RUN             "👽"
#define ICON_TEST            "🔔"
#define ICON_CMD             "💻"
#define ICON_TERMPAINT       "💻"
#define ICON_STREAM          "🌊"
#define ICON_CAP             "🎩"
///////////////
#define COMMON_OPTIONS_TERMPAINT
#define COMMON_OPTIONS_DB
#define COMMON_OPTIONS_TEST                           \
  common_options_b[COMMON_OPTION_CLEAR_SCREEN](args), \
///////////////
#define COMMON_OPTIONS_TEST_COMMON                        \
  common_options_b[COMMON_OPTION_VERBOSE_MODE](args),     \
  common_options_b[COMMON_OPTION_DEBUG_MODE](args),       \
  common_options_b[COMMON_OPTION_CLEAR_SCREEN](args),     \
  common_options_b[COMMON_OPTION_LIST_TESTS_MODE](args),  \
  common_options_b[COMMON_OPTION_LIST_SUITES_MODE](args), \
  common_options_b[COMMON_OPTION_ABORT_TESTS_MODE](args),
#define COMMON_OPTIONS_TEST_TS_MS
#define COMMON_OPTIONS_TEST_TS \
  COMMON_OPTIONS_TEST_COMMON
#define COMMON_OPTIONS_TEST_WINDOWS \
  COMMON_OPTIONS_TEST_COMMON
#define COMMON_OPTIONS_TEST_VECTOR \
  COMMON_OPTIONS_TEST_COMMON
#define COMMON_OPTIONS_TEST_FIND_WINDOW \
  COMMON_OPTIONS_TEST_COMMON
#define COMMON_OPTIONS_TEST_LAYOUT_VERTICAL
#define COMMON_OPTIONS_TEST_LAYOUT_HORIZONTAL
#define COMMON_OPTIONS_TEST_LAYOUTS \
  COMMON_OPTIONS_TEST_COMMON
#define COMMON_OPTIONS_TEST_FROG
#define COMMON_OPTIONS_TEST_DROID
#define COMMON_OPTIONS_TEST_FILE_READER
#define COMMON_OPTIONS_TEST_SHAPE
#define COMMON_OPTIONS_TEST_PICK
#define COMMON_OPTIONS_TEST_CLIPBOARD \
  COMMON_OPTIONS_TEST_COMMON
#define COMMON_OPTIONS_TEST_HASH \
  COMMON_OPTIONS_TEST_COMMON
////////////////////////////////////////////////////////////
#define COMMON_OPTIONS_TEST_CAP
#define COMMON_OPTIONS_TEST_CSV
#define COMMON_OPTIONS_TEST_CAP_DISPLAY
#define COMMON_OPTIONS_TEST_CAP_WINDOW
#define COMMON_OPTIONS_TEST_STREAM                        \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args),      \
  common_options_b[COMMON_OPTION_VERBOSE_MODE](args),     \
  common_options_b[COMMON_OPTION_DEBUG_MODE](args),       \
  common_options_b[COMMON_OPTION_CLEAR_SCREEN](args),     \
  common_options_b[COMMON_OPTION_WIDTH](args),            \
  common_options_b[COMMON_OPTION_HEIGHT](args),           \
  common_options_b[COMMON_OPTION_DURATION_SECONDS](args), \
  common_options_b[COMMON_OPTION_ID](args),               \
  common_options_b[COMMON_OPTION_INDEX](args),            \
///////////////
#define COMMON_OPTIONS_TEST_TERMINAL
#define COMMON_OPTIONS_TEST_STREAM_WINDOW            \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args), \
///////////////
#define COMMON_OPTIONS_TEST_TERMPAINT                \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args), \
///////////////
///////////////
#define COMMON_OPTIONS_TEST_STREAM_DISPLAY           \
  common_options_b[COMMON_OPTION_HELP_SUBCMD](args), \
///////////////
#define COMMON_OPTIONS_TEST_CMD
#define COMMON_OPTIONS_TEST_LAYOUT                   \
  common_options_b[COMMON_OPTION_LAYOUT_SIZE](args), \
  common_options_b[COMMON_OPTION_LAYOUT_QTY](args),
///////////////
#define SUBCOMMANDS_TEST_LAYOUT                \
  CREATE_SUBCOMMAND(TEST_LAYOUT_VERTICAL, ),   \
  CREATE_SUBCOMMAND(TEST_LAYOUT_HORIZONTAL, ), \
  CREATE_SUBCOMMAND(TEST_LAYOUTS, ),           \
///////////////
#define SUBCOMMANDS_TEST_TS             \
  CREATE_SUBCOMMAND(TEST_TS_MS, ),
#define SUBCOMMANDS_TEST_STREAM             \
  CREATE_SUBCOMMAND(TEST_STREAM_DISPLAY, ), \
  CREATE_SUBCOMMAND(TEST_STREAM_WINDOW, ),  \
////////////////////////////////
#define SUBCOMMANDS_TEST_CSV
#define SUBCOMMANDS_TEST_CAP             \
  CREATE_SUBCOMMAND(TEST_CAP_DISPLAY, ), \
  CREATE_SUBCOMMAND(TEST_CAP_WINDOW, ),  \
////////////////////////////////
#define SUBCOMMANDS_TEST                                   \
  CREATE_SUBCOMMAND(TEST_CMD, ),                           \
  CREATE_SUBCOMMAND(TEST_CSV, ),                           \
  CREATE_SUBCOMMAND(TEST_PICK, ),                          \
  CREATE_SUBCOMMAND(TEST_HASH, ),                          \
  CREATE_SUBCOMMAND(TEST_CLIPBOARD, ),                     \
  CREATE_SUBCOMMAND(TEST_VECTOR, ),                        \
  CREATE_SUBCOMMAND(TEST_WINDOWS, ),                       \
  CREATE_SUBCOMMAND(TEST_FIND_WINDOW, ),                   \
  CREATE_SUBCOMMAND(TEST_DROID, ),                         \
  CREATE_SUBCOMMAND(TEST_FILE_READER, ),                   \
  CREATE_SUBCOMMAND(TEST_FROG, ),                          \
  CREATE_SUBCOMMAND(TEST_TERMINAL, ),                      \
  CREATE_SUBCOMMAND(TEST_SHAPE, ),                         \
  CREATE_SUBCOMMAND(TEST_LAYOUT, SUBCOMMANDS_TEST_LAYOUT), \
  CREATE_SUBCOMMAND(TEST_CAP, SUBCOMMANDS_TEST_CAP),       \
  CREATE_SUBCOMMAND(TEST_STREAM, SUBCOMMANDS_TEST_STREAM), \
////////////////
#define ADD_DB_TYPES()
////////////////////////////////////////////////////////////
#define ADD_TEST_SUBCOMMANDS()               \
  CREATE_SUBCOMMAND(TEST, SUBCOMMANDS_TEST), \
////////////////////////////////////////////////////////////
#define ADD_DB_SUBCOMMANDS() \
////////////////////////////////////////////////////////////
#define ADD_DB_COMMANDS() \
////////////////////////////////////////////////////////////
#define ADD_TEST_COMMANDS()                                                                                                            \
  COMMAND(ICON_TEST, TEST, "test", COLOR_LIST, "Test Commands", 0)                                                                     \
  COMMAND(ICON_WINDOW, TEST_STREAM_WINDOW, "window", COLOR_WINDOW, "Test Window Stream", *_command_test_stream_window)                 \
  COMMAND(ICON_LAYOUT, TEST_LAYOUT, "layout", COLOR_LAYOUT, "Test Layout", 0)                                                          \
  COMMAND(ICON_STREAM, TEST_STREAM, "stream", COLOR_STREAM, "Test Stream", 0)                                                          \
  COMMAND(ICON_CAP, TEST_CAP, "cap", COLOR_CAP, "Test Capture", 0)                                                                     \
  COMMAND(ICON_DISPLAY, TEST_STREAM_DISPLAY, "display", COLOR_DISPLAY, "Test Display Stream", *_command_test_stream_display)           \
  COMMAND(ICON_WINDOW, TEST_CAP_WINDOW, "window", COLOR_WINDOW, "Test Window Capture", *_command_test_cap_window)                      \
  COMMAND(ICON_DISPLAY, TEST_CAP_DISPLAY, "display", COLOR_DISPLAY, "Test Display Capture", *_command_test_cap_display)                \
  COMMAND(ICON_HASH, TEST_HASH, "hash", COLOR_HASH, "Test Hash", *_command_test_test_hash)                                             \
  COMMAND(ICON_CLIPBOARD, TEST_CLIPBOARD, "clipboard", COLOR_CLIPBOARD, "Test Clipboard", *_command_test_test_clipboard)               \
  COMMAND(ICON_PICK, TEST_PICK, "pick", COLOR_PICK, "Test Pick", *_command_test_pick)                                                  \
  COMMAND(ICON_VECTOR, TEST_VECTOR, "vector", COLOR_VECTOR, "Test Vector", *_command_test_test_vector)                                 \
  COMMAND(ICON_WINDOWS, TEST_WINDOWS, "windows", COLOR_WINDOWS, "Test Windows", *_command_test_test_windows)                           \
  COMMAND(ICON_SHAPE, TEST_SHAPE, "shape", COLOR_SHAPE, "Test Shape", *_command_test_shape)                                            \
  COMMAND(ICON_DROID, TEST_DROID, "droid", COLOR_DROID, "Test Droid", *_command_test_droid)                                            \
  COMMAND(ICON_FILE, TEST_FILE_READER, "file-reader", COLOR_FILE, "Test File Reader", *_command_test_file_reader)                      \
  COMMAND(ICON_FROG, TEST_FROG, "frog", COLOR_FROG, "Test Frog", *_command_test_frog)                                                  \
  COMMAND(ICON_LAYOUT, TEST_LAYOUTS, "all", COLOR_LAYOUT, "Test Layouts", *_command_test_layouts)                                      \
  COMMAND(ICON_LAYOUT, TEST_LAYOUT_VERTICAL, "vertical", COLOR_LAYOUT, "Test Vertical Layout", *_command_test_layout_vertical)         \
  COMMAND(ICON_TERMINAL, TEST_TERMINAL, "terminal", COLOR_TERMINAL, "Test Terminal", *_command_test_terminal)                          \
  COMMAND(ICON_WINDOW, TEST_FIND_WINDOW, "find", COLOR_WINDOW, "Test Find Window", *_command_test_find_window)                         \
  COMMAND(ICON_LAYOUT, TEST_LAYOUT_HORIZONTAL, "horizontal", COLOR_LAYOUT, "Test Horizontal Layout", *_command_test_layout_horizontal) \
//////////////////////////////////////////////////
#endif
