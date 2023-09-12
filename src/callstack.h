//
// callstack.h
//

#ifndef CALLSTACK_H
#define CALLSTACK_H

#include "closure.h"

#define CALLSTACK_DEFAULT_SIZE (1 << 9)

typedef struct
{
  Closure *cl;
  uint8_t *ip;
  Value *slots;
} CallFrame;

typedef struct
{
  int size;
  CallFrame *frames;
  CallFrame *top;
  CallFrame *end;
} Callstack;

void callstack_init(Callstack *cstack, int size, Result *result);

#endif // CALLSTACK_H
