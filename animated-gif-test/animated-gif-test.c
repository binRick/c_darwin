////////////////////////////////////////////
#include "animated-gif-test/animated-gif-test.h"
#include "animated-gif/animated-gif.h"
#include "tempdir.c/tempdir.h"
////////////////////////////////////////////

int main(const int argc, const char **argv) {
  char *d = "/tmp/b";

  d = "/tmp/a";
  //d = gettempdir();
  int res = load_pngs_create_animated_gif(d);

  return(res);
}
