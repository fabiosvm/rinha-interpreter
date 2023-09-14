//
// chunk.h
//

#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>
#include "result.h"

#define CHUNK_MIN_CAPACITY (1 << 3)

typedef enum
{
  OP_CONSTANT,
  OP_TUPLE,
  OP_CLOSURE,
  OP_GET_LOCAL,
  OP_SET_LOCAL,
  OP_GET_NONLOCAL,
  OP_SET_NONLOCAL,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_REM,
  OP_EQ,
  OP_NEQ,
  OP_LT,
  OP_GT,
  OP_LTE,
  OP_GTE,
  OP_JUMP,
  OP_JUMP_IF_FALSE,
  OP_FIRST,
  OP_SECOND,
  OP_PRINT,
  OP_CALL,
  OP_RETURN,
  OP_HALT
} OpCode;

typedef struct
{
  int capacity;
  int count;
  uint8_t *code;
} Chunk;

void chunk_init(Chunk *chunk, Result *result);
void chunk_emit_byte(Chunk *chunk, uint8_t byte, Result *result);
void chunk_emit_word(Chunk *chunk, uint16_t word, Result *result);

#endif // CHUNK_H
