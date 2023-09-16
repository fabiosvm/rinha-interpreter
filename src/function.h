//
// function.h
//

#ifndef FUNCTION_H
#define FUNCTION_H

#include "array.h"
#include "chunk.h"

struct Function;

typedef struct Closure
{
  int capacity;
  int count;
  struct Function **elements;
} FunctionArray;

typedef struct Function
{
  int arity;
  int numNonlocals;
  Chunk chunk;
  Array constants;
  FunctionArray functions;
} Function;

Function *function_new(int arity, int numNonlocals, Result *result);
void function_inplace_add_child(Function *fn, Function *child, Result *result);

#endif // FUNCTION_H
