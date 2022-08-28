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

////////////////////////////////////////////
TEST t_manage_window_move(){
  window_t *w = get_window_id(cfg->WINDOWID);

  log_debug("window id %d pos: %dx%d", w->window_id, (int)(w->position.x), (int)(w->position.y));
  ASSERT_NEQm("Failed to get focused window", w, NULL);
  bool ok = move_window(w, cfg->X, cfg->Y);

  ASSERT_EQm("Failed to move window", ok, true);
  window_t      *w1          = get_window_id(w->window_id);
  unsigned long started_wait = timestamp();
  size_t        waited_qty   = 0;

  while ((int)w1->position.x == (int)w->position.x || (int)w1->position.y == (int)w->position.y) {
    unsigned long waited_dur = (unsigned long)timestamp() - started_wait;
    if (waited_dur > cfg->max_wait_ms) {
      break;
    }
    if ((waited_qty % 5) == 0) {
      log_info("Waiting for window to move (%dx%d => %dx%d). Waited %ld/%ldms",
               (int)w->position.x, (int)w->position.y,
               (int)w1->position.x, (int)w1->position.y,
               (unsigned long)(timestamp() - started_wait),
               cfg->max_wait_ms
               );
    }

    usleep(1000 * 10);
    w1 = get_window_id(w->window_id);
    waited_qty++;
  }
  unsigned long dur_ms = timestamp() - started_wait;

  w1 = get_window_id(w->window_id);
  log_debug("window id %d new position: %dx%d after %ldms", w1->window_id, (int)(w1->position.x), (int)(w1->position.y), (unsigned long)(timestamp() - started_wait));
  ASSERT_EQm("Failed to move window X properly", (int)w1->position.x, cfg->X);
  ASSERT_EQm("Failed to move window Y properly", (int)w1->position.y, cfg->Y);
  asprintf(&msg, "Moved Window ID %d from %dx%d to %dx%d in %s",
           w1->window_id, (int)w->position.x, (int)w->position.y, cfg->X, cfg->Y, milliseconds_to_string(dur_ms)
           );
  PASSm(msg);
} /* t_manage_window_move */

TEST t_manage_window_resize(){
  window_t *w = get_window_id(cfg->WINDOWID);

  log_debug("window id %d size: %dx%d", w->window_id, (int)(w->size.width), (int)(w->size.height));
  ASSERT_NEQm("Failed to get focused window", w, NULL);

  bool ok = resize_window(w, cfg->WIDTH, cfg->HEIGHT);

  ASSERT_EQm("Failed to resize window", ok, true);
  window_t *w1 = get_window_id(w->window_id);

  log_debug("window id %d size: %dx%d", w1->window_id, (int)w1->size.width, (int)w1->size.height);

  asprintf(&msg, "Resized Window ID %d to %dx%d", w->window_id, cfg->WIDTH, cfg->HEIGHT);
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
  log_info("Using Window ID %d | move %dx%d | resize %dx%d | ", cfg->WINDOWID, cfg->X, cfg->Y, cfg->WIDTH, cfg->HEIGHT);
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_manage_window_move);
  RUN_SUITE(s_manage_window_resize);
  GREATEST_MAIN_END();
}
