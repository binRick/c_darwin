#pragma once
#ifndef ASYNC_C
#define ASYNC_C
#define LOCAL_DEBUG_MODE    ASYNC_DEBUG_MODE
////////////////////////////////////////////
#include "async/async.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

////////////////////////////////////////////
static bool ASYNC_DEBUG_MODE = false;

struct Iterator_t {
  const char *name;
  int        ncroaks;
};

/*
 * struct SeriesIterator *SeriesIterator_new(const char *name) {
 * struct Iterator_t *self = malloc(sizeof *self);
 * assert(self);
 * self->name    = name;
 * self->ncroaks = 0;
 * return(self);
 * }
 * MapIterator *MapIterator_new(const char *name) {
 * struct Iterator_t *self = malloc(sizeof *self);
 * assert(self);
 * self->name    = name;
 * self->ncroaks = 0;
 * return(self);
 * }
 * bool SeriesIterator_iterate(struct Vector *items,iterator_cb *cb){
 * log_info("series iterator");
 * return(true);
 * }
 */
bool MapIterator_iterate(struct Vector *items, iterator_cb *cb){
  log_info("map iterator");
  return(true);
}
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__async(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_async") != NULL) {
    log_debug("Enabling async Debug Mode");
    ASYNC_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////

interface(AsyncMap);
typedef struct {
  int x, y;
} AsyncMap_t;

#undef LOCAL_DEBUG_MODE
#endif
