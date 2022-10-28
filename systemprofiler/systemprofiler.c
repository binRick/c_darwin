#pragma once
#ifndef SYSTEMPROFILER_C
#define SYSTEMPROFILER_C
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "submodules/reproc/reproc/include/reproc/export.h"
#include "submodules/reproc/reproc/include/reproc/reproc.h"
#include "systemprofiler/systemprofiler.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "which/src/which.h"
////////////////////////////////////////////
static bool SYSTEMPROFILER_DEBUG_MODE = false;
static int save_system_profiler_cache_item(char *ITEM_NAME, char *SYSTEM_PROFILER_JSON_CONTENT);
static char *load_system_profiler_cache_item(char *ITEM_NAME, size_t CACHE_TTL);
char *run_system_profiler_item_subprocess(char *ITEM_NAME, size_t CACHE_TTL);
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__systemprofiler(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_SYSTEMPROFILER") != NULL) {
    log_debug("Enabling systemprofiler Debug Mode");
    SYSTEMPROFILER_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#define CLEANUP_SYSTEM_PROFILER_CACHE_ITEM() \
  if (val.size > 0) {                        \
    iwkv_val_dispose(&val);                  \
  }                                          \
  iwkv_close(&iwkv);                         \
  if (cache_item_file) {                     \
    free(cache_item_file);                   \
  }                                          \
  if (cache_item_key) {                      \
    free(cache_item_key);                    \
  }                                          \
  if (cache_item_ts_key) {                   \
    free(cache_item_ts_key);                 \
  }

#define INIT_SYSTEM_PROFILER_CACHE_ITEM()                                   \
  char *cache_item_file = NULL, *cache_item_ts_key = NULL, *cache_item_key; \
  asprintf(&cache_item_file, "%s/.systemprofiler-cache-%s.db",              \
           gettempdir(),                                                    \
           stringfn_to_lowercase(ITEM_NAME)                                 \
           );                                                               \
  asprintf(&cache_item_key, "%s_data", stringfn_to_lowercase(ITEM_NAME));   \
  asprintf(&cache_item_ts_key, "%s_ts", cache_item_key);                    \
  IWKV_OPTS opts = {                                                        \
    .path   = cache_item_file,                                              \
    .oflags = IWFSM_NOLOCKS,                                                \
  };                                                                        \
  IWKV      iwkv;                                                           \
  IWDB      mydb;                                                           \
  iwrc      rc = iwkv_open(&opts, &iwkv);                                   \
  if (rc) {                                                                 \
    iwlog_ecode_error3(rc);                                                 \
    goto fail;                                                              \
  }                                                                         \
  rc = iwkv_db(iwkv, 1, 0, &mydb);                                          \
  if (rc) {                                                                 \
    iwlog_ecode_error2(rc, "Failed to open mydb");                          \
    goto fail;                                                              \
  }                                                                         \
  IWKV_val key, val;                                                        \
  IWKV_val tskey, tsval;                                                    \
  tskey.data = cache_item_key;                                              \
  tskey.size = strlen(tskey.data);                                          \
  key.data   = cache_item_ts_key;                                           \
  key.size   = strlen(key.data);

static int save_system_profiler_cache_item(char *ITEM_NAME, char *SYSTEM_PROFILER_JSON_CONTENT){
  INIT_SYSTEM_PROFILER_CACHE_ITEM()
  val.data = SYSTEM_PROFILER_JSON_CONTENT;
  val.size = strlen(val.data);
  rc       = iwkv_put(mydb, &key, &val, 0);
  if (rc) {
    iwlog_ecode_error3(rc);
    goto fail;
  }
  asprintf(&tsval.data, "%lu", (size_t)timestamp());
  tsval.size = strlen(tsval.data);
  rc         = iwkv_put(mydb, &tskey, &tsval, 0);
  if (rc) {
    iwlog_ecode_error3(rc);
    goto fail;
  }
  CLEANUP_SYSTEM_PROFILER_CACHE_ITEM()
  return(EXIT_SUCCESS);

fail:
  CLEANUP_SYSTEM_PROFILER_CACHE_ITEM()
  return(EXIT_FAILURE);
}

static char *load_system_profiler_cache_item(char *ITEM_NAME, size_t CACHE_TTL){
  INIT_SYSTEM_PROFILER_CACHE_ITEM()
  val.data   = 0;
  val.size   = 0;
  tsval.data = 0;
  tsval.size = 0;
  rc         = iwkv_get(mydb, &key, &val);
  if (rc) {
    if (SYSTEMPROFILER_DEBUG_MODE == true) {
      if (rc == IWKV_ERROR_NOTFOUND)
        log_error("Failed to get %s/%s from %s", cache_item_key, cache_item_ts_key, cache_item_file);
      else
        iwlog_ecode_error3(rc);
    }
    goto fail;
  }
  rc = iwkv_get(mydb, &tskey, &tsval);
  if (rc) {
    if (rc == IWKV_ERROR_NOTFOUND)
      log_error("Items %s/%s not found in %s", cache_item_key, cache_item_ts_key, cache_item_file);
    else
      iwlog_ecode_error3(rc);
    goto fail;
  }
  char   **ep;
  size_t ts      = (size_t)strtoimax(tsval.data, &ep, 10);
  char   *s      = strdup(val.data);
  bool   expired = ((((size_t)timestamp() - ts) / 1000) > CACHE_TTL) ? true : false;
  if (SYSTEMPROFILER_DEBUG_MODE == true)
    log_debug("%s> %s cache with ts %lu|age: %s|expired:%d|", ITEM_NAME, bytes_to_string(strlen(s)), ts, milliseconds_to_string(((size_t)timestamp()) - ts), expired);
  if (expired == true || strlen(s) < 1024)
    return(NULL);

  CLEANUP_SYSTEM_PROFILER_CACHE_ITEM()
  return(s);

fail:
  CLEANUP_SYSTEM_PROFILER_CACHE_ITEM()
  return(NULL);
} /* load_system_profiler_cache_item */

char *run_system_profiler_item_subprocess(char *ITEM_NAME, size_t CACHE_TTL){
  char *cached = load_system_profiler_cache_item(ITEM_NAME, CACHE_TTL);

  if ((cached != NULL) && (cached[0] == '{') && (cached[strlen(cached) - 1] == '}')) {
    if (SYSTEMPROFILER_DEBUG_MODE == true)
      log_debug("%s> returning cache!", ITEM_NAME);
    return(cached);
  }

  enum { NUM_CHILDREN = 1 };

  reproc_event_source  children[NUM_CHILDREN] = { { 0 } };
  int                  r                      = -1;
  struct  StringBuffer *SB                    = stringbuffer_new();

  for (int i = 0; i < NUM_CHILDREN; i++) {
    reproc_t   *process = reproc_new();
    const char *args[]  = {
      (char *)which_path("system_profiler", getenv("PATH")),
      "-detailLevel",
      "mini",
      "-json",
      ITEM_NAME,
      NULL
    };

    r = reproc_start(process, args, (reproc_options){ .nonblocking = true });
    if (r < 0)
      goto finish;

    children[i].process   = process;
    children[i].interests = REPROC_EVENT_OUT;
  }

  for ( ;;) {
    r = reproc_poll(children, NUM_CHILDREN, REPROC_INFINITE);
    if (r < 0) {
      r = r == REPROC_EPIPE ? 0 : r;
      goto finish;
    }

    for (int i = 0; i < NUM_CHILDREN; i++) {
      if (children[i].process == NULL || !children[i].events)
        continue;
      uint8_t output[1024 * 256];
      r = reproc_read(children[i].process, REPROC_STREAM_OUT, output, sizeof(output));
      if (r == REPROC_EPIPE) {
        children[i].process = reproc_destroy(children[i].process);
        continue;
      }

      if (r < 0)
        goto finish;

      output[r] = '\0';
      stringbuffer_append_string(SB, output);
    }
  }

finish:
  for (int i = 0; i < NUM_CHILDREN; i++)
    reproc_destroy(children[i].process);

  if (r < 0) {
    log_error("%s", reproc_strerror(r));
    return(NULL);
  }
  char *output = stringbuffer_to_string(SB);

  stringbuffer_release(SB);
  save_system_profiler_cache_item(ITEM_NAME, output);
  return(output);
} /* run_system_profiler_item_subprocess */
#endif
