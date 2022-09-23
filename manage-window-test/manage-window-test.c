////////////////////////////////////////////

////////////////////////////////////////////
#include "manage-window-test/manage-window-test.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app-utils/app-utils.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core-utils/core-utils.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"
#define MIN_Y_VALUE             25
#define POLL_WINDOW_STATE_MS    5
static char *msg;
static struct cfg_t {
  size_t        WINDOWID;
  int           X, Y;
  int           WIDTH, HEIGHT;
  int           SPACEID;
  unsigned long max_wait_ms;
} *cfg = &(struct cfg_t){
  .WINDOWID    = 0, .X = 0, .Y = 0, .WIDTH = 500, .HEIGHT = 600,
  .max_wait_ms = 500, .SPACEID = 0,
};
static void setup_cfg();

TEST t_authorized_tests(void){
  authorized_test_t *authorized_test_results = execute_authorization_tests();

  for (size_t i = 0; i < AUTHORIZED_TEST_TYPE_IDS_QTY; i++) {
    if (i != AUTHORIZED_ACCESSIBILITY) {
      continue;
    }
    log_debug("Auth test #%lu> %s => %s :: %s", i, authorized_test_results[i].name, authorized_test_results[i].authorized == true ? "OK" : "Failed", milliseconds_to_string(authorized_test_results[i].dur_ms));
    ASSERT_EQ(authorized_test_results[i].authorized, true);
  }
}

////////////////////////////////////////////
TEST t_manage_window_move_space_id(){
  struct window_info_t *w = get_window_id_info(cfg->WINDOWID);

  if (w->window_id != cfg->WINDOWID) {
    FAIL();
  }

  log_debug("Moving Window ID %lu from spaceid %d to %d",
            w->window_id,
            w->space_id,
            cfg->SPACEID
            );
  window_send_to_space(w, cfg->SPACEID);
  struct window_info_t *w2 = get_window_id_info(cfg->WINDOWID);

  ASSERT_EQm("Failed to move window to new spaceid", w2->space_id, cfg->SPACEID);
  asprintf(&msg, "Moved window #%lu from space id %d to space id %d",
           w->window_id,
           w->space_id,
           w2->space_id
           );

  PASSm(msg);
}

TEST t_manage_window_move(){
  struct window_info_t *w = get_window_id_info(cfg->WINDOWID);

  ASSERT_NEQm("Failed to get focused window", w, NULL);
  bool ok = move_window(w, cfg->X, cfg->Y);

  log_debug("Managed Window ID %lu %d", w->window_id, ok);

  ASSERT_EQm("Failed to move window", ok, true);

  struct window_info_t *w1          = get_window_id_info(w->window_id);
  size_t               waited_qty   = 0;
  unsigned long        started_wait = timestamp();

  unsigned long        dur_ms = timestamp() - started_wait;

  PASSm(msg);
} /* t_manage_window_move */

TEST t_manage_window_resize(){
  struct window_info_t *w = get_window_id_info(cfg->WINDOWID);

  ASSERT_NEQm("Failed to get focused window", w, NULL);

  unsigned long started_wait = timestamp();
  bool          ok           = resize_window(w, cfg->WIDTH, cfg->HEIGHT);

  unsigned long dur_ms = timestamp() - started_wait;

  ASSERT_EQm("Failed to resize window", ok, true);
  struct window_info_t *w1 = get_window_id_info(w->window_id);

  asprintf(&msg, "Resized Window ID %lu to %dx%d in %s", w->window_id, cfg->WIDTH, cfg->HEIGHT, milliseconds_to_string(dur_ms));
  PASSm(msg);
}

SUITE(s_manage_window_resize) {
  RUN_TEST(t_manage_window_resize);
}
SUITE(s_manage_window_move) {
  RUN_TEST(t_manage_window_move);
}
SUITE(s_manage_window_move_space_id) {
  RUN_TEST(t_manage_window_move_space_id);
}

GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  setup_cfg();
  GREATEST_MAIN_BEGIN();
  RUN_TEST(t_authorized_tests);
  RUN_SUITE(s_manage_window_move);
  RUN_SUITE(s_manage_window_resize);
  if (cfg->SPACEID > 0) {
    RUN_SUITE(s_manage_window_move_space_id);
  }
  GREATEST_MAIN_END();
}

static void setup_cfg(){
  if (getenv("SPACEID") != NULL) {
    cfg->SPACEID = atoi(getenv("SPACEID"));
  }
  if (getenv("WINDOWID") != NULL) {
    cfg->WINDOWID = atoi(getenv("WINDOWID"));
  }
  if (getenv("WIDTH") != NULL) {
    cfg->WIDTH = atoi(getenv("WIDTH"));
  }
  if (getenv("HEIGHT") != NULL) {
    cfg->HEIGHT = atoi(getenv("HEIGHT"));
  }
  if (getenv("X") != NULL) {
    cfg->X = atoi(getenv("X"));
  }
  if (getenv("Y") != NULL) {
    cfg->Y = atoi(getenv("Y"));
  }
  if (cfg->WINDOWID < 1) {
    cfg->WINDOWID = get_focused_window_id();
  }
  if (cfg->Y < MIN_Y_VALUE) {
    log_info(AC_YELLOW "Adjusting Y to minimum value " AC_MAGENTA "%d" AC_RESETALL, MIN_Y_VALUE);
    cfg->Y = MIN_Y_VALUE;
  }
  log_info("Using Window ID %lu | move %dx%d | resize %dx%d | Space ID %d",
           cfg->WINDOWID, cfg->X, cfg->Y, cfg->WIDTH, cfg->HEIGHT,
           cfg->SPACEID
           );
}
