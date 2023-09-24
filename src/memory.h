//
// memory.h
//

#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include "result.h"

void *memory_alloc(size_t size, Result *result);
void *memory_realloc(void *ptr, size_t size, Result *result);

#endif // MEMORY_H
