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
  Value *nonlocals;
} Closure;

#endif // CLOSURE_H
