#ifndef OPAQUE_TYPE_SHAPE_H
#define OPAQUE_TYPE_SHAPE_H
#include "interface99/interface99.h"

#define Shape_IFACE              \
  vfunc(int, perim, const VSelf) \
  vfunc(void, scale, VSelf, int factor)

interface(Shape);

typedef struct {
  int a, b;
} Rectangle;

typedef struct {
  int a, b, c;
} Triangle;

void test(Shape shape);
void Triangle_scale(VSelf, int factor);
int Triangle_perim(const VSelf);
void Rectangle_scale(VSelf, int factor);
int Rectangle_perim(const VSelf);

declImplExtern(Shape, Triangle);
declImplExtern(Shape, Rectangle);
#endif
