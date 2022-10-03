#pragma once
#ifndef MT_MA_H
#define MT_MA_H
#include "mt/mt-struct.h"

int ma_moving_average(int *ptrArrNumbers, long *ptrSum, int pos, int len, int nextNum);
float ma_moving_average_float(float *ptrArrNumbers, float *ptrSum, int pos, size_t len, float nextNum);

#endif
