//
// stack.h
//

#ifndef STACK_H
#define STACK_H

#include "result.h"
#include "value.h"

#define STACK_DEFAULT_SIZE (1 << 10)

#define stack_is_empty(s) ((s)->top == &(s)->slots[-1])
#define stack_is_full(s)  ((s)->top == (s)->end)
#define stack_peek(s, i)  ((s)->top[-(i)])

#define stack_push(s, v) \
  do { \
    ++(s)->top; \
    (s)->slots[(s)->top] = (v); \
  } while (0)

#define stack_pop(s) \
  do { \
    --(s)->top; \
  } while (0)

#define stack_set(s, i, v) \
  do { \
    (s)->slots[(i)] = (v); \
  } while (0)

typedef struct
{
  int size;
  Value *slots;
  Value *top;
  Value *end;
} Stack;

void stack_init(Stack *stack, int size, Result *result);

#endif // STACK_H
