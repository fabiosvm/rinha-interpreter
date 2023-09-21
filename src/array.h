//
// array.h
//

#ifndef ARRAY_H
#define ARRAY_H

#include "value.h"

#define ARRAY_MIN_CAPACITY (1 << 3)

#define array_get(a, i) ((a)->elements[(i)])

typedef struct
{
  int capacity;
  int count;
  Value *elements;
} Array;

void array_init(Array *arr, Result *result);
void array_add(Array *arr, Value val, Result *result);

#endif // ARRAY_H
