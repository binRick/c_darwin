#include "pbpaste-exec.c"
#include "pbpaste.h"
//////////////////////////////////////////////
#include "b64.c/b64.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "deps/deps.h"
#include "greatest.h"
#include "int/int.h"
#include "smaz/smaz.h"
#include "socket99/socket99.h"
#include "submodules/assertf/assertf.h"
#include "submodules/b64.c/b64.h"
#include "submodules/c_ansi/ansi-codes/ansi-codes.h"
#include "submodules/c_ansi/ansi-utils/ansi-utils.h"
#include "submodules/c_eventemitter/include/eventemitter.h"
#include "submodules/c_forever/include/forever.h"
#include "submodules/c_vector/include/vector.h"
#include "submodules/c_workqueue/include/workqueue.h"
//#include "submodules/dbg.h/dbg.h"
#include "submodules/libbeaufort/include/beaufort.h"
#include "submodules/microtar/src/microtar.h"
#include "submodules/mkcreflect/lib/include/mkcreflect.h"
#include "submodules/murmurhash.c/murmurhash.h"
#include "submodules/occurrences/occurrences.h"
#include "submodules/rhash_md5.c/md5.h"
#include "submodules/subprocess.h/subprocess.h"
#include "submodules/timestamp/timestamp.h"
#include "submodules/tiny-regex-c/re.h"
#include <stdio.h>
//////////////////////////////////////////////
static const uint32_t __murmurhashseed__ = 27362362;


void dump_cliboard_event(const clipboard_event_t *E){
  fprintf(stdout,

          "[raw]       :" AC_RESETALL "\n\t'<<<" AC_YELLOW AC_REVERSED "%s" AC_RESETALL ">>>'\n\t(%lub)\t\n\n" AC_RESETALL
          "[b64]       :" AC_RESETALL "\n\t'<<<" AC_YELLOW AC_REVERSED "%s" AC_RESETALL ">>>'\n\t(%lub)\t\n\n" AC_RESETALL
          "[compressed]:" AC_RESETALL "\n\t'<<<" AC_YELLOW AC_REVERSED "%s" AC_RESETALL ">>>'\n\t(%lub)(%dcomp level)\t\n\n" AC_RESETALL
          "[hash]      :" AC_RESETALL "\n\t'<<<" AC_YELLOW AC_REVERSED "%" PRIu32 "|" "%" PRIu32 " (seed) " AC_RESETALL ">>>'\t\n\n" AC_RESETALL

          , E->raw, E->raw_size,
          E->b64, E->b64_size,
          E->compressed, E->compressed_size, E->compression_level,
          E->hash, E->seed

          );
}


clipboard_event_t encode_cliboard_event(char *CLIPBOARD_CONTENT){
  size_t            LEN = strlen(CLIPBOARD_CONTENT);
  clipboard_event_t E   = {
    .raw               = CLIPBOARD_CONTENT,
    .raw_size          = LEN,
    .b64               = b64_encode(CLIPBOARD_CONTENT, LEN),
    .compressed        = "",
    .compression_level = 0,
    .b64_size          = 0,                           .hash = 0,.seed = (uint32_t)__murmurhashseed__,
  };

  E.b64_size          = strlen(E.b64);
  E.compressed_size   = smaz_compress(E.raw, LEN, E.compressed, sizeof(E.compressed));
  E.compression_level = 100 - ((100 * E.compressed_size) / LEN);
  E.hash              = murmurhash(E.raw, (uint32_t)LEN, E.seed);

  return(E);
}
