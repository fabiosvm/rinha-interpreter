//
// array.h
//

#ifndef ARRAY_H
#define ARRAY_H

#include "result.h"
#include "value.h"

#define ARRAY_MIN_CAPACITY (1 << 3)

#define array_is_empty(a) (!(a)->count)
#define array_get(a, i)   ((a)->elements[(i)])

typedef struct
{
  int capacity;
  int count;
  Value *elements;
} Array;

void array_init(Array *arr, Result *result);
void array_inplace_add(Array *arr, Value val, Result *result);

#endif // ARRAY_H
