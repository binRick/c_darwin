#pragma once
#ifndef TABLE_SORT_C
#define TABLE_SORT_C
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
#include "timestamp/timestamp.h"
#include "table/sort/sort.h"
static bool TABLE_SORT_DEBUG_MODE = false;
enum sort_direction_t get_sort_key_direction(char *name){
  for(size_t i=0;i<SORT_DIRECTIONS_QTY;i++){
    if(strcmp(sort_direction_keys[i],name)==0)
      return(i);
  }
  return(-1);
}
#define QTY(X) (sizeof(X)/sizeof(X[0]))
sort_function get_sort_type_function_from_key(enum sort_type_t type, const char *name, const char *direction){
  enum sort_direction_t direction_id = get_sort_key_direction(direction);
  if(TABLE_SORT_DEBUG_MODE){
    log_info("type id:%d, name:%s, direction id: %d, direction: %s", type, name, direction_id, direction);
  }
  for (size_t i = 0; i < QTY(sort_types[type]->handlers); i++) {
    if(TABLE_SORT_DEBUG_MODE)
      log_info("     name: %s, dir: %d", sort_types[type]->handlers[i]->name, sort_types[type]->handlers[i]->direction);
    if (strcmp(name, sort_types[type]->handlers[i]->name) == 0 && direction_id == sort_types[type]->handlers[i]->direction) {
      if(TABLE_SORT_DEBUG_MODE)
        log_info("           match");
      return((sort_function)(sort_types[type]->handlers[i]->function));
    }
  }
  log_error("SORT FUNCTION NOT FOUND> type id:%d, name:%s, direction id: %d, direction: %s", type, name, direction_id, direction);
  return NULL;
}
#undef QTY

#define SORT_FUNCTION_STR(NAME,TYPE,ITEM)\
int NAME##_sort_##ITEM##_asc(SORT_PARAMS){\
  if(TABLE_SORT_DEBUG_MODE){\
    log_info("%s",(size_t)((struct TYPE)i0)->ITEM);\
    log_info("%s",(size_t)((struct TYPE)i1)->ITEM);\
  }\
  return (strcmp(   ((struct TYPE)i0)->ITEM, ((struct TYPE)i1)->ITEM));\
}\
int NAME##_sort_##ITEM##_desc(SORT_PARAMS){\
  if(TABLE_SORT_DEBUG_MODE){\
    log_info("%s",(size_t)((struct TYPE)i0)->ITEM);\
    log_info("%s",(size_t)((struct TYPE)i1)->ITEM);\
  }\
  return (strcmp(   ((struct TYPE)i0)->ITEM, ((struct TYPE)i1)->ITEM)*-1);\
}
#define SORT_FUNCTION_INT(NAME,TYPE,ITEM)\
int NAME##_sort_##ITEM##_asc(SORT_PARAMS){\
  if(TABLE_SORT_DEBUG_MODE){\
    log_info("%lu",(size_t)((struct TYPE)i0)->ITEM);\
    log_info("%lu",(size_t)((struct TYPE)i1)->ITEM);\
  }\
  return   (     (((struct TYPE)i0)->ITEM   > ((struct TYPE)i1)->ITEM) ?  1  :   (((struct TYPE)i0)->ITEM  < ((struct TYPE)i1)->ITEM) ? -1 : 0   );\
}\
int NAME##_sort_##ITEM##_desc(SORT_PARAMS){\
  if(TABLE_SORT_DEBUG_MODE){\
    log_info("%lu",(size_t)((struct TYPE)i0)->ITEM);\
    log_info("%lu",(size_t)((struct TYPE)i1)->ITEM);\
  }\
  return   (     (((struct TYPE)i0)->ITEM   < ((struct TYPE)i1)->ITEM) ?  1  :   (((struct TYPE)i0)->ITEM  > ((struct TYPE)i1)->ITEM) ? -1 : 0   );\
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
