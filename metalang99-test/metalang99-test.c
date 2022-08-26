#include "metalang99-test/metalang99-test.h"
///////////////////////////////////////////////
void test_shape(Shape shape);

TEST t_frog(void){
  Frog *paul  = Frog_new("Paul");
  Frog *steve = Frog_new("Steve");

  VCALL(DYN(Frog, Croak, paul), croak);
  VCALL(DYN(Frog, Croak, steve), croak);
  VCALL(DYN(Frog, Croak, steve), croak);
  VCALL(DYN(Frog, Croak, steve), croak);
  VCALL(DYN(Frog, Croak, steve), croak);
  VCALL(DYN(Frog, Croak, paul), croak);

  Frog_free(paul);
  Frog_free(steve);

  PASS();
}

void test_shape(Shape shape) {
  printf("perim        = %d\n", VCALL(shape, perim));
  VCALL(shape, scale, 5);
  printf("scaled perim = %d\n", VCALL(shape, perim));
}

TEST t_shapes(void){
  Shape r = DYN_LIT(Rectangle, Shape, { 5, 7 });
  Shape t = DYN_LIT(Triangle, Shape, { 10, 20, 30 });

  test_shape(r);
  test_shape(t);

  PASS();
}

TEST t_droid(void){
  Droid c_3po = DYN_LIT(C_3PO, Droid, { 0 });
  Droid r2_d2 = DYN_LIT(R2_D2, Droid, { 0 });

  VCALL_OBJ(c_3po, turn_on);
  VCALL_OBJ(r2_d2, turn_on);

  PASS();
}

SUITE(s_droid){
  RUN_TEST(t_droid);
}
SUITE(s_shapes){
  RUN_TEST(t_shapes);
}
SUITE(s_frog){
  RUN_TEST(t_frog);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_frog);
  RUN_SUITE(s_droid);
  RUN_SUITE(s_shapes);
  GREATEST_MAIN_END();
  return(0);
}
