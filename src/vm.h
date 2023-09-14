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

void vm_init(VM *vm, int stackSize, int callstackSize, Result *result);

#endif // VM_H
