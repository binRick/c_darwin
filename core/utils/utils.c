#pragma once
#ifndef CORE_UTILS_C
#define CORE_UTILS_C
////////////////////////////////////////////
#include "core/utils/utils.h"
////////////////////////////////////////////

////////////////////////////////////////////
static bool CORE_UTILS_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
struct whereami_report_t *core_utils_whereami_report(){
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
static void __attribute__((constructor)) __constructor__core_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_core_utils") != NULL) {
    log_debug("Enabling core_utils Debug Mode");
    CORE_UTILS_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
