#include "greatest.h"
#include "keylogger-db-test.h"
#include <assert.h>
#include <inttypes.h>
#include <math.h>


////////////////////////////////////////////////////////
TEST t_keylogger_db_lifecycle(void){
  int res = -1;

  res = keylogger_db_lifecycle();
  ASSERT_EQ(res, 0);
  PASS();
} /* t_sqldb */

SUITE(s_keylogger_db_lifecycle){
  RUN_TEST(t_keylogger_db_lifecycle);
  PASS();
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_keylogger_db_lifecycle);
  GREATEST_MAIN_END();
  return(0);
}

