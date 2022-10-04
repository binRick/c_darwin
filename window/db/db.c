#pragma once
#ifndef WINDOW_DB_C
#define WINDOW_DB_C
#define LOCAL_DEBUG_MODE    WINDOW_DB_DEBUG_MODE
////////////////////////////////////////////
#include "window/db/db.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "sqldbal/src/sqldbal.h"
#include "timestamp/timestamp.h"
#include "whereami/src/whereami.h"
struct whereami_report_t {
  const char *executable, *executable_directory, *executable_basename;
};
static struct whereami_report_t *window_db_whereami_report(void);
////////////////////////////////////////////
static bool WINDOW_DB_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////

#define DRIVER      SQLDBAL_DRIVER_SQLITE
#define PORT        NULL
#define USERNAME    NULL
#define PASSWORD    NULL
#define DATABASE    NULL
#define FLAGS       (SQLDBAL_FLAG_DEBUG |              \
                     SQLDBAL_FLAG_SQLITE_OPEN_CREATE | \
                     SQLDBAL_FLAG_SQLITE_OPEN_READWRITE)

struct whereami_report_t *window_db_whereami_report(){
  struct whereami_report_t *r = calloc(1, sizeof(struct whereami_report_t));
  char                     *path = NULL;
  int                      length, dirname_length;

  length = wai_getExecutablePath(NULL, 0, &dirname_length);
  if (length > 0) {
    path = (char *)malloc(length + 1);
    wai_getExecutablePath(path, length, &dirname_length);
    path[length] = '\0';
    asprintf(&(r->executable), "%s", path);
    path[dirname_length] = '\0';
    asprintf(&(r->executable_directory), "%s", path);
    asprintf(&(r->executable_basename), "%s", path + dirname_length + 1);
    free(path);
    return(r);
  }
  return(NULL);
}

static void __attribute__((constructor)) __constructor__window_db(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_window_db") != NULL) {
    log_debug("Enabling window_db Debug Mode");
    WINDOW_DB_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#endif
