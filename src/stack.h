//
// stack.h
//

#ifndef STACK_H
#define STACK_H

#include "value.h"

#define STACK_DEFAULT_SIZE (1 << 20) /* 1MB */

#define stack_is_full(s) ((s)->top == (s)->end)
#define stack_peek(s, i) ((s)->top[-(i)])

#define stack_push(s, v) \
  do { \
    ++(s)->top; \
    (s)->top[0] = (v); \
  } while (0)

#define stack_pop(s) \
  do { \
    --(s)->top; \
  } while (0)

#define stack_set(s, i, v) \
  do { \
    (s)->top[-(i)] = (v); \
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
