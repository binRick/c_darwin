#pragma once
#ifndef CORE_H
#define CORE_H
//////////////////////////////////////
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <libgen.h>
#include <limits.h>
#include <mach/clock.h>
#include <mach/mach.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
//////////////////////////////////////
#include "submodules/c_ansi/ansi-codes/ansi-codes.h"
#include "submodules/c_ansi/ansi-utils/ansi-utils.h"
//////////////////////////////////////
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "chan-utils/chan-utils.h"
#include "container_of/container_of.h"
#include "csplit/csplit.h"
#include "date.c/date.h"
#include "hash/hash.h"
#include "hashmap.h/hashmap.h"
#include "libcyaml/include/cyaml/cyaml.h"
#include "libfort/lib/fort.h"
#include "ms/ms.h"
#include "murmurhash.c/murmurhash.h"
#include "optparse99/optparse99.h"
#include "parson/parson.h"
#include "path_module/src/path.h"
#include "sqldbal/src/sqldbal.h"
#include "string-utils/string-utils.h"
#include "submodules/bitfield/bitfield.h"
#include "submodules/c_exception/lib/CException.h"
#include "submodules/levenshtein/deps/levenshtein.c/levenshtein.h"
#include "submodules/log/log.h"
#include "tempdir.c/tempdir.h"
#include "timelib/timelib.h"
#include "timestamp/timestamp.h"
#include "variation-ui/vn_ui.h"
#include "c_ringbuf/ringbuf.h"
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
#include "app/app.h"
#include "db/db.h"
#include "display/display.h"
#include "icon-utils/icon-utils.h"
#include "image/image.h"
#include "keylogger/keylogger.h"
#include "layout/layout.h"
#include "menu-bar-utils/menu-bar-utils.h"
#include "optparse99/optparse99.h"
#include "process/process.h"
#include "reproc/reproc/include/reproc/export.h"
#include "reproc/reproc/include/reproc/reproc.h"
#include "space/space.h"
#include "systemprofiler/systemprofiler.h"
#include "window/window.h"
#include <pthread.h>
//////////////////////////////////////
//#include "capture/capture.h"
//#include "capture/type/type.h"
//#include "capture/utils/utils.h"
//////////////////////////////////////
struct optparse_cmd;
struct optparse_opt;
extern char                *__option_names[];
extern struct optparse_opt __optparse_opt[];
extern struct optparse_cmd *dls_cmd;
extern char                *DLS_RE_EXEC_CMD;
extern pthread_mutex_t     *core_stdout_mutex;
/*
 * struct dls_conn_t {
 * char *stdout_mutex,*cmd,**option_names;
 * struct optparse_opt *options;
 * };
 * struct dls_conn_t *__core_conn__ = NULL;
 * struct dls_conn_t *core_get_conn(){
 #define c __core_conn__
 * c->stdout_mutex = core_stdout_mutex;
 * c->cmd = DLS_RE_EXEC_CMD;
 * c->options = &(__optparse_opt);
 * c->option_names = __option_names;
 * return(c);
 #undef c
 * }
 */
/*
 */
#endif
