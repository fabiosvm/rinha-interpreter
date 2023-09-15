//
// function.c
//

#include "function.h"
#include <string.h>
#include "memory.h"

static inline void array_function_init(FunctionArray *arr, Result *result);
static inline void array_function_grow(FunctionArray *arr, Result *result);

static inline void array_function_init(FunctionArray *arr, Result *result)
{
  int capacity = ARRAY_MIN_CAPACITY;
  Function **elements = memory_alloc(sizeof(*elements) * capacity, result);
  if (!result_is_ok(result))
    return;
  arr->capacity = capacity;
  arr->count = 0;
  arr->elements = elements;
}

static inline void array_function_grow(FunctionArray *arr, Result *result)
{
  if (arr->count < arr->capacity)
    return;
  int capacity = arr->capacity << 1;
  Function **elements = memory_alloc(sizeof(*elements) * capacity, result);
  if (!result_is_ok(result))
    return;
  memcpy(elements, arr->elements, sizeof(*elements) * arr->count);
  arr->capacity = capacity;
  arr->elements = elements;
}

Function *function_new(int arity, int numNonlocals, Result *result)
{
  Function *fn = memory_alloc(sizeof(*fn), result);
  if (!result_is_ok(result))
    return NULL;
  fn->arity = arity;
  fn->numNonlocals = numNonlocals;
  chunk_init(&fn->chunk, result);
  if (!result_is_ok(result))
    return NULL;
  array_init(&fn->constants, result);
  if (!result_is_ok(result))
    return NULL;
  array_function_init(&fn->children, result);
  if (!result_is_ok(result))
    return NULL;
  return fn;
}

void function_inplace_add_child(Function *fn, Function *child, Result *result)
{
  FunctionArray *arr = &fn->children;
  array_function_grow(arr, result);
  if (!result_is_ok(result))
    return;
  arr->elements[arr->count] = child;
  ++arr->count;
}
