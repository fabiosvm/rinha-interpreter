//
// tuple.h
//

#ifndef TUPLE_H
#define TUPLE_H

#include "value.h"

typedef struct
{
  Value first;
  Value second;
} Tuple;

Tuple *tuple_new(Value first, Value second, Result *result);
bool tuple_equal(Tuple *tuple1, Tuple *tuple2);
void tuple_print(Tuple *tuple);

#endif // TUPLE_H
