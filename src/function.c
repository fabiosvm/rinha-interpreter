//
// function.c
//

#include "function.h"
#include "memory.h"

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
  array_init(&fn->functions, result);
  if (!result_is_ok(result))
    return NULL;
  return fn;
}
