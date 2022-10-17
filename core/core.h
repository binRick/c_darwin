#pragma once
#ifndef CORE_H
#define CORE_H
//////////////////////////////////////
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <mach/clock.h>
#include <mach/mach.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
//////////////////////////////////////
#include "submodules/c_ansi/ansi-codes/ansi-codes.h"
#include "submodules/c_ansi/ansi-utils/ansi-utils.h"
//////////////////////////////////////
#include "sqldbal/src/sqldbal.h"
#include "submodules/log/log.h"
#include "bytes/bytes.h"
#include "parson/parson.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "libcyaml/include/cyaml/cyaml.h"
#include "ms/ms.h"
#include "path_module/src/path.h"
#include "timestamp/timestamp.h"
#include "murmurhash.c/murmurhash.h"
#include "libfort/lib/fort.h"
#include "string-utils/string-utils.h"
#include "timelib/timelib.h"
#include "date.c/date.h"
#include "tempdir.c/tempdir.h"
#include "hashmap.h/hashmap.h"
#include "hash/hash.h"
#include "submodules/bitfield/bitfield.h"
#include "submodules/levenshtein/deps/levenshtein.c/levenshtein.h"
#include "submodules/c_exception/lib/CException.h"
//////////////////////////////////////
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
//////////////////////////////////////
#include "frameworks/frameworks.h"
//////////////////////////////////////
#include "core/debug/debug.h"
#include "core/image/image.h"
#include "core/utils/utils.h"
//////////////////////////////////////
#include "window/window.h"
#include "layout/layout.h"
#include "process/process.h"
#include "display/display.h"
#include "image/image.h"
#include "db/db.h"
#include "space/space.h"
#include "app/app.h"
#include "keylogger/keylogger.h"
#include "icon-utils/icon-utils.h"
#include "systemprofiler/systemprofiler.h"
//////////////////////////////////////
//#include "capture/capture.h"
//#include "capture/type/type.h"
//#include "capture/utils/utils.h"
//////////////////////////////////////
#endif
