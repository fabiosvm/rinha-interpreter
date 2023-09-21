//
// closure.c
//

#include "closure.h"
#include "memory.h"

Closure *closure_new(Function *fn, Result *result)
{
  int numNonlocals = fn->nonlocals.count;
  int size = sizeof(Closure) + sizeof(Value *) * (numNonlocals - 1);
  Closure *cl = memory_alloc(size, result);
  if (!result_is_ok(result))
    return NULL;
  cl->fn = fn;
  return cl;
}
