////////////////////////////////////////////
#include "bench/bench.h"
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "log.h/log.h"
#include "tempdir.c/tempdir.h"
#include "wrec-capture/wrec-capture.h"
#include "wrec-common/wrec-common.h"
#include "stb/stb_image.h"
#include "wrec-animation-test/wrec-animation-test.h"
////////////////////////////////////////////
static const char *tempdir = NULL;

int main(const int argc, const char **argv) {
  MEASURE(measurement_create_animated_gif)
  asprintf(&tempdir, "/tmp/wrec-animation-test/%d", getpid());
  tempdir = "/tmp/wrec-image-test/1";
  if (fsio_dir_exists(tempdir) == false) {
    fsio_mkdirs(tempdir, 0700);
  }
  int res = load_pngs_create_animated_gif(tempdir);

  END_MEASURE(measurement_create_animated_gif)
  MEASURE_SUMMARY(measurement_create_animated_gif);
  return(res);
}
