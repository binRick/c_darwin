#pragma once
#ifndef ASYNC_H
#define ASYNC_H
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
//////////////////////////////////////
#include "c_vector/vector/vector.h"
#include "chan/src/chan.h"
#include "chan/src/queue.h"
#include "interface99/interface99.h"
typedef struct MapIterator MapIterator;
typedef bool (*iterator_f)(void *);
typedef bool (^iterator_b)(void *);
typedef union {
  iterator_f *f;
  iterator_b *b;
} iterator_cb;
struct iterator_t {
  iterator_cb cb;
};

#define Iterator_IFACE    vfunc(bool, iterate, VSelf, struct Vector *, iterator_cb *)
interface(Iterator);

bool MapIterator_iterate(struct Vector *, iterator_cb *);
declImplExtern(Iterator, MapIterator);

bool SeriesIterator_iterate(struct Vector *, iterator_cb *);
declImplExtern(Iterator, SeriesIterator);

/*
 #define Iterate_IFACE               \
 * vfunc(const char *, name, void) \
 * vfuncDefault(void, iterate, Iterate i)
 * interface(Iterate);
 * typedef struct {
 * struct Vector *items;
 * size_t concurrency;
 * unsigned long started, dur;
 * } Map;
 * typedef struct {
 * struct Vector *items;
 * unsigned long started, dur;
 * } Series;
 * Iterate *iterate(Iterate i);
 * void Map_iterate(Iterate i);
 * void Series_iterate(Iterate i);
 * const char *Map_name(void);
 * const char *Series_name(void);
 * declImplExtern(Iterate, Series);
 * declImplExtern(Iterate, Map);
 */

#endif
