#ifndef __DROID__IC
#define __DROID__IC
#include "interface99/interface99.h"

#include "droid.h"
#include <stdio.h>

void Droid_turn_on(Droid droid) {
  printf("Turning on %s...\n", droid.vptr->name());
}

const char *C_3PO_name(void) {
  return("C-3PO");
}
implExtern(Droid, C_3PO);

const char *R2_D2_name(void) {
  return("R2-D2");
}

void R2_D2_turn_on(Droid droid) {
  Droid_turn_on(droid);
  puts("Waaaaoow!");
}

#define R2_D2_turn_on_CUSTOM    ()
implExtern(Droid, R2_D2);
#endif
