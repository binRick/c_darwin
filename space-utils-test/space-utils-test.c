////////////////////////////////////////////
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
////////////////////////////////////////////
#include "space-utils-test/space-utils-test.h"
////////////////////////////////////////////
#include "active-app/active-app.h"
#include "ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "core-utils/core-utils.h"
#include "log.h/log.h"
#include "process/process.h"
#include "space-utils/space-utils.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"

////////////////////////////////////////////
TEST t_space_set_test(){
  set_space_by_index(1);
  PASS();
}

TEST t_space_utils_test(){
  int rows, cols;

  CoreDockGetWorkspacesCount(&rows, &cols);
  struct Vector *display_ids_v = get_display_ids_v();

  log_debug("%lu Displays ::  |focused pid: %d|Spaces: %dx%d|",
            vector_size(display_ids_v),
            get_focused_pid(),
            rows, cols
            );
  for (size_t d = 0; d < vector_size(display_ids_v); d++) {
    uint32_t      display_id   = (uint32_t)(size_t)(vector_get(display_ids_v, d));
    struct Vector *space_ids_v = get_display_id_space_ids_v(display_id);
    log_debug("   Display %lu/%lu #%u  ::  %lu Spaces",
              d + 1, vector_size(display_ids_v),
              display_id, vector_size(space_ids_v));
    size_t on_space_index = 0;
    for (size_t i = 0; i <= vector_size(space_ids_v) + 1; i++) {
      int      space_id = (int)(size_t)vector_get(space_ids_v, i);
      int      minimized_window_count, window_count;
      uint32_t *window_list = space_window_list(i, &window_count, true);
      if (window_count == 0) {
        continue;
      }
      on_space_index++;
      space_minimized_window_list(i, &minimized_window_count);
      log_debug("    Space " AC_GREEN "#%lu/%lu " AC_RESETALL " :: |"
                AC_RESETALL AC_YELLOW "%d" AC_RESETALL
                " windows (" AC_BLUE "%d" AC_RESETALL " minimized)     ",
                on_space_index, vector_size(space_ids_v),
                window_count, minimized_window_count
                );
      for (int w = 0; w < window_count && w < 99; w++) {
        struct window_t *W = get_window_id(window_list[w]);
        if (!W) {
          continue;
        }
        log_debug("       Window " AC_RESETALL AC_MAGENTA "#%d/%d" AC_RESETALL
                  "  [window ID " AC_RESETALL AC_RED "%" PRIu32 AC_RESETALL "]\n"
                  "               |app:" AC_RESETALL AC_GREEN AC_UNDERLINE AC_BOLD "%s" AC_RESETALL "\n"
                  "               |pid:%d|ismin:%s|layer:%d|spaceid:%d|size:%dx%d|display:%d|\n"
                  "               |psn:%d|ppid:%d|rtime:%d|focused:%s|visible:%s|childpids:%lu|",
                  w + 1, window_count,
                  (window_list[w]),
                  W->app_name,
                  W->pid,
                  W->is_minimized?"Yes":"No",
                  W->layer,
                  W->space_id,
                  W->width, W->height,
                  W->display_id,
                  W->psn.highLongOfPSN + W->psn.lowLongOfPSN,
                  W->pid_info.kp_eproc.e_ppid, W->pid_info.kp_proc.p_rtime.tv_usec,
                  W->is_focused?"Yes":"No",
                  W->is_visible?"Yes":"No",
                  vector_size(W->child_pids_v)
                  );
      }
    }
  }
  PASS();
} /* t_space_utils_test */

SUITE(s_space_utils_test) {
  RUN_TEST(t_space_utils_test);
  if (isatty(STDOUT_FILENO)) {
    RUN_TEST(t_space_set_test);
  }
}

GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  GREATEST_MAIN_BEGIN();
  // RUN_SUITE(s_space_utils_test);
  GREATEST_MAIN_END();
}
