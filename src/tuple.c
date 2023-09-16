//
// tuple.c
//

#include "tuple.h"
#include <stdio.h>
#include "memory.h"

Tuple *tuple_new(Value first, Value second, Result *result)
{
  Tuple *tuple = memory_alloc(sizeof(*tuple), result);
  if (!result_is_ok(result))
    return NULL;
  tuple->first = first;
  tuple->second = second;
  return tuple;
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
