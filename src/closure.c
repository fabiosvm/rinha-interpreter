//
// closure.c
//

#include "closure.h"
#include "memory.h"

Closure *closure_new(Function *fn, Result *result)
{
  int size = sizeof(Closure) + sizeof(Value) * (fn->numNonlocals - 1);
  Closure *cl = memory_alloc(size, result);
  if (!result_is_ok(result))
    return NULL;
  cl->fn = fn;
  return cl;
}
