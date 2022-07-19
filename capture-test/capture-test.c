#define DEBUG_MODE    false
#include "dbg.h"
//#include "debug-memory/debug_memory.h"
#include "capture-test.h"
#include "print.h"


TEST t_capture_images(void){
  struct Vector *window_ids = get_windows_ids();

  dbg(vector_size(window_ids), %d);
  ASSERT_GTEm("Failed to get window ids", vector_size(window_ids), 1);
  for (int i = 0; i < vector_size(window_ids); i++) {
    int  id               = (int)vector_get(window_ids, i);
    char *SCREENSHOT_FILE = malloc(1024);
    sprintf(SCREENSHOT_FILE, "/tmp/window-%d.png", id);
    dbg(SCREENSHOT_FILE, %s);
    bool ok = capture_to_file_image(id, SCREENSHOT_FILE);
    ASSERT_EQm("Failed to take screenshot", ok, true);
    free(SCREENSHOT_FILE);
  }
  PASS();
}


SUITE(s_capture){
  RUN_TEST(t_capture_images);
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_capture);
  GREATEST_MAIN_END();
  return(0);
}
