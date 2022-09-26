#pragma once
#ifndef TABLE_SORT_C
#define TABLE_SORT_C
#ifndef QTY
#define QTY(X)    (sizeof(X) / sizeof(X[0]))
#endif
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app/utils/utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "font-utils/font-utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "table/sort/sort.h"
#include "timestamp/timestamp.h"
static bool TABLE_SORT_DEBUG_MODE = false;
enum sort_direction_t get_sort_key_direction(char *name){
  for (size_t i = 0; i < SORT_DIRECTIONS_QTY; i++) {
    if (strcmp(sort_direction_keys[i],
               name) == 0) {
      return(i);
    }
  }
  return(-1);
}

int default_sort_function(SORT_PARAMS){
  return(1);
}

bool is_valid_sort_type_name(enum sort_type_t type, const char *name){
  for (size_t i = 0; i < SORT_BY_TYPES_QTY; i++) {
    if (sort_types[type]->enabled && sort_by_type_names[i] && strcmp(name, sort_by_type_names[i]) == 0) {
      return(true);
    }
  }
  return(false);
}

sort_function get_sort_type_function_from_key(enum sort_type_t type, const char *name, const char *direction){
  enum sort_direction_t direction_id = get_sort_key_direction(direction);
  bool                  valid        = is_valid_sort_type_name(type, name);

  if (TABLE_SORT_DEBUG_MODE) {
    log_info("type id:%d, name:%s, direction id: %d, direction: %s, valid: %s", type, name, direction_id, direction, valid?"Yes":"No");
  }
  if ((int)direction_id >= 0 && valid) {
    for (size_t i = 0; i < SORT_BY_TYPES_QTY; i++) {
      if (strcmp(name, sort_by_type_names[i]) == 0) {
        if (TABLE_SORT_DEBUG_MODE) {
          log_info(" Sort Handler Match for %s", sort_by_type_names[i]);
        }
        return(sort_types[type]->handlers[i]->functions[direction_id]);
      }
    }
  }
  log_error("SORT FUNCTION NOT FOUND> type id:%d, name:%s, direction id: %d, direction: %s", type, name, direction_id, direction);
  return(0);
}
#define SORT_FUNCTION_STR(NAME, TYPE, ITEM)                                \
  int NAME ## _sort_ ## ITEM ## _asc(SORT_PARAMS){                         \
    return(strcmp(((struct TYPE)i0)->ITEM, ((struct TYPE)i1)->ITEM));      \
  }                                                                        \
  int NAME ## _sort_ ## ITEM ## _desc(SORT_PARAMS){                        \
    return(-1 * strcmp(((struct TYPE)i0)->ITEM, ((struct TYPE)i1)->ITEM)); \
  }
#define SORT_FUNCTION_INT(NAME, TYPE, ITEM)                                                                                             \
  int NAME ## _sort_ ## ITEM ## _asc(SORT_PARAMS){                                                                                      \
    if (TABLE_SORT_DEBUG_MODE) {                                                                                                        \
      log_info("%lu", (size_t)((struct TYPE)i0)->ITEM);                                                                                 \
      log_info("%lu", (size_t)((struct TYPE)i1)->ITEM);                                                                                 \
    }                                                                                                                                   \
    return((((struct TYPE)i0)->ITEM > ((struct TYPE)i1)->ITEM) ?  1  :   (((struct TYPE)i0)->ITEM < ((struct TYPE)i1)->ITEM) ? -1 : 0); \
  }                                                                                                                                     \
  int NAME ## _sort_ ## ITEM ## _desc(SORT_PARAMS){                                                                                     \
    if (TABLE_SORT_DEBUG_MODE) {                                                                                                        \
      log_info("%lu", (size_t)((struct TYPE)i0)->ITEM);                                                                                 \
      log_info("%lu", (size_t)((struct TYPE)i1)->ITEM);                                                                                 \
    }                                                                                                                                   \
    return((((struct TYPE)i0)->ITEM < ((struct TYPE)i1)->ITEM) ?  1  :   (((struct TYPE)i0)->ITEM > ((struct TYPE)i1)->ITEM) ? -1 : 0); \
  }
SORT_FUNCTIONS()
#undef SORT_FUNCTION_INT
#undef SORT_FUNCTION_STR
////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__sort(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_sort") != NULL) {
    log_debug("Enabling sort Debug Mode");
    TABLE_SORT_DEBUG_MODE = true;
  }
}
#endif
