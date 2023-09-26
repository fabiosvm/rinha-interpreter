//
// tuple.c
//

#include "tuple.h"
#include <stdio.h>
#include "hash.h"
#include "memory.h"

Tuple *tuple_new(Value first, Value second, Result *result)
{
  Tuple *tuple = memory_alloc(sizeof(*tuple), result);
  if (!result_is_ok(result))
    return NULL;
  tuple->hash = -1;
  tuple->first = first;
  tuple->second = second;
  return tuple;
}

uint32_t tuple_hash(Tuple *tuple)
{
  if (tuple->hash == -1)
  {
    uint32_t hash;
    hash_init(&hash);
    hash_update_int(&hash, value_hash(tuple->first));
    hash_update_int(&hash, value_hash(tuple->second));
    tuple->hash = hash;
  }
  return (uint32_t) tuple->hash;
}

bool tuple_equal(Tuple *tuple1, Tuple *tuple2)
{
  return value_equal(tuple1->first, tuple2->first) &&
    value_equal(tuple1->second, tuple2->second);
}

void tuple_print(Tuple *tuple)
{
  printf("(");
  value_print(tuple->first);
  printf(", ");
  value_print(tuple->second);
  printf(")");
}
