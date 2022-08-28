////////////////////////////////////////////

////////////////////////////////////////////
#include "record-focused-window-test/record-focused-window-test.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"
#include "wrec-common/wrec-common.h"
static char   *msg, *tempdir;
static bool   _DEBUG_MODE = false;
static size_t CAPTURE_QTY = 5, CAPTURE_INTERVAL_MS = 5000, CAPTURED_QTY = 0;

char *generate_window_id_screenshot_file_path(unsigned long WINDOW_ID){
  char *SCREENSHOT_FILE = NULL;

  if (getenv("SCREENSHOT") != NULL) {
    asprintf(&SCREENSHOT_FILE, "%s-%lu.png", getenv("SCREENSHOT"), WINDOW_ID);
  }else{
    asprintf(&SCREENSHOT_FILE, "%s/%lu-%ld.png", tempdir, WINDOW_ID, (size_t)timestamp());
  }
  if (fsio_dir_exists(dirname(SCREENSHOT_FILE)) == false) {
    fsio_mkdirs(dirname(SCREENSHOT_FILE), 0700);
  }
  return(SCREENSHOT_FILE);
}

////////////////////////////////////////////
TEST t_record_focused_window(void){
  size_t        SLEEP_MS = 1000;
  unsigned long started = timestamp(), dur = 0;

  while (CAPTURED_QTY < CAPTURE_QTY) {
    started = timestamp();
//    struct Vector *windows = get_windows();
    dur = timestamp() - started;
    //  asprintf(&msg, "%lu windows in %s",vector_size(windows),milliseconds_to_string(dur));

    struct window_t *w               = get_focused_window();
    char            *SCREENSHOT_FILE = generate_window_id_screenshot_file_path(w->window_id);
    capture_to_file_image(w->window_id, SCREENSHOT_FILE);
    dur = timestamp() - started;
    asprintf(&msg, "Acquired Screenshot"
             "\n\t|id:%d|pid:%d|%dx%d|disp:%d\n\t|app:%s|space:%d|dur:%s|dir:%s|",
             w->window_id, w->pid, w->width, w->height, w->display_id, w->app_name, w->space_id,
             milliseconds_to_string(dur), tempdir

             );
    if (_DEBUG_MODE) {
      log_info("%s", msg);
    }

    if (capture_to_file_image(w->window_id, SCREENSHOT_FILE) == true) {
      CAPTURED_QTY++;
      fprintf(stdout, "%s\n", SCREENSHOT_FILE);
    }else{
      log_error("Failed to capture window id %d", w->window_id);
    }
//  free(SCREENSHOT_FILE);
    usleep(SLEEP_MS * 1000);
  }
  dur = timestamp() - started;
  asprintf(&msg, "Acquired %lu Screenshots in %s", CAPTURED_QTY, milliseconds_to_string(dur));
  PASSm(msg);
}

SUITE(s_record_focused_window) {
  RUN_TEST(t_record_focused_window);
}

GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  tempdir = NULL;
  if (getenv("STORAGE_DIR") != NULL) {
    asprintf(&tempdir, "%s", getenv("STORAGE_DIR"));
  }
  if (tempdir == NULL) {
    asprintf(&tempdir, "/tmp/record-focused-window-test/%d", getpid());
  }
  if (fsio_dir_exists(tempdir) == false) {
    fsio_mkdirs(tempdir, 0700);
  }

  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_record_focused_window);
  GREATEST_MAIN_END();
}
