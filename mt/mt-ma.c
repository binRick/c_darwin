#pragma once
#ifndef MT_MA_C
#define MT_MA_C
#include "mt/mt-include.h"

float ma_moving_average_float(float *ptrArrNumbers, float *ptrSum, int pos, size_t len, float nextNum){
  *ptrSum            = *ptrSum - ptrArrNumbers[pos] + nextNum;
  ptrArrNumbers[pos] = nextNum;
  float avg = *ptrSum / len;
  if (args->debug_mode)
    log_info("     Averaged Values using value %f in position %d using %lu values and sum of %f to: %f",
             nextNum,
             pos,
             len,
             *ptrSum,
             avg
             );
  return(avg);
}

int ma_moving_average(int *ptrArrNumbers, long *ptrSum, int pos, int len, int nextNum){
  *ptrSum            = *ptrSum - ptrArrNumbers[pos] + nextNum;
  ptrArrNumbers[pos] = nextNum;
  return(*ptrSum / len);
}

#endif
