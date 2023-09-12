//
// tuple.h
//

#ifndef TUPLE_H
#define TUPLE_H

#include "result.h"
#include "value.h"

typedef struct
{
  Value first;
  Value second;
} Tuple;

Tuple *tuple_new(Value first, Value second, Result *result);
void tuple_print(Tuple *tuple);

#endif // TUPLE_H
