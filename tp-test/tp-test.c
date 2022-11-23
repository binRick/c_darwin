
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "tp-test/tp-test.h"
#include "tp/tp.h"

TEST t_tp_test2(){
  PASS();
}

TEST t_tp_main(){
  tp_main(0, NULL);
  PASS();
}

SUITE(s_tp_test) {
  RUN_TEST(t_tp_main);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  if (isatty(STDOUT_FILENO))
    RUN_SUITE(s_tp_test);
  GREATEST_MAIN_END();
}
