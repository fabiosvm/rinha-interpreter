//
// value.c
//

#include "value.h"
#include <stdio.h>
#include "str.h"
#include "tuple.h"

void value_print(Value val)
{
  switch (val.type)
  {
  case TYPE_BOOL:
    printf("%s", as_bool(val) ? "true" : "false");
    break;
  case TYPE_INT:
    printf("%d", as_int(val));
    break;
  case TYPE_STR:
    str_print(as_str(val));
    break;
  case TYPE_TUPLE:
    tuple_print(as_tuple(val));
    break;
  case TYPE_CLOSURE:
    printf("<#closure>");
    break;
  }  
}
