//
// memory.c
//

#include "memory.h"
#include <stdlib.h>

void *memory_alloc(size_t size, Result *result)
{
 void *ptr = malloc(size);
  if (!ptr)
  {
    result_error(result, "out of memory");
    return NULL;
  }
  return ptr;
}

void *memory_realloc(void *ptr, size_t size, Result *result)
{
  ptr = realloc(ptr, size);
  if (!ptr)
  {
    result_error(result, "out of memory");
    return NULL;
  }
  return ptr;
}
