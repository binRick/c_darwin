#include <stdbool.h>
#define DEBUG_PASTEBOARD_TEST    true
//////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bench/bench.h"
#include "bytes/bytes.h"
#include "c_greatest/greatest/greatest.h"
#include "pasteboard-test/pasteboard-test.h"
#include "pasteboard/pasteboard.h"


/*
 * ////////////////////////////////////////
 *  BENCHMARK_QTY(benchmark_name, 3)
 *  //DO_WORK
 *  END_BENCHMARK(benchmark_name)
 *  BENCHMARK_SUMMARY(benchmark_name);
 * ////////////////////////////////////////
 *  MEASURE(measurement_name)
 *  //DO_WORK
 *  END_MEASURE(measurement_name)
 *  MEASURE_SUMMARY(measurement_name);
 * ////////////////////////////////////////
 */


TEST t_pasteboard_benchmark(void){
  char *pasteboard_content;

  BENCHMARK_QTY(benchmark_pasteboard, 250)

  pasteboard_content = read_clipboard();
  ASSERT_NEQ(pasteboard_content, NULL);

  END_BENCHMARK(benchmark_pasteboard)
  BENCHMARK_SUMMARY(benchmark_pasteboard);

  free(pasteboard_content);
}


TEST t_pasteboard(void){
  MEASURE(pasteboard_measurement)

  char *pasteboard_content = read_clipboard();

  if (pasteboard_content == NULL) {
    fprintf(stdout,
            AC_RESETALL AC_RED "pasteboard read failed." AC_RESETALL "\n"
            );
  }else{
    size_t len = (pasteboard_content != NULL) ? strlen(pasteboard_content) : 0;
    fprintf(stdout,
            AC_RESETALL "pasteboard_content:" AC_RESETALL "\n"
            AC_RESETALL AC_YELLOW AC_INVERSE "%.40000s" AC_RESETALL
            AC_RESETALL "...\n" AC_RESETALL
            AC_RESETALL "pasteboard_content size:" AC_RESETALL " "
            AC_RESETALL AC_GREEN AC_BOLD "%s" AC_RESETALL
            AC_RESETALL " (%lub)" AC_RESETALL
            "\n",
            pasteboard_content,
            bytes_to_string(len), len
            );
  }
  END_MEASURE(pasteboard_measurement)
  MEASURE_SUMMARY(pasteboard_measurement);
  PASS();
}


SUITE(s_pasteboard){
  RUN_TEST(t_pasteboard);
  RUN_TEST(t_pasteboard_benchmark);
  PASS();
}


GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_pasteboard);
  GREATEST_MAIN_END();
  return(0);
}
