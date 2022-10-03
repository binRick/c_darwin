#pragma once
#ifndef MT_EMA_H
#define MT_EMA_H
#include "mt/mt-struct.h"

float avg_float_pointers_v(struct Vector *v);
const char *get_embedded_csv_names_list(void);
const char *get_embedded_csv_name_data(const char *name);
const char *get_largest_embedded_csv_name(void);

#endif
