//
// vm.c
//

#include "vm.h"

void vm_init(VM *vm, int stackSize, int cstackSize, Result *result)
{
  stack_init(&vm->stack, stackSize, result);
  if (!result_is_ok(result))
    return;
  callstack_init(&vm->cstack, cstackSize, result);   
}

void vm_run(VM *vm, Closure *cl, Result *result)
{
  // TODO: Implement this function.
  (void) vm;
  (void) cl;
  (void) result;
}
