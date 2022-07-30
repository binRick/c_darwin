#include "c_string_buffer/include/stringbuffer.h"
#include "deps/deps.h"
#include "int/int.h"
#include "pbpaste.h"
#include "smaz/smaz.h"
#include "socket99/socket99.h"
#include "submodules/assertf/assertf.h"
#include "submodules/b64.c/b64.h"
#include "submodules/c_ansi/ansi-codes/ansi-codes.h"
#include "submodules/c_ansi/ansi-utils/ansi-utils.h"
#include "submodules/c_eventemitter/include/eventemitter.h"
#include "submodules/c_forever/include/forever.h"
#include "submodules/c_string_buffer/include/stringbuffer.h"
#include "submodules/c_vector/include/vector.h"
#include "submodules/c_workqueue/include/workqueue.h"
#include "submodules/libbeaufort/include/beaufort.h"
#include "submodules/microtar/src/microtar.h"
#include "submodules/mkcreflect/lib/include/mkcreflect.h"
#include "submodules/murmurhash.c/murmurhash.h"
#include "submodules/occurrences/occurrences.h"
#include "submodules/rhash_md5.c/md5.h"
#include "submodules/subprocess.h/subprocess.h"
#include "submodules/timestamp/timestamp.h"
#include "submodules/tiny-regex-c/re.h"
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
