
#include "ansi-codes/ansi-codes.h"
#include "async-test/async-test.h"
#include "async/async.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

TEST t_async_test2(){
  PASS();
}

TEST t_async_test1(){
//  MapIterator *m = MapIterator_new("m");
//  SeriesIterator *s = SeriesIterator_new("s");
  struct Vector *v = vector_new();

//  VCALL(DYN(Iterator,MapIterator,m),iterate);
// VCALL(DYN(Iterator,SeriesIterator,s),iterate);

  PASS();
}

SUITE(s_async_test) {
  RUN_TEST(t_async_test1);
  if (isatty(STDOUT_FILENO))
    RUN_TEST(t_async_test2);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_async_test);
  GREATEST_MAIN_END();
}
