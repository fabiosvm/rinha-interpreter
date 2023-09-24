//
// callstack.h
//

#ifndef CALLSTACK_H
#define CALLSTACK_H

#include "closure.h"

#define CALLSTACK_DEFAULT_SIZE (1 << 10) /* 1 KB */

#define callstack_is_empty(cs) ((cs)->top == &(cs)->frames[-1])
#define callstack_is_full(cs)  ((cs)->top == (cs)->end)

#define callstack_push(cs, cl, ip, s) \
  do { \
    ++(cs)->top; \
    CallFrame frame = { \
      .cl = (cl), \
      .ip = (ip), \
      .slots = (s), \
    }; \
    (cs)->top[0] = frame; \
  } while (0)

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
CallFrame callstack_pop(Callstack *cstack);

#endif // CALLSTACK_H
