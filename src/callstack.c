//
// callstack.c
//

#include "callstack.h"
#include "memory.h"

void callstack_init(Callstack *cstack, int size, Result *result)
{
  CallFrame *frames = memory_alloc(sizeof(*frames) * size, result);
  if (!result_is_ok(result))
    return;
  cstack->size = size;
  cstack->frames = frames;
  cstack->top = &frames[-1];
  cstack->end = &frames[size - 1];
}

CallFrame callstack_pop(Callstack *cstack)
{
  CallFrame frame = cstack->top[0];
  --cstack->top;
  return frame;
}
