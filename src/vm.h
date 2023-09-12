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

#endif // VM_H
