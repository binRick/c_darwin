#pragma once
#ifndef MT_EMA_C
#define MT_EMA_C
#include "mt/mt-include.h"
static struct StringBuffer *sb;

const char *get_largest_embedded_csv_name(){
  size_t size  = 0;
  char   *name = NULL;

  for (size_t i = 0; i < EMBEDDED_CSV_NAMES_QTY; i++) {
    if (strlen(EMBEDDED_CSV_VARS[i]) > size) {
      name = EMBEDDED_CSV_NAMES[i];
    }
  }
  return(name);
}

const char *get_embedded_csv_name_data(const char *name){
  for (size_t i = 0; i < EMBEDDED_CSV_NAMES_QTY; i++) {
    if (strcmp(name, EMBEDDED_CSV_NAMES[i]) == 0) {
      return(EMBEDDED_CSV_VARS[i]);
    }
  }
  return(NULL);
}

const char *get_embedded_csv_names_list(void){
  sb = stringbuffer_new();
  for (size_t i = 0; i < EMBEDDED_CSV_NAMES_QTY; i++) {
    stringbuffer_append_string(sb, EMBEDDED_CSV_NAMES[i]);
    if (i < EMBEDDED_CSV_NAMES_QTY - 1) {
      stringbuffer_append_string(sb, ", ");
    }
  }
  return(stringbuffer_to_string(sb));
}

float avg_float_pointers_v(struct Vector *v){
  if (vector_size(v) == 0) {
    return(0);
  }
  float total = 0;

  for (size_t i = 0; i < vector_size(v); i++) {
    total += *((float *)vector_get(v, i));
  }
  return((float)total / vector_size(v));
}

#endif
