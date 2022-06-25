#pragma once
#include "c_string_buffer/include/stringbuffer.h"
#include "submodules/assertf/assertf.h"
#include "submodules/subprocess.h/subprocess.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
//////////////////////////////////////
#include "deps/deps.h"
#include "int/int.h"
#include "smaz/smaz.h"
#include "socket99/socket99.h"
#include "submodules/b64.c/b64.h"
#include "submodules/c_ansi/ansi-codes/ansi-codes.h"
#include "submodules/c_ansi/ansi-utils/ansi-utils.h"
#include "submodules/c_eventemitter/include/eventemitter.h"
#include "submodules/c_forever/include/forever.h"
#include "submodules/c_vector/include/vector.h"
#include "submodules/c_workqueue/include/workqueue.h"
#include "submodules/dbg.h/dbg.h"
#include "submodules/libbeaufort/include/beaufort.h"
#include "submodules/microtar/src/microtar.h"
#include "submodules/mkcreflect/lib/include/mkcreflect.h"
#include "submodules/murmurhash.c/murmurhash.h"
#include "submodules/occurrences/occurrences.h"
#include "submodules/rhash_md5.c/md5.h"
#include "submodules/timestamp/timestamp.h"
#include "submodules/tiny-regex-c/re.h"
typedef struct {
  char     *raw, *b64;
  char     compressed[2048];
  size_t   raw_size, b64_size, compressed_size;
  int      compression_level;
  uint32_t hash, seed;
} clipboard_event_t;

char *pbpaste_exec(void);


clipboard_event_t encode_cliboard_event(char *CLIPBOARD_CONTENT);
void dump_cliboard_event(const clipboard_event_t *E);
