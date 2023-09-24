//
// chunk.c
//

#include "chunk.h"
#include "memory.h"

static inline void ensure_capacity(Chunk *chunk, int minCapacity, Result *result);

static inline void ensure_capacity(Chunk *chunk, int minCapacity, Result *result)
{
  if (minCapacity <= chunk->capacity)
    return;
  int capacity = chunk->capacity << 1;
  while (capacity < minCapacity)
    capacity <<= 1;
  uint8_t *code = memory_realloc(chunk->code, capacity, result);
  if (!result_is_ok(result))
    return;
  chunk->capacity = capacity;
  chunk->code = code;
}

void chunk_init(Chunk *chunk, Result *result)
{
  int capacity = CHUNK_MIN_CAPACITY;
  uint8_t *code = memory_alloc(capacity, result);
  if (!result_is_ok(result))
    return;
  chunk->capacity = capacity;
  chunk->count = 0;
  chunk->code = code;
}

void chunk_emit_byte(Chunk *chunk, uint8_t byte, Result *result)
{
  ensure_capacity(chunk, chunk->count + 1, result);
  chunk->code[chunk->count] = byte;
  ++chunk->count;
}

void chunk_emit_word(Chunk *chunk, uint16_t word, Result *result)
{
  ensure_capacity(chunk, chunk->count + 1, result);
  *((uint16_t *) &chunk->code[chunk->count]) = word;
  chunk->count += 2;
}
