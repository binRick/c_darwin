#pragma once
#ifndef WINDOW_INFO_C
#define WINDOW_INFO_C
////////////////////////////////////////////
#include "window-info/window-info.h"
////////////////////////////////////////////
#include "active-app/active-app.h"
#include "ansi-codes/ansi-codes.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "frameworks/frameworks.h"
#include "libfort/lib/fort.h"
#include "log/log.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "process-utils/process-utils.h"
#include "process/process.h"
#include "space-utils/space-utils.h"
#include "string-utils/string-utils.h"
#include "submodules/log.h/log.h"
#include "system-utils/system-utils.h"
#include "table-utils/table-utils.h"
#include "timestamp/timestamp.h"
#include "window-info/window-info.h"
////////////////////////////////////////////
static bool WINDOW_INFO_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__window_info(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_window_info") != NULL) {
    log_debug("Enabling window-info Debug Mode");
    WINDOW_INFO_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
typedef int (^window_info_sort_block)(WINDOW_INFO_SORT_PARAMS);
struct window_info_sort_t {
  const char                *key, *name, *direction;
  window_info_sort_block    block;
  window_info_sort_function function;
};
struct window_info_sort_t window_info_sorts[];
#define REGISTER_WINDOW_INFO_SORT_FUNCTION(KEY, DIRECTION, TYPE) \
  int window_info_sort_ ## KEY ## _ ## DIRECTION(WINDOW_INFO_SORT_PARAMS){ return(window_info_sorts[TYPE].block(w0, w1)); }
REGISTER_WINDOW_INFO_SORT_FUNCTION(pid, asc, WINDOW_INFO_SORT_PID_ASC);
REGISTER_WINDOW_INFO_SORT_FUNCTION(pid, desc, WINDOW_INFO_SORT_PID_DESC);
REGISTER_WINDOW_INFO_SORT_FUNCTION(window_id, asc, WINDOW_INFO_SORT_WINDOW_ID_ASC);
REGISTER_WINDOW_INFO_SORT_FUNCTION(window_id, desc, WINDOW_INFO_SORT_WINDOW_ID_DESC);
struct window_info_sort_t window_info_sorts[] = {
  [WINDOW_INFO_SORT_PID_ASC] =        { .direction = "asc",                    .key  = "pid", .name           = "PID Ascending",
                                        .function  = window_info_sort_pid_asc,
                                        .block     = ^ int (WINDOW_INFO_SORT_PARAMS){
                                          return((w0->pid > w1->pid) ? 1 : (w0->pid < w1->pid) ? -1 : 0);
                                        }, },
  [WINDOW_INFO_SORT_PID_DESC] =       { .direction = "desc",                    .key = "pid", .name           = "PID Descending",
                                        .function  = window_info_sort_pid_desc,
                                        .block     = ^ int (WINDOW_INFO_SORT_PARAMS){
                                          return((w0->pid < w1->pid) ? 1 : (w0->pid > w1->pid) ? -1 : 0);
                                        }, },
  [WINDOW_INFO_SORT_WINDOW_ID_ASC] =  { .direction = "asc",                          .key  = "window-id-asc", .name = "Window ID Ascending",
                                        .function  = window_info_sort_window_id_asc,
                                        .block     = ^ int (WINDOW_INFO_SORT_PARAMS){
                                          return((w0->window_id > w1->window_id) ? 1 : (w0->window_id < w1->window_id) ? -1 : 0);
                                        }, },
  [WINDOW_INFO_SORT_WINDOW_ID_DESC] = { .direction = "desc",                          .key = "window-id", .name     = "Window ID Descending",
                                        .function  = window_info_sort_window_id_desc,
                                        .block     = ^ int (WINDOW_INFO_SORT_PARAMS){
                                          return((w0->window_id < w1->window_id) ? 1 : (w0->window_id > w1->window_id) ? -1 : 0);
                                        }, },
};

window_info_sort_function get_window_info_sort_function_from_key(const char *name, const char *direction){
  for (size_t i = 0; i < WINDOW_INFO_SORT_TYPES_QTY; i++) {
    if (strcmp(name, window_info_sorts[i].key) == 0 && strcmp(direction, window_info_sorts[i].direction) == 0) {
      return((window_info_sort_function)(window_info_sorts[i].function));
    }
  }
  return((window_info_sort_function)(window_info_sorts[0].function));
}
////////////////////////////////////////////
#endif
