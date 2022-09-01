////////////////////////////////////////////
#include "bench/bench.h"
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "log.h/log.h"
#include "stb/stb_image.h"
#include "tempdir.c/tempdir.h"
#include "wrec-animation-test/wrec-animation-test.h"
#include "wrec-animation/wrec-animation.h"
#include "wrec-capture/wrec-capture.h"
#include "wrec-common/wrec-common.h"
////////////////////////////////////////////
static const char *tempdir = NULL;

int main(const int argc, const char **argv) {
  if (!isatty(STDOUT_FILENO)) {
    exit(0);
  }
  MEASURE(measurement_create_animated_gif)
  struct animated_png_render_request_t *req = calloc(1, sizeof(struct animated_png_render_request_t));

  asprintf(&tempdir, "/tmp/wrec-animation-test/%d", getpid());
  tempdir = "/tmp/wrec-image-test/1";
  if (fsio_dir_exists(tempdir) == false) {
    fsio_mkdirs(tempdir, 0700);
  }
  if (argc > 1) {
    asprintf(&tempdir, "%s", argv[1]);
  }
  if (fsio_dir_exists(tempdir) == false) {
    exit(1);
  }

  req->png_dir = tempdir;
//  req->max_age_minutes = 10;
  //req->start_ts = timestamp()/1000 - 60*20;
  // req->end_ts = timestamp()/1000 - 60*5*0;
  //req->max_age_hours = 1;
  int res = load_pngs_create_animated_gif(req);

  END_MEASURE(measurement_create_animated_gif)
  MEASURE_SUMMARY(measurement_create_animated_gif);
  return(res);
}
