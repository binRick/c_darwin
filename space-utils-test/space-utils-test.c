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
#include "process-utils/process-utils.h"
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
      int space_id = (int)(size_t)vector_get(space_ids_v, i);
      int minimized_window_count, window_count;
      on_space_index++;
      get_space_minimized_window_list(i, &minimized_window_count);
      log_debug("    Space " AC_GREEN "#%lu/%lu " AC_RESETALL " :: |"
                AC_RESETALL AC_YELLOW "%d" AC_RESETALL
                " windows (" AC_BLUE "%d" AC_RESETALL " minimized)     ",
                on_space_index, vector_size(space_ids_v),
                window_count, minimized_window_count
                );
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
#include "window-info/window-info.h"
