#include"c_greatest/greatest/greatest.h"
#include "pbpaste-exec.c"
#include "pbpaste.h"
//////////////////////////////////////////////
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


clipboard_event_t encode_clipboard_event(char *CLIPBOARD_CONTENT){
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
