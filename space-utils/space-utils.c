#include "core-utils/core-utils-extern.h"
#include "space-utils/space-utils.h"

bool space_is_user(uint64_t sid){
  return(SLSSpaceGetType(g_connection, sid) == 0);
}
