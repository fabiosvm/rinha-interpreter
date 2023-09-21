//
// function.h
//

#ifndef FUNCTION_H
#define FUNCTION_H

#include "array.h"
#include "chunk.h"

typedef struct
{
  bool isLocal;
  uint8_t index;
} Nonlocal;

typedef struct
{
  int capacity;
  int count;
  Nonlocal *elements;
} NonlocalArray;

struct Function;

typedef struct
{
  int capacity;
  int count;
  struct Function **elements;
} FunctionArray;

typedef struct Function
{
  int arity;
  Chunk chunk;
  Array constants;
  NonlocalArray nonlocals;
  FunctionArray functions;
} Function;

Function *function_new(int arity, Result *result);
uint8_t function_add_nonlocal(Function *fn, bool isLocal, uint8_t index, Result *result);
void function_add_child(Function *fn, Function *child, Result *result);

#endif // FUNCTION_H
