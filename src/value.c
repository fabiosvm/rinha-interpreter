//
// value.c
//

#include "value.h"
#include <stdio.h>
#include "closure.h"
#include "hash.h"
#include "str.h"
#include "tuple.h"

char *type_name(Type type)
{
  char *name = "undefined";
  switch (type)
  {
  case TYPE_UNDEFINED:
    break;
  case TYPE_BOOL:
    name = "bool";
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

uint32_t value_hash(Value val)
{
  uint32_t hash;
  hash_init(&hash);
  Type valType = type(val);
  hash_update(&hash, (uint32_t) valType);
  switch (valType)
  {
  case TYPE_UNDEFINED:
    break;
  case TYPE_BOOL:
    hash_update(&hash, (uint32_t) as_bool(val));
    break;
  case TYPE_INT:
    hash_update_int(&hash, as_int(val));
    break;
  case TYPE_STR:
    hash_update(&hash, str_hash(as_str(val)));
    break;
  case TYPE_TUPLE:
    hash_update(&hash, tuple_hash(as_tuple(val)));
    break;
  case TYPE_CLOSURE:
    hash_update_pointer(&hash, as_closure(val));
    break;
  }
  return hash;
}

bool value_equal(Value val1, Value val2)
{
  if (type(val1) != type(val2))
    return false;
  bool isEqual = true;
  switch (type(val1))
  {
  case TYPE_UNDEFINED:
    break;
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
  if (type(val1) != type(val2))
  {
    result_error(result, "cannot compare %s and %s", type_name(type(val1)),
      type_name(type(val2)));
    return 0;
  }
  int cmp = 0;
  switch (type(val1))
  {
  case TYPE_UNDEFINED:
    break;
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
    result_error(result, "cannot compare %s", type_name(type(val1)));
    break;
  }
  return cmp;
}

void value_print(Value val)
{
  switch (type(val))
  {
  case TYPE_UNDEFINED:
    printf("undefined");
    break;
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
