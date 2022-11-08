#pragma once
#ifndef TP_C
#define TP_C
#define LOCAL_DEBUG_MODE    TP_DEBUG_MODE
#include "clamp/clamp.h"
#include "string-utils/string-utils.h"
#include "termpaint.h"
#include "termpaint_image.h"
#include "termpaintx.h"
#include "termpaintx_ttyrescue.h"
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <locale.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>

////////////////////////////////////////////
#include "tp/tp.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
enum cell_t { DEAD,
              ALIVE };
typedef struct board_t_ {
  int  width;
  int  height;
  char *cells;
} board_t;
board_t board;
typedef struct CURSOR_PROFILE {
  int  x;
  int  y;
  bool visible, blink;
  int  style;
} cursor_profile_t;
typedef struct event_ {
  int           type;
  int           modifier;
  const char    *string;
  struct event_ *next;
} event;
static struct {
  bool                          quit;
  char                          *terminal_name;
  termpaint_surface             *surface;
  termpaint_attr                *attr_ui, *attr_sample, *attr_bottom_msg;
  termpaint_integration         *integration;
  termpaint_terminal            *terminal;
  event                         *event_current;
  struct winsize                *winsz;
  struct { int height, width; } surface_size;
} _tp = { 0 }, *tp = &_tp;

static void tp_render_ui_lines(termpaint_attr *attr_ui, termpaint_attr *attr_sample),
tp_render_ui(termpaint_attr * attr_ui, termpaint_attr * attr_sample),
tp_render_ui_line(termpaint_attr * attr_ui, termpaint_attr * attr_sample, int line, char const *name, int style),
__tp_timestamp(void),
__tp_main(void),
__tp_init(void)
;
static termpaint_attr *__tp_attr(char *fg, char *bg, int style);

void __tp_cleanup(void) {
  termpaint_terminal_free_with_restore(tp->terminal);
  while (tp->event_current) {
    free((void *)tp->event_current->string);
    event *next = tp->event_current->next;
    free(tp->event_current);
    tp->event_current = next;
  }
  printf(
    AC_RESTORE_PALETTE AC_SHOW_CURSOR
    );
}

static void tp_render_ui(termpaint_attr *attr_ui, termpaint_attr *attr_sample){
  termpaint_surface_clear_with_attr(tp->surface, tp->attr_ui);
  termpaint_surface_write_with_attr(tp->surface, 1, 1, "Terminal UI", tp->attr_ui);
  tp_render_ui_lines(tp->attr_ui, tp->attr_sample);
  termpaint_surface_write_with_attr(tp->surface, 2, 20, "q: Quit", tp->attr_ui);
}

static event *__tp_poll_event(void) {
  while (!tp->event_current->next)
    if (!termpaintx_full_integration_do_iteration(tp->integration)) {
      log_error("err");
      exit(1);
    }
  if (tp->event_current->string)
    free((void *)tp->event_current->string);
  event *next = tp->event_current->next;
  if (tp->event_current)
    free(tp->event_current);
  tp->event_current = next;
  return(next);
}
#define TP_COLOR_ZINC      "146,137,138"
#define TP_COLOR_ZOMBIE    "107,192,38"
#define TP_COLOR_YOSHI     "85,170,0"
#define TP_COLOR_BLACK     "0,0,0"
#define TP_COLOR_WHITE     "255,255,255"
#define TP_COLOR_RED       "201,11,66"
#define TP_PALETTE         "\033]4;0;#000000\033\x5c\033]4;8;#555555\033\x5c\033]4;1;#cc5555\033\x5c\033]4;9;#ff5555\033\x5c\033]4;2;#55cc55\033\x5c\033]4;10;#55ff55\033\x5c\033]4;3;#cdcd55\033\x5c\033]4;11;#ffff55\033\x5c\033]4;4;#5455cb\033\x5c\033]4;12;#5555ff\033\x5c\033]4;5;#cc55cc\033\x5c\033]4;13;#ff55ff\033\x5c\033]4;6;#7acaca\033\x5c\033]4;14;#55ffff\033\x5c\033]4;7;#cccccc\033\x5c\033]4;15;#ffffff\033\x5c\033]11;#00002A\033\x5c\033]10;#b3b3b3\033\x5c\033]19;#000000\033\x5c\033]17;#4c52f8\033\x5c\033]12;#fff1f3\033\x5c\0331C1917\033\x5c\033[?12h\033\x5c\033]12;#fff011\033\x5c\033[5 q\033\x5c\033[?25h\033\x5c\033[21D\033[?1049l\033[?25h"
#define TP_STYLE_INFO \
  __tp_attr(TP_COLOR_ZINC, TP_COLOR_BLACK, TERMPAINT_STYLE_ITALIC)
#define TP_STYLE_ERROR \
  __tp_attr(TP_COLOR_RED, TP_COLOR_BLACK, TERMPAINT_STYLE_BLINK | TERMPAINT_STYLE_BOLD)
#define TP_STYLE_YOSHI \
  __tp_attr(TP_COLOR_YOSHI, TP_COLOR_BLACK, 0)
#define TP_LINE(X, Y, TEXT, STYLE) \
  termpaint_surface_write_with_attr(tp->surface, X, Y, TEXT, STYLE);
static void tp_show_lines(char **lines, int len, int x, int y, termpaint_attr *attr, int width);

static void tp_render_ui_lines(termpaint_attr *attr_ui, termpaint_attr *attr_sample){
  tp_render_ui_line(attr_ui, attr_sample, 3, "No Style:", 0);
  tp_render_ui_line(attr_ui, attr_sample, 4, "Bold:", TERMPAINT_STYLE_BOLD);
  tp_render_ui_line(attr_ui, attr_sample, 5, "Italic:", TERMPAINT_STYLE_ITALIC);
  tp_render_ui_line(attr_ui, attr_sample, 6, "Blinking:", TERMPAINT_STYLE_BLINK);
  tp_render_ui_line(attr_ui, attr_sample, 7, "Underline:", TERMPAINT_STYLE_UNDERLINE);
  tp_render_ui_line(attr_ui, attr_sample, 8, "Strikeout:", TERMPAINT_STYLE_STRIKE);
  tp_render_ui_line(attr_ui, attr_sample, 9, "Inverse:", TERMPAINT_STYLE_INVERSE);
  tp_render_ui_line(attr_ui, attr_sample, 11, "Overline:", TERMPAINT_STYLE_OVERLINE);
  tp_render_ui_line(attr_ui, attr_sample, 12, "Dbl under:", TERMPAINT_STYLE_UNDERLINE_DBL);
  tp_render_ui_line(attr_ui, attr_sample, 13, "curly:", TERMPAINT_STYLE_UNDERLINE_CURLY);
  termpaint_attr *attr_url = termpaint_attr_clone(attr_sample);
  termpaint_attr_set_patch(attr_url, true, "\e]8;;http://example.com\a", "\e]8;;\a");
  tp_render_ui_line(attr_ui, attr_url, 14, "url:", 0);

  TP_LINE(25, 11, "Yoshi", TP_STYLE_YOSHI);
  TP_LINE(25, 12, "some error", TP_STYLE_ERROR);
  TP_LINE(25, 13, "some info", TP_STYLE_INFO);
  __tp_timestamp();
}

static void tp_show_lines(char **lines, int len, int x, int y, termpaint_attr *attr, int width){
  for (int i = 0; i < len; i++)
    TP_LINE(x, y + i, pad_string_right(lines[i], width, ' '), attr);
}

static termpaint_attr *__tp_attr(char *fg, char *bg, int style){
  struct StringFNStrings s[2]  = { stringfn_split(fg, ','), stringfn_split(bg, ',') };
  termpaint_attr         *attr = termpaint_attr_new(
    TERMPAINT_RGB_COLOR(atoi(s[0].strings[0]), atoi(s[0].strings[1]), atoi(s[0].strings[2])),
    TERMPAINT_RGB_COLOR(atoi(s[1].strings[0]), atoi(s[1].strings[1]), atoi(s[1].strings[2]))
    );

  termpaint_attr_set_style(attr, style);
  stringfn_release_strings_struct(s[0]);
  stringfn_release_strings_struct(s[1]);
  return(attr);
}

static void tp_render_ui_line(termpaint_attr *attr_ui, termpaint_attr *attr_sample, int line, char const *name, int style) {
  termpaint_surface_write_with_attr(tp->surface, 0, line, name, tp->attr_ui);
  termpaint_attr_reset_style(attr_sample);
  termpaint_attr_set_style(attr_sample, style);
  termpaint_surface_write_with_attr(tp->surface, 11, line, "Sample", tp->attr_sample);
}

void __tp_sig_handler(int sig){
  switch (sig) {
  case SIGWINCH:
    ioctl(0, TIOCGWINSZ, tp->winsz);
    int was_width  = tp->surface_size.width,
        was_height = tp->surface_size.height;
    tp->surface_size.height = (int)tp->winsz->ws_row;
    tp->surface_size.width  = (int)tp->winsz->ws_col;
    termpaint_surface_resize(tp->surface, tp->surface_size.width, tp->surface_size.height);
    break;
  }
}

static void __tp_event_callback(void *userdata, termpaint_event *tp_event) {
  event *my_event = NULL;

  my_event           = malloc(sizeof(event));
  my_event->type     = tp_event->type;
  my_event->modifier = tp_event->c.modifier;
  switch (tp_event->type) {
  case TERMPAINT_EV_KEY:
    my_event->string = strdup(tp_event->key.atom);
    break;
  case TERMPAINT_EV_CHAR:
    my_event->string = strndup(tp_event->c.string, tp_event->c.length);
    break;
  }
  my_event->next = NULL;

  if (my_event) {
    event *prev = tp->event_current;
    while (prev->next)
      prev = prev->next;
    prev->next = my_event;
  }
}

static void __tp_logging_func(__attribute__((unused)) termpaint_integration *integration, const char *data, int length){
  fprintf(stderr, AC_RESETALL AC_GREEN "[logging_func] len:%d|data:'%s'\n" AC_RESETALL "\n", length, data
          );
}

static void __tp_timestamp(void){
  char *_s[10];

  asprintf(&_s[0], "%lld", timestamp());
  asprintf(&_s[1], "%s", tp->terminal_name);
  char                   *__s  = stringfn_join(_s, ",", 0, 2);
  struct StringFNStrings s     = stringfn_split(__s, ',');
  int                    width = clamp(tp->surface_size.width / 2, 10, tp->surface_size.width);

  tp_show_lines(
    s.strings,
    s.count,
    clamp(tp->surface_size.width - width, 5, tp->surface_size.width),
    0,
    TP_STYLE_INFO,
    width
    );
  stringfn_release_strings_struct(s);
}

static void __tp_loop(void){
  while (!tp->quit) {
    event *evt = __tp_poll_event();
    switch (evt->type) {
    case TERMPAINT_EV_CHAR:
      if (stringfn_equal(evt->string, "q"))
        tp->quit = true;
      if (stringfn_equal(evt->string, "t"))
        __tp_timestamp();
      break;
    }
    if (evt->type == TERMPAINT_EV_CHAR || evt->type == TERMPAINT_EV_KEY) {
      char buff[100];
      snprintf(buff, 100, "Last Key: %-20.20s Key Modifier: %d", evt->string, evt->modifier);
//      termpaint_surface_write_with_attr(tp->surface, 0, 0, buff, tp->attr_ui);
    }
    termpaint_terminal_flush(tp->terminal, true);
  }
}

static void __tp_init(void){
  fprintf(stdout,
          TP_PALETTE
          );
  tp->event_current         = malloc(sizeof(event));
  tp->winsz                 = malloc(sizeof(struct winsize));
  tp->event_current->next   = NULL;
  tp->event_current->string = NULL;
  tp->attr_ui               = termpaint_attr_new(TERMPAINT_DEFAULT_COLOR, TERMPAINT_DEFAULT_COLOR);
  tp->attr_sample           = termpaint_attr_new(TERMPAINT_DEFAULT_COLOR, TERMPAINT_DEFAULT_COLOR);
  tp->attr_bottom_msg       = termpaint_attr_new(TERMPAINT_DEFAULT_COLOR, TERMPAINT_DEFAULT_COLOR);
  tp->integration           = termpaintx_full_integration_setup_terminal_fullscreen("+kbdsigint +kbdsigtstp", __tp_event_callback, NULL, &tp->terminal);
  tp->surface               = termpaint_terminal_get_surface(tp->terminal);
  termpaint_terminal_set_mouse_mode(tp->terminal, TERMPAINT_MOUSE_MODE_CLICKS);
  termpaint_terminal_set_mouse_mode(tp->terminal, TERMPAINT_MOUSE_MODE_DRAG);
  termpaint_terminal_set_mouse_mode(tp->terminal, TERMPAINT_MOUSE_MODE_MOVEMENT);
  termpaint_terminal_request_focus_change_reports(tp->terminal, false);
  ioctl(0, TIOCGWINSZ, tp->winsz);
  tp->surface_size.height = (int)tp->winsz->ws_row;
  tp->surface_size.width  = (int)tp->winsz->ws_col;
  sigaction(SIGWINCH, &(struct sigaction const){
    .sa_handler = __tp_sig_handler,
    .sa_flags   = SA_NODEFER
  }, 0);
  /*
   */
//  bool ok = termpaint_surface_resize_mustcheck(tp->surface, 50, 10);
// termpaint_integration_set_logging_func(tp->integration, __tp_logging_func);
  if (termpaint_terminal_self_reported_name_and_version(tp->terminal))
    tp->terminal_name = termpaint_terminal_self_reported_name_and_version(tp->terminal);
  termpaint_terminal_flush(tp->terminal, true);
}

static void __tp_main(void){
  __tp_init();
  atexit(__tp_cleanup);
  tp_render_ui(tp->attr_ui, tp->attr_sample);
  __tp_loop();
  __tp_cleanup();
}

////////////////////////////////////////////
static bool TP_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__tp(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_tp") != NULL) {
    log_debug("Enabling tp Debug Mode");
    TP_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////

int tp_main(const int argc, const char **argv) {
  //log_debug("TP Main start");
  __tp_main();
  //repaint_all(attr_ui, attr_sample);
//  menu(attr_ui, attr_sample);
//  termpaint_attr_free(attr_sample);
  //attr_sample = NULL;
//  termpaint_attr_free(attr_ui);
//  attr_ui = NULL;
  return(0);
}
#undef LOCAL_DEBUG_MODE

#endif
