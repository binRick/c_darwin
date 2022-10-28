#pragma once
#ifndef CACHE_UTILS_C
#define CACHE_UTILS_C
#define CACHE_UTILS_HASH_SEED    912736136
////////////////////////////////////////////
#include "cache-utils/cache-utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "iowow/src/fs/iwfile.h"
#include "iowow/src/iowow.h"
#include "iowow/src/kv/iwkv.h"
#include "iowow/src/log/iwlog.h"
#include "iowow/src/platform/iwp.h"
#include "log/log.h"
#include "ms/ms.h"
#include "murmurhash.c/murmurhash.h"
#include "string-utils/string-utils.h"
#include "tempdir.c/tempdir.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "timestamp/timestamp.h"
#include "which/src/which.h"
////////////////////////////////////////////
static bool cache_utils_DEBUG_MODE = false;
struct cache_utils_item_t {
  IWDB      iwdb;
  IWKV      iwkv;
  IWKV_OPTS opts;
};

////////////////////////////////////////////
#define CLEANUP_CACHE_UTILS_ITEM() \
  iwkv_close(&iwkv);               \
  if (cache_item_file) {           \
    free(cache_item_file);         \
  }                                \
  if (cache_item_key) {            \
    free(cache_item_key);          \
  }                                \
  if (cache_item_ts_key) {         \
    free(cache_item_ts_key);       \
  }

#define INIT_CACHE_UTILS_ITEM()                                                    \
  IWKV_val key, val, tskey, tsval;                                                 \
  char *cache_item_file = NULL, *cache_item_ts_key = NULL, *cache_item_key = NULL; \
  asprintf(&cache_item_file, "%s/.cache_utils-cached-items.db",                    \
           gettempdir()                                                            \
           );                                                                      \
  asprintf(&cache_item_key, "%s_data", stringfn_to_lowercase(ITEM_KEY));           \
  asprintf(&cache_item_ts_key, "%s_ts", cache_item_key);                           \
  IWKV_OPTS opts = {                                                               \
    .path   = cache_item_file,                                                     \
    .oflags = IWFSM_NOLOCKS,                                                       \
  };                                                                               \
  IWKV      iwkv;                                                                  \
  IWDB      mydb;                                                                  \
  iwrc      rc = iwkv_open(&opts, &iwkv);                                          \
  if (rc) {                                                                        \
    iwlog_ecode_error3(rc);                                                        \
    goto fail;                                                                     \
  }                                                                                \
  rc = iwkv_db(iwkv, 1, 0, &mydb);                                                 \
  if (rc) {                                                                        \
    iwlog_ecode_error2(rc, "Failed to open mydb");                                 \
    goto fail;                                                                     \
  }                                                                                \
  tskey.data = cache_item_ts_key;                                                  \
  tskey.size = strlen(tskey.data);                                                 \
  key.data   = cache_item_key;                                                     \
  key.size   = strlen(key.data);
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__cache_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_cache_utils") != NULL) {
    log_debug("Enabling cache-utils Debug Mode");
    cache_utils_DEBUG_MODE = true;
  }
}

bool cache_utils_exists(char *ITEM_KEY){
  bool res = false;

  INIT_CACHE_UTILS_ITEM()
  rc = iwkv_get(mydb, &key, &val);
  if (!rc)
    res = true;
  else if (rc && rc != IWKV_ERROR_NOTFOUND)
    iwlog_ecode_error3(rc);

fail:
  CLEANUP_CACHE_UTILS_ITEM()
  return(res);
}

bool cache_utils_is_expired(char *ITEM_KEY, int TTL){
  return((cache_utils_get_age(ITEM_KEY) > TTL) ? true : false);
}

size_t cache_utils_get_size(char *ITEM_KEY){
  int res = 0;

  INIT_CACHE_UTILS_ITEM()
  val.data = 0;
  val.size = 0;
  rc       = iwkv_get(mydb, &key, &val);
  log_debug("key:%s", (char *)key.data);
  if (!rc)
    res = (size_t)val.size;

  CLEANUP_CACHE_UTILS_ITEM()
  return(res);

fail:
  log_error("Failed to get size");
  CLEANUP_CACHE_UTILS_ITEM()
  return(res);
}

int cache_utils_get_age(char *ITEM_KEY){
  int res = -1;

  INIT_CACHE_UTILS_ITEM()
  tsval.data = 0;
  tsval.size = 0;
  rc         = iwkv_get(mydb, &tskey, &tsval);
  log_debug("tskey:%s", (char *)tskey.data);
  if (!rc)
    res = (int)((size_t)((size_t)timestamp() - string_size_to_size_t((char *)tsval.data)) / 1000);

  CLEANUP_CACHE_UTILS_ITEM()
  return(res);

fail:
  log_error("Failed to get age");
  CLEANUP_CACHE_UTILS_ITEM()
  return(res);
}

size_t cache_utils_get_hash(char *STRING){
  return((size_t)murmurhash(
           (const char *)STRING,
           (uint32_t)strlen((const char *)STRING),
           (uint32_t)CACHE_UTILS_HASH_SEED
           )
         );
}

int cache_utils_set_item(char *ITEM_KEY, char *ITEM_CONTENT){
  int res = EXIT_FAILURE;

  INIT_CACHE_UTILS_ITEM()
  val.data = ITEM_CONTENT;
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
  }else
    res = EXIT_SUCCESS;
  CLEANUP_CACHE_UTILS_ITEM()
  return(res);

fail:
  CLEANUP_CACHE_UTILS_ITEM()
  return(EXIT_FAILURE);
}

char *cache_utils_get_item(char *ITEM_KEY, size_t CACHE_TTL){
  INIT_CACHE_UTILS_ITEM()
  val.data   = 0;
  val.size   = 0;
  tsval.data = 0;
  tsval.size = 0;
  rc         = iwkv_get(mydb, &key, &val);
  if (rc) {
    if (cache_utils_DEBUG_MODE == true) {
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
  if (cache_utils_DEBUG_MODE == true)
    log_debug("%s> %s cache with ts %lu|age: %s|expired:%d|", ITEM_KEY, bytes_to_string(strlen(s)), ts, milliseconds_to_string(((size_t)timestamp()) - ts), expired);
  if (expired == true || strlen(s) < 1024)
    return(NULL);

  CLEANUP_CACHE_UTILS_ITEM()
  return(s);

fail:
  CLEANUP_CACHE_UTILS_ITEM()
  return(NULL);
} /* cache_utils_get_item */

#endif
