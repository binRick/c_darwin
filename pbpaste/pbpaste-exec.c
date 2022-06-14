#include "pbpaste.h"
#include "submodules/c_string_buffer/include/stringbuffer.h"
#define STDOUT_READ_BUFFER_SIZE    1024


char *pbpaste_exec(void){
  const char          *command_line[] = { "/bin/sh", "-c", "pbpaste", NULL };
  char                *READ_STDOUT,
                      stdout_buffer[STDOUT_READ_BUFFER_SIZE] = { 0 };
  int                 exited,
                      result;
  struct subprocess_s subprocess;
  struct StringBuffer *SB        = stringbuffer_new_with_options(STDOUT_READ_BUFFER_SIZE, true);
  size_t              bytes_read = 0,
                      index      = 0;

  result = subprocess_create(command_line, 0, &subprocess);
  assert_eq(result, 0, %d);
  do {
    bytes_read = subprocess_read_stdout(&subprocess, stdout_buffer, STDOUT_READ_BUFFER_SIZE - 1);
    if (bytes_read > 0) {
      stringbuffer_append_string(SB, stdout_buffer);
    }
    index += bytes_read;
  } while (bytes_read != 0);
  result = subprocess_join(&subprocess, &exited);
  assert_eq(result, 0, %d);
  assert_eq(exited, 0, %d);
  READ_STDOUT = stringbuffer_to_string(SB);
  stringbuffer_release(SB);

  return(READ_STDOUT);
}
