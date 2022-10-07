#pragma once
#ifndef MONITOR_UTILS_C
#define MONITOR_UTILS_C
#include "monitor/utils/utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "display/utils/utils.h"
#include "c_libmonitors/src/monitors.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
////////////////////////////////////////////
static bool MONITOR_UTILS_DEBUG_MODE = false;
void print_monitors();
static void print_monitor(struct monitor_t *m);
static struct Vector *monitor_utils_iterate_monitors(void);
typedef void (^monitor_parser_b)(struct monitor_t *m, struct libmonitors_monitor *i);
enum font_parser_type_t {
  MONITOR_PARSER_TYPE_NAME = 1,
  MONITOR_PARSER_TYPE_PRIMARY,
  MONITOR_PARSER_TYPE_WIDTH,
  MONITOR_PARSER_TYPE_HEIGHT,
  MONITOR_PARSER_TYPE_REFRESH,
  MONITOR_PARSER_TYPE_ID,
  MONITOR_PARSER_TYPE_UUID,
  MONITOR_PARSER_TYPE_MODES_QTY,
  MONITOR_PARSER_TYPES_QTY,
};
struct monitor_parser_t {
  bool enabled;
  void (^parser)(struct monitor_t *m, struct libmonitors_monitor *i);
};

static struct monitor_parser_t monitor_parsers[MONITOR_PARSER_TYPES_QTY + 1] = {
  [MONITOR_PARSER_TYPE_NAME] =      { .enabled = true,
                                      .parser  = ^ void (struct monitor_t *m, struct libmonitors_monitor *i){
                                        m->name         = strdup(i->name);
                                      }, },
  [MONITOR_PARSER_TYPE_UUID] =      { .enabled = true,
                                      .parser  = ^ void (struct monitor_t *m, struct libmonitors_monitor *i){
                                        m->uuid         = i->uuid;
                                      }, },
  [MONITOR_PARSER_TYPE_ID] =        { .enabled = true,
                                      .parser  = ^ void (struct monitor_t *m, struct libmonitors_monitor *i){
                                        m->id             = i->id;
                                      }, },
  [MONITOR_PARSER_TYPE_PRIMARY] =   { .enabled = true,
                                      .parser  = ^ void (struct monitor_t *m, struct libmonitors_monitor *i){
                                        m->primary   = i->primary;
                                      }, },
  [MONITOR_PARSER_TYPE_WIDTH] =     { .enabled = true,
                                      .parser  = ^ void (struct monitor_t *m, struct libmonitors_monitor *i){
                                        m->width_pixels = i->current_mode->width;
                                        m->width_mm     = i->width;
                                      }, },
  [MONITOR_PARSER_TYPE_HEIGHT] =    { .enabled = true,
                                      .parser  = ^ void (struct monitor_t *m, struct libmonitors_monitor *i){
                                        m->height_pixels = i->current_mode->height;
                                        m->height_mm     = i->height;
                                      }, },
  [MONITOR_PARSER_TYPE_REFRESH] =   { .enabled = true,
                                      .parser  = ^ void (struct monitor_t *m, struct libmonitors_monitor *i){
                                        m->refresh_hz = i->current_mode->refresh;
                                      }, },
  [MONITOR_PARSER_TYPE_MODES_QTY] = { .enabled = true,
                                      .parser  = ^ void (struct monitor_t *m, struct libmonitors_monitor *i){ m->modes_qty = (size_t)i->mode_count; }, },
  [MONITOR_PARSER_TYPES_QTY] =      { 0 },
};

static void print_monitor(struct monitor_t *m){
  fprintf(stdout,
          "\n  Name                  :   " AC_RESETALL AC_GREEN AC_BOLD AC_INVERSE "%s" AC_RESETALL
          "\n  UUID                  :   %s"
          "\n  ID                    :   "AC_RESETALL AC_CYAN AC_UNDERLINE "%d" AC_RESETALL
          "\n  Primary               :   %s" AC_RESETALL
          "\n  Width                 :   %lupx|%lumm"
          "\n  Height                :   %lupx|%lumm"
          "\n  Refresh Rate          :   %dhz"
          "\n  # Modes               :   %lu"
          "%s",
          m->name,
          m->uuid,
          m->id,
          (m->primary == true) ? AC_GREEN "Yes" : AC_RED "No",
          m->width_pixels,
          m->width_mm,
          m->height_pixels,
          m->height_mm,
          m->refresh_hz,
          m->modes_qty,
          "\n"
          );
}

static void parse_monitor(struct monitor_t *m, struct libmonitors_monitor *i){
  for (size_t x = 0; x < MONITOR_PARSER_TYPES_QTY; x++) {
    if (monitor_parsers[x].enabled == true) {
      monitor_parsers[x].parser(m, i);
    }
  }
}
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__monitor_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_MONITOR_UTILS") != NULL) {
    log_debug("Enabling monitor-utils Debug Mode");
    MONITOR_UTILS_DEBUG_MODE = true;
  }
}

struct Vector *get_monitors_v(void){
  return(monitor_utils_iterate_monitors());
}

void print_monitors(){
  struct Vector *_monitors_v = get_monitors_v();

  fprintf(stdout,
          "\t" AC_CYAN AC_UNDERLINE "%lu" AC_RESETALL " " AC_YELLOW AC_UNDERLINE "Monitors" AC_RESETALL
          "%s",
          vector_size(_monitors_v),
          ""
          );
  for (size_t i = 0; i < vector_size(_monitors_v); i++) {
    print_monitor((struct monitor_t *)vector_get(_monitors_v, i));
  }
  vector_release(_monitors_v);
}
static struct Vector *monitor_utils_iterate_monitors(void){
  struct Vector              *_monitors_v = vector_new();
  int                        count;
  struct libmonitors_monitor **monitors;

  if (libmonitors_init()) {
    if (libmonitors_detect(&count, &monitors)) {
      for (int i = 0; i < count; ++i) {
        struct monitor_t *m = calloc(1, sizeof(struct monitor_t));
        parse_monitor(m, monitors[i]);
        vector_push(_monitors_v, (void *)m);
      }
      libmonitors_free_monitors(count, monitors);
    }
    libmonitors_deinit();
  }
  return(_monitors_v);
}
#endif
