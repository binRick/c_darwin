
#include "../include/includes.h"


void move_mouse(const CGPoint pt){
  CGPostMouseEvent(pt, 1, 1, 1);
}


void test_move_mouse(){
  CGPoint pt;

  pt.x = 100;
  pt.y = 200;
  move_mouse(pt);
}
