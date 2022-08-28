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
  int           WINDOWID;
  int           X, Y;
  int           WIDTH, HEIGHT;
  unsigned long max_wait_ms;
} *cfg = &(struct cfg_t){
  .WINDOWID    = 0, .X = 0, .Y = 0, .WIDTH = 500, .HEIGHT = 600,
  .max_wait_ms = 500,
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
TEST t_manage_window_move(){
  struct window_t *w = get_window_id(cfg->WINDOWID);

  log_debug("Managing Window ID %d @%dx%d", w->window_id, (int)(w->position.x), (int)(w->position.y));
  ASSERT_NEQm("Failed to get focused window", w, NULL);
  bool ok = move_window(w, cfg->X, cfg->Y);

  ASSERT_EQm("Failed to move window", ok, true);

  struct window_t *w1          = get_window_id(w->window_id);
  size_t          waited_qty   = 0;
  unsigned long   started_wait = timestamp();

  while ((int)w1->position.x != cfg->X || (int)w1->position.y != cfg->Y) {
    unsigned long waited_dur = (unsigned long)timestamp() - started_wait;
    if (waited_dur > cfg->max_wait_ms) {
      break;
    }
    if ((waited_qty % 5) == 0) {
      log_debug(AC_YELLOW "Waiting for window to move (%dx%d => %dx%d). Waited %ld/%ldms" AC_RESETALL,
                (int)w->position.x, (int)w->position.y,
                (int)w1->position.x, (int)w1->position.y,
                (unsigned long)(timestamp() - started_wait),
                cfg->max_wait_ms
                );
    }

    usleep(1000 * POLL_WINDOW_STATE_MS);
    w1 = get_window_id(w->window_id);
    waited_qty++;
  }
  unsigned long dur_ms = timestamp() - started_wait;

  log_info(AC_GREEN "Window ID %d moved new position (%dx%d) in " AC_RESETALL AC_BLUE "%s" AC_RESETALL,
           w1->window_id, (int)(w1->position.x), (int)(w1->position.y), milliseconds_to_string((unsigned long)(timestamp() - started_wait))
           );
  ASSERT_EQm("Failed to move window X properly", (int)w1->position.x, cfg->X);
  ASSERT_EQm("Failed to move window Y properly", (int)w1->position.y, cfg->Y);
  asprintf(&msg, AC_GREEN "Moved Window ID %d from %dx%d to %dx%d in " AC_RESETALL AC_BLUE "%s" AC_RESETALL,
           w1->window_id, (int)w->position.x, (int)w->position.y, cfg->X, cfg->Y, milliseconds_to_string(dur_ms)
           );
  PASSm(msg);
} /* t_manage_window_move */

TEST t_manage_window_resize(){
  struct window_t *w = get_window_id(cfg->WINDOWID);

  log_debug("window id %d size: %dx%d", w->window_id, (int)(w->size.width), (int)(w->size.height));
  ASSERT_NEQm("Failed to get focused window", w, NULL);

  unsigned long started_wait = timestamp();
  bool          ok           = resize_window(w, cfg->WIDTH, cfg->HEIGHT);

  unsigned long dur_ms = timestamp() - started_wait;

  ASSERT_EQm("Failed to resize window", ok, true);
  struct window_t *w1 = get_window_id(w->window_id);

  log_info("window id %d size: %dx%d", w1->window_id, (int)w1->size.width, (int)w1->size.height);

  asprintf(&msg, "Resized Window ID %d to %dx%d in %s", w->window_id, cfg->WIDTH, cfg->HEIGHT, milliseconds_to_string(dur_ms));
  PASSm(msg);
}

SUITE(s_manage_window_resize) {
  RUN_TEST(t_manage_window_resize);
}
SUITE(s_manage_window_move) {
  RUN_TEST(t_manage_window_move);
}

GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  setup_cfg();
  GREATEST_MAIN_BEGIN();
  RUN_TEST(t_authorized_tests);
  RUN_SUITE(s_manage_window_move);
  RUN_SUITE(s_manage_window_resize);
  GREATEST_MAIN_END();
}

static void setup_cfg(){
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
  log_info("Using Window ID %d | move %dx%d | resize %dx%d | ", cfg->WINDOWID, cfg->X, cfg->Y, cfg->WIDTH, cfg->HEIGHT);
}
