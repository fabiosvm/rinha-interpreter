//
// value.h
//

#ifndef VALUE_H
#define VALUE_H

#include "result.h"

#define FLAG_NONE  0x00
#define FLAG_FALSY 0x01

#define FALSE_VALUE      ((Value) { .type = TYPE_BOOL, .flags = FLAG_FALSY, .asBool = false })
#define TRUE_VALUE       ((Value) { .type = TYPE_BOOL, .flags = FLAG_NONE, .asBool = true })
#define int_value(i)     ((Value) { .type = TYPE_INT, .flags = FLAG_NONE, .asInt = (i) })
#define str_value(s)     ((Value) { .type = TYPE_STR, .flags = FLAG_NONE, .asPointer = (s) })
#define tuple_value(t)   ((Value) { .type = TYPE_TUPLE, .flags = FLAG_NONE, .asPointer = (t) })
#define closure_value(c) ((Value) { .type = TYPE_CLOSURE, .flags = FLAG_NONE, .asPointer = (c) })

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
#define is_falsy(v)   ((v).flags & FLAG_FALSY)

typedef enum
{
  TYPE_BOOL,
  TYPE_INT,
  TYPE_STR,
  TYPE_TUPLE,
  TYPE_CLOSURE
} Type;

typedef struct
{
  Type type;
  int flags;
  union
  {
    bool asBool;
    int asInt;
    void *asPointer;
  };
} Value;

char *type_name(Type type);
bool value_equal(Value val1, Value val2);
int value_compare(Value val1, Value val2, Result *result);
void value_print(Value val);

#endif // VALUE_H
