#ifndef OPAQUE_TYPE_SHAPE_C
#define OPAQUE_TYPE_SHAPE_C
#include "interface99/interface99.h"

#include "shape.h"
#include <stdio.h>

int Rectangle_perim(const VSelf) {
  VSELF(const Rectangle);
  return((self->a + self->b) * 2);
}

void Rectangle_scale(VSelf, int factor) {
  VSELF(Rectangle);
  self->a *= factor;
  self->b *= factor;
}

int Triangle_perim(const VSelf) {
  VSELF(const Triangle);
  return(self->a + self->b + self->c);
}

void Triangle_scale(VSelf, int factor) {
  VSELF(Triangle);
  self->a *= factor;
  self->b *= factor;
  self->c *= factor;
}


void test(Shape shape) {
    printf("perim = %d\n", VCALL(shape, perim));
    VCALL(shape, scale, 5);
    printf("perim = %d\n", VCALL(shape, perim));
}


implExtern(Shape, Rectangle);
implExtern(Shape, Triangle);
#endif
