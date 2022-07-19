#define DEBUG_MODE                false
#define DEBUG_KITTEN_LISTEN_ON    false
#include "dbg.h"
//#include "debug-memory/debug_memory.h"
#include "audit-test.h"
#include "audit/audit.h"
#include "print.h"


TEST t_audit(void){
  int res = audit_main();

  ASSERT_EQ(res, 0);
  PASS();
}


SUITE(s_audit){
  RUN_TEST(t_audit);
  PASS();
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_audit);
  GREATEST_MAIN_END();
  return(0);
}
