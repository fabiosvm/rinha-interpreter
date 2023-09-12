//
// array.h
//

#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>
#include "result.h"

#define ARRAY_MIN_CAPACITY (1 << 3)

#define array_is_empty(a) (!(a)->count)
#define array_get(a, i)   ((a)->elements[(i)])

typedef uint64_t Opaque;

typedef struct
{
  int capacity;
  int count;
  Opaque *elements;
} Array;

void array_init(Array *arr, Result *result);
void array_inplace_add(Array *arr, Opaque elem, Result *result);

#endif // ARRAY_H
