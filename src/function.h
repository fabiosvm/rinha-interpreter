//
// function.h
//

#ifndef FUNCTION_H
#define FUNCTION_H

#include "array.h"
#include "chunk.h"

typedef struct Function
{
  int arity;
  int numNonlocals;
  Chunk chunk;
  Array constants;
  Array functions;
} Function;

Function *function_new(int arity, int numNonlocals, Result *result);

#endif // FUNCTION_H
