//
// value.c
//

#include "value.h"
#include <stdio.h>
#include "closure.h"
#include "str.h"
#include "tuple.h"

char *type_name(Type type)
{
  char *name = "bool";
  switch (type)
  {
  case TYPE_BOOL:
    break;
  case TYPE_INT:
    name = "int";
    break;
  case TYPE_STR:
    name = "str";
    break;
  case TYPE_TUPLE:
    name = "tuple";
    break;
  case TYPE_CLOSURE:
    name = "closure";
    break;
  }
  return name;
}

bool value_equal(Value val1, Value val2)
{
  if (val1.type != val2.type)
    return false;
  bool isEqual = false;
  switch (val1.type)
  {
  case TYPE_BOOL:
    isEqual = as_bool(val1) == as_bool(val2);
    break;
  case TYPE_INT:
    isEqual = as_int(val1) == as_int(val2);
    break;
  case TYPE_STR:
    isEqual = str_equal(as_str(val1), as_str(val2));
    break;
  case TYPE_TUPLE:
    isEqual = tuple_equal(as_tuple(val1), as_tuple(val2));
    break;
  case TYPE_CLOSURE:
    isEqual = as_closure(val1) == as_closure(val2);
    break;
  }
  return isEqual;
}

int value_compare(Value val1, Value val2, Result *result)
{
  if (val1.type != val2.type)
  {
    result_error(result, "cannot compare %s and %s", type_name(val1.type),
      type_name(val2.type));
    return 0;
  }
  int cmp = 0;
  switch (val1.type)
  {
  case TYPE_BOOL:
    cmp = as_bool(val1) - as_bool(val2);
    break;
  case TYPE_INT:
    cmp = as_int(val1) - as_int(val2);
    break;
  case TYPE_STR:
    cmp = str_compare(as_str(val1), as_str(val2));
    break;
  case TYPE_TUPLE:
  case TYPE_CLOSURE:
    result_error(result, "cannot compare %s", type_name(val1.type));
    break;
  }
  return cmp;
}

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
