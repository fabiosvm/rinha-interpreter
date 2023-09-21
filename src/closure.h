//
// closure.h
//

#ifndef CLOSURE_H
#define CLOSURE_H

#include "function.h"
#include "value.h"

typedef struct
{
  Function *fn;
  Value *nonlocals[1];
} Closure;

Closure *closure_new(Function *fn, Result *result);

#endif // CLOSURE_H
