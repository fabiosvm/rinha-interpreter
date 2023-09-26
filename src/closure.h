//
// closure.h
//

#ifndef CLOSURE_H
#define CLOSURE_H

#include "function.h"
#include "value.h"

#ifndef MEMORIZATION
#define MEMORIZATION 0
#endif

#if MEMORIZATION
#define CLOSURE_MEMO_CAPACITY (1 << 4) /* 16 */
#define CLOSURE_MEMO_MASK     (CLOSURE_MEMO_CAPACITY - 1)
#endif

typedef struct
{
  Function *fn;
#if MEMORIZATION
  Value *memo;
#endif
  Value nonlocals[1];
} Closure;

Closure *closure_new(Function *fn, Result *result);

#if MEMORIZATION
void closure_memorize(Closure *cl, Value *args, Value resultVal);
Value closure_remember(Closure *cl, Value *args);
#endif

#endif // CLOSURE_H
