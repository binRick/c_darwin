#include <stdbool.h>
#define DEBUG_PASTEBOARD_TEST    true
//////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "greatest/greatest.h"
#include "pasteboard-test/pasteboard-test.h"
#include "pasteboard/pasteboard.h"


TEST t_pasteboard(void){
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
  PASS();
}


SUITE(s_pasteboard){
  RUN_TEST(t_pasteboard);
  PASS();
}


GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_pasteboard);
  GREATEST_MAIN_END();
  return(0);
}
