////////////////////////////////////////////
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
////////////////////////////////////////////
#include "space-test/space-test.h"
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
#include "space/space.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"
#include "process/process.h"

////////////////////////////////////////////
TEST t_space_set_test(){
  set_space_by_index(1);
  PASS();
}

TEST t_space_test(){
  int space_cnt = total_spaces() + 1, rows, cols, window_count, nonmin_window_count;

  CoreDockGetWorkspacesCount(&rows, &cols);
  log_debug("get_front_window_pid:%d", get_front_window_pid());
  log_debug("get_space_id:%d", get_space_id());
  log_debug("is_full_screen:%d", is_full_screen());
  log_debug("display_spaces: %d", space_cnt);
  log_debug("total_spaces:%d", space_cnt);
  log_debug("get_space_via_keywin:%d", get_space_via_keywin());
  log_debug("CoreDockGetWorkspacesCount rows:%d|cols:%d", rows, cols);
  for (int i = 0; i < space_cnt; i++) {
    int      space_minimized_window_qty;
    space_minimized_window_list(i, &space_minimized_window_qty);
    uint32_t display_id             = space_display_id(i);
    uint32_t *window_list           = space_window_list(i + 1, &window_count, true);
    if (window_count == 0) {
      continue;
    }
    CGRect   db                  = display_bounds(display_id);
    space_window_list(i + 1, &nonmin_window_count, false);
    int      qty_min             = window_count - nonmin_window_count;
    log_debug("    Space #%d/%d> %d minimized windows|display #%d|%s|%fx%f|",
              i + 1,
              space_cnt,
              space_minimized_window_qty,
              display_id, cstring_from_CFString(display_uuid(display_id)), db.size.width, db.size.height);
    log_debug("               |user:%d|fullscreen:%d|system:%d|visible:%d|",
              space_is_user(i),
              space_is_fullscreen(i),
              space_is_system(i),
              space_is_visible(i)

              );
    log_debug("               windows:  %d (%d minimized)", window_count, qty_min);
    for (int w = 0; w < window_count && w < 99; w++) {
      struct window_t *W     = get_window_id(window_list[w]);

      log_debug("                   Window #%d/%d>", w + 1, window_count);
      log_debug("                           [window id %" PRIu32 "] ", (window_list[w]));
      log_debug("                                    |app:%s|pid:%d|ismin:%s|layer:%d|spaceid:%d|size:%dx%d|display:%d|                ",
                W->app_name,
                W->pid,
                W->is_minimized?"Yes":"No",
                W->layer,
                W->space_id,
                W->width, W->height,
                W->display_id
                );
      log_debug("                                    |psn:%d|ppid:%d|rtime:%d|nice:%d|focused:%s|visible:%s|childpids:%lu|",
                W->psn.highLongOfPSN + W->psn.lowLongOfPSN,
                W->pid_info.kp_eproc.e_ppid, W->pid_info.kp_proc.p_rtime.tv_usec,
                W->pid_info.kp_proc.p_nice,
                W->is_focused?"Yes":"No",
                W->is_visible?"Yes":"No",
                vector_size(W->child_pids_v)
                );
    }
  }
  PASS();
} /* t_space_test */

SUITE(s_space_test) {
  RUN_TEST(t_space_test);
  if (isatty(STDOUT_FILENO)) {
    RUN_TEST(t_space_set_test);
  }
}

GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_space_test);
  GREATEST_MAIN_END();
}
