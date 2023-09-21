//
// function.c
//

#include "function.h"
#include <string.h>
#include "memory.h"

static inline void nonlocal_array_init(NonlocalArray *arr, Result *result);
static inline void nonlocal_array_grow(NonlocalArray *arr, Result *result);
static inline void function_array_init(FunctionArray *arr, Result *result);
static inline void function_array_grow(FunctionArray *arr, Result *result);

static inline void nonlocal_array_init(NonlocalArray *arr, Result *result)
{
  int capacity = ARRAY_MIN_CAPACITY;
  Nonlocal *elements = memory_alloc(sizeof(*elements) * capacity, result);
  if (!result_is_ok(result))
    return;
  arr->capacity = capacity;
  arr->count = 0;
  arr->elements = elements;
}

static inline void nonlocal_array_grow(NonlocalArray *arr, Result *result)
{
  if (arr->count < arr->capacity)
    return;
  int capacity = arr->capacity << 1;
  Nonlocal *elements = memory_alloc(sizeof(*elements) * capacity, result);
  if (!result_is_ok(result))
    return;
  memcpy(elements, arr->elements, sizeof(*elements) * arr->count);
  arr->capacity = capacity;
  arr->elements = elements;
}

static inline void function_array_init(FunctionArray *arr, Result *result)
{
  int capacity = ARRAY_MIN_CAPACITY;
  Function **elements = memory_alloc(sizeof(*elements) * capacity, result);
  if (!result_is_ok(result))
    return;
  arr->capacity = capacity;
  arr->count = 0;
  arr->elements = elements;
}

static inline void function_array_grow(FunctionArray *arr, Result *result)
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

Function *function_new(int arity, Result *result)
{
  Function *fn = memory_alloc(sizeof(*fn), result);
  if (!result_is_ok(result))
    return NULL;
  fn->arity = arity;
  chunk_init(&fn->chunk, result);
  if (!result_is_ok(result))
    return NULL;
  array_init(&fn->constants, result);
  if (!result_is_ok(result))
    return NULL;
  nonlocal_array_init(&fn->nonlocals, result);
  if (!result_is_ok(result))
    return NULL;
  function_array_init(&fn->functions, result);
  if (!result_is_ok(result))
    return NULL;
  return fn;
}

uint8_t function_add_nonlocal(Function *fn, bool isLocal, uint8_t index, Result *result)
{
  NonlocalArray *arr = &fn->nonlocals;
  nonlocal_array_grow(arr, result);
  if (!result_is_ok(result))
    return 0;
  Nonlocal nonlocal = {
    .isLocal = isLocal,
    .index = index
  };
  int nonlocalIndex = arr->count;
  if (nonlocalIndex > UINT8_MAX)
  {
    result_error(result, "too many non-locals");
    return 0;
  }
  arr->elements[arr->count] = nonlocal;
  ++arr->count;
  return (uint8_t) nonlocalIndex;
}

void function_add_child(Function *fn, Function *child, Result *result)
{
  FunctionArray *arr = &fn->functions;
  function_array_grow(arr, result);
  if (!result_is_ok(result))
    return;
  arr->elements[arr->count] = child;
  ++arr->count;
}
