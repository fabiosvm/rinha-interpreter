//
// value.h
//

#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>

typedef enum
{
  TYPE_BOOL,
  TYPE_INT,
  TYPE_STR,
  TYPE_TUPLE,
  TYPE_CLOSURE
} Type;

#define FALSE_VALUE      ((Value) { .type = TYPE_BOOL, .asBool = false })
#define TRUE_VALUE       ((Value) { .type = TYPE_BOOL, .asBool = true })
#define int_value(i)     ((Value) { .type = TYPE_INT, .asInt = (i) })
#define str_value(s)     ((Value) { .type = TYPE_STR, .asPointer = (s) })
#define tuple_value(t)   ((Value) { .type = TYPE_TUPLE, .asPointer = (r) })
#define closure_value(c) ((Value) { .type = TYPE_CLOSURE, .asPointer = (c) })

#define as_bool(v)    ((v).asBool)
#define as_int(v)     ((v).asInt)
#define as_str(v)     ((Str *) (v).asPointer)
#define as_tuple(v)   ((Tuple *) (v).asPointer)
#define as_closure(v) ((Closure *) (v).asPointer)

#define is_bool(v)    ((v).type == TYPE_BOOL)
#define is_int(v)     ((v).type == TYPE_INT)
#define is_str(v)     ((v).type == TYPE_STR)
#define is_tuple(v)   ((v).type == TYPE_TUPLE)
#define is_closure(v) ((v).type == TYPE_CLOSURE)

typedef struct
{
  Type type;
  union
  {
    bool asBool;
    int asInt;
    void *asPointer;
  };
} Value;

void value_print(Value val);

#endif // VALUE_H
