//
// vm.c
//

#include "vm.h"

void vm_init(VM *vm, int stackSize, int callstackSize, Result *result)
{
  stack_init(&vm->stack, stackSize, result);
  if (!result_is_ok(result))
    return;
  callstack_init(&vm->cstack, callstackSize, result);   
}
