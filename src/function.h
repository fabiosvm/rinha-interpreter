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
  Chunk chunk;
  Array constants;
  Array functions;
  int numNonlocals;
} Function;

#endif // FUNCTION_H
