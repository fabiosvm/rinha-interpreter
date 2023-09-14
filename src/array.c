//
// array.c
//

#include "array.h"
#include <string.h>
#include "memory.h"

static inline void grow(Array *arr, Result *result);

static inline void grow(Array *arr, Result *result)
{
  if (arr->count < arr->capacity)
    return;
  int capacity = arr->capacity << 1;
  Opaque *elements = memory_alloc(sizeof(*elements) * capacity, result);
  if (!result_is_ok(result))
    return;
  memcpy(elements, arr->elements, sizeof(*elements) * arr->count);
  arr->capacity = capacity;
  arr->elements = elements;
}

void array_init(Array *arr, Result *result)
{
  int capacity = ARRAY_MIN_CAPACITY;
  Opaque *elements = memory_alloc(sizeof(*elements) * capacity, result);
  if (!result_is_ok(result))
    return;
  arr->capacity = capacity;
  arr->count = 0;
  arr->elements = elements;
}

void array_inplace_add(Array *arr, Opaque elem, Result *result)
{
  grow(arr, result);
  if (!result_is_ok(result))
    return;
  arr->elements[arr->count] = elem;
  ++arr->count;
}
