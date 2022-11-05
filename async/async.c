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
#include "c_workqueue/include/workqueue.h"
////////////////////////////////////////////
static bool ASYNC_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__async(void);
////////////////////////////////////////////

AsyncSeries *AsyncSeries_load(struct Vector *items){
  AsyncSeries *self = calloc(1,sizeof(AsyncSeries));
  self->common.items = items;
  return(self);
}

AsyncWaterfall *AsyncWaterfall_load(struct Vector *items){
  AsyncSeries *self = calloc(1,sizeof(AsyncWaterfall));
  self->common.items = items;
  return(self);
}

AsyncMap *AsyncMap_load(struct Vector *items){
  AsyncSeries *self = calloc(1,sizeof(AsyncMap));
  self->common.items = items;
  log_info("Loaded %lu Items",vector_size(self->common.items));
  return(self);
}

struct Vector *AsyncSeries_iterate(VSelf, async_iterator iterator){
  VSELF(AsyncSeries);
  log_info("Series Iterating %lu Items",vector_size(self->common.items));
  self->common.results = vector_new();
  for(size_t i=0;i<vector_size(self->common.items);i++)
    vector_push(self->common.results, iterator(i,vector_size(self->common.items),(void*)(vector_get(self->common.items,i))));
  return(self->common.results);
}

void *AsyncWaterfall_iterate(VSelf,wi_async_iterator iterator){
  VSELF(AsyncWaterfall);
  log_info("Waterfall Iterating %lu Items",vector_size(self->common.items));
  void *res = NULL;
  self->common.results = vector_new();
  for(size_t i=0;i<vector_size(self->common.items);i++){
    res = iterator(i,vector_size(self->common.items),(void*)(vector_get(self->common.items,i)),res);
    vector_push(self->common.results, res);
  }
  return(self->common.results);
}

struct Vector *AsyncMap_iterate(VSelf,async_iterator iterator, size_t concurrency){
  VSELF(AsyncMap);
  log_info("Map Iterating %lu Items",vector_size(self->common.items));
  self->common.results = vector_new();
  self->concurrency = concurrency;

  struct WorkQueue *queues[self->concurrency];
  for (size_t wq = 0; wq < self->concurrency; wq++) {
    queues[wq] = workqueue_new_with_options(vector_size(self->common.items), NULL);
    log_info("Queue Size: %zu Backlog Size: %zu\n", workqueue_get_queue_size(queues[wq]), workqueue_get_backlog_size(queues[wq]));
  }
  for (size_t wq = 0; wq < self->concurrency; wq++) {
    printf("Backlog Size: %zu\n", workqueue_get_backlog_size(queues[wq]));
  }
  for (size_t wq = 0; wq < self->concurrency; wq++) {
    workqueue_drain(queues[wq]);
  }
  for (size_t wq = 0; wq < self->concurrency; wq++) {
    printf("Backlog Size: %zu\n", workqueue_get_backlog_size(queues[wq]));
  }
  for (size_t wq = 0; wq < self->concurrency; wq++) {
    workqueue_release(queues[wq]);
  }  

  for(size_t i=0;i<vector_size(self->common.items);i++)
    vector_push(self->common.results, iterator(i,vector_size(self->common.items),(void*)(vector_get(self->common.items,i))));
  return(self->common.results);
}
implExtern(ASYNC, AsyncSeries);
implExtern(ASYNC, AsyncWaterfall);
implExtern(ASYNC, AsyncMap);
static void __attribute__((constructor)) __constructor__async(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_async") != NULL) {
    log_debug("Enabling async Debug Mode");
    ASYNC_DEBUG_MODE = true;
  }
}
#undef LOCAL_DEBUG_MODE
#endif
