#pragma once
#ifndef WINDOW_C
#define WINDOW_C
#include "c_ansi/async/async.h"
#include "capture/capture.h"
#include "capture/save/save.h"
#include "capture/type/type.h"
#include "capture/utils/utils.h"
#include "exec-fzf/exec-fzf.h"
#include "glib.h"
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
#include "process/utils/utils.h"
#include "qoir/src/qoir.h"
#include "window/info/info.h"
#include "window/window.h"
#include "window/window.h"
#include "window_qoir/window_qoir.h"
static bool WINDOW_DEBUG_MODE = false;
MODULE_FUNCTIONS(MODULE_FUNCTION)
#undef MODULE_FUNCTION

struct Vector *qoirs_from_ids(struct Vector *ids);
struct Vector *qoirs_from_ids(struct Vector *ids){
  struct Vector   *v;
  async_worker_cb cb = ^ void *(void *VOID){
    size_t len;
    //   save_cgref_to_qoir_memory(capture_type_capture(CAPTURE_TYPE_WINDOW,(size_t)VOID),&len);
    Dbg(len, %lu);
//    save_cgref_to_qoir_memory(capture_type_capture(CAPTURE_TYPE_WINDOW,(size_t)(VOID)));
    void *r = NULL;
    return((void *)r);
  };

  v = require(async)->each->vec(10, ids, cb);
  return(v);
}

int get_window_id_from_name(char *NAME){
  qoirs_from_ids(require(window)->ids);
  int id = -1;
  log_info("getting id from %s", NAME);
  return(id);
}

struct Vector *get_qoirs(void){
  struct Vector *v = vector_new();

  return(v);
}

int window_module_init(module(window) *exports) {
  clib_module_init(window, exports);
  exports->mode = WINDOW_NONE;
  return(0);
}

void window_module_deinit(module(window) *exports) {
  clib_module_deinit(window);
}

char *select_window_name(void){
  return(exec_fzf_select_from_items_v(require(window)->names, "Window"));
}

//char *get_window_preview(module(window) *exports) {
//  char *s;asprintf(&s,"xxxxxxxx");
//  return(s);
//}

static void __attribute__((constructor)) __constructor__window(void);

static void __attribute__((constructor)) __constructor__window(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_window") != NULL) {
    log_debug("Enabling window Debug Mode");
    WINDOW_DEBUG_MODE = true;
  }
}
#endif
#undef MODULE_FUNCTIONS
