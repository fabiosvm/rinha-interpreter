//
// stack.c
//

#include "stack.h"
#include "memory.h"

void stack_init(Stack *stack, int size, Result *result)
{
  Value *slots = memory_alloc(sizeof(*slots) * size, result);
  if (!result_is_ok(result))
    return;
  stack->size = size;
  stack->slots = slots;
  stack->top = &slots[-1];
  stack->end = &slots[size - 1];
}
