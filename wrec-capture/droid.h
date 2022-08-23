#ifndef DROID__H
#define DROID__H
#include "interface99/interface99.h"
#include <stdio.h>

#define Droid_IFACE               \
  vfunc(const char *, name, void) \
  vfuncDefault(void, turn_on, Droid droid)
interface(Droid);

typedef struct {
  char dummy;
} C_3PO;
typedef struct {
  char dummy;
} R2_D2;

Droid *turn_on(Droid droid);
void R2_D2_turn_on(Droid droid);
const char *R2_D2_name(void);

const char *C_3PO_name(void);
void Droid_turn_on(Droid droid);

declImplExtern(Droid, R2_D2);
declImplExtern(Droid, C_3PO);
#endif
