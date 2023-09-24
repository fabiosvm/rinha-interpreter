//
// memory.h
//

#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include "result.h"

#define MEMORY_SIZE (1 << 27) /* 128 MB */ 

void *memory_alloc(size_t size, Result *result);

#endif // MEMORY_H
