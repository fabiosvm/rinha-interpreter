//
// vm.h
//

#ifndef VM_H
#define VM_H

#include "callstack.h"
#include "stack.h"

typedef struct
{
  Stack stack;
  Callstack cstack;
} VM;

void vm_init(VM *vm, int stackSize, int cstackSize, Result *result);
void vm_run(VM *vm, Closure *cl, Result *result);

#endif // VM_H
