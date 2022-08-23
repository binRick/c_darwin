////////////////////////////////////////////
#define STB_IMAGE_IMPLEMENTATION
#include "tempdir.c/tempdir.h"
#include "wrec-animation-test/wrec-animation-test.h"
#include "wrec-animation/wrec-animation.h"
#include "wrec-common/wrec-common.h"
////////////////////////////////////////////

int main(const int argc, const char **argv) {
  char *d = "/tmp/b";

  d = "/tmp/a";
  int res = load_pngs_create_animated_gif(d);

  return(res);
}
