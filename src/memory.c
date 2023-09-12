//
// memory.c
//

#include "memory.h"
#include <stdint.h>

static uint8_t memory[MEMORY_SIZE];
static uint8_t *memoryPtr = memory;
static uint8_t *memoryEnd = &memory[MEMORY_SIZE];

void *memory_alloc(size_t size, Result *result)
{
  void *ptr = memoryPtr;
  memoryPtr += size;
  if (memoryPtr > memoryEnd)
  {
    result_error(result, "out of memory");
    return NULL;
  }
  return ptr;
}
