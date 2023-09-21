//
// value.h
//

#ifndef VALUE_H
#define VALUE_H

#include "result.h"

#ifndef TAGGED_POINTER
#define TAGGED_POINTER 1
#endif

#if TAGGED_POINTER
#include <stdint.h>
#endif

typedef enum
{
  TYPE_BOOL,
  TYPE_INT,
  TYPE_STR,
  TYPE_TUPLE,
  TYPE_CLOSURE
} Type;

#define FLAG_NONE  0x00
#define FLAG_FALSY 0x01

#if TAGGED_POINTER
#define FALSE_VALUE      ((Value) { .bits = (((((int64_t) false) << 1) | FLAG_FALSY) << 3) | TYPE_BOOL })
#define TRUE_VALUE       ((Value) { .bits = (((int64_t) true) << 4) | TYPE_BOOL })
#define int_value(i)     ((Value) { .bits = (((int64_t) (i)) << 4) | TYPE_INT })
#define str_value(s)     ((Value) { .bits = (((int64_t) (s)) << 4) | TYPE_STR })
#define tuple_value(t)   ((Value) { .bits = (((int64_t) (t)) << 4) | TYPE_TUPLE })
#define closure_value(c) ((Value) { .bits = (((int64_t) (c)) << 4) | TYPE_CLOSURE })
#endif

#if !TAGGED_POINTER
#define FALSE_VALUE      ((Value) { .type = TYPE_BOOL, .flags = FLAG_FALSY, .asBool = false })
#define TRUE_VALUE       ((Value) { .type = TYPE_BOOL, .flags = FLAG_NONE, .asBool = true })
#define int_value(i)     ((Value) { .type = TYPE_INT, .flags = FLAG_NONE, .asInt = (i) })
#define str_value(s)     ((Value) { .type = TYPE_STR, .flags = FLAG_NONE, .asPointer = (s) })
#define tuple_value(t)   ((Value) { .type = TYPE_TUPLE, .flags = FLAG_NONE, .asPointer = (t) })
#define closure_value(c) ((Value) { .type = TYPE_CLOSURE, .flags = FLAG_NONE, .asPointer = (c) })
#endif

#if TAGGED_POINTER
#define type(v)  ((Type) ((v).bits & 0x07))
#define flags(v) ((((v).bits) >> 3) & 0x01)
#endif

#if !TAGGED_POINTER
#define type(v)  ((v).type)
#define flags(v) ((v).flags)
#endif

#define is_bool(v)    (type(v) == TYPE_BOOL)
#define is_int(v)     (type(v) == TYPE_INT)
#define is_str(v)     (type(v) == TYPE_STR)
#define is_tuple(v)   (type(v) == TYPE_TUPLE)
#define is_closure(v) (type(v) == TYPE_CLOSURE)
#define is_falsy(v)   (flags(v) & FLAG_FALSY)

#if TAGGED_POINTER
#define as_bool(v)    ((bool) ((v).bits >> 4))
#define as_int(v)     ((int) ((v).bits >> 4))
#define as_str(v)     ((Str *) ((v).bits >> 4))
#define as_tuple(v)   ((Tuple *) ((v).bits >> 4))
#define as_closure(v) ((Closure *) ((v).bits >> 4))
#endif

#if !TAGGED_POINTER
#define as_bool(v)    ((v).asBool)
#define as_int(v)     ((v).asInt)
#define as_str(v)     ((Str *) (v).asPointer)
#define as_tuple(v)   ((Tuple *) (v).asPointer)
#define as_closure(v) ((Closure *) (v).asPointer)
#endif

#if TAGGED_POINTER
typedef union
{
  int64_t bits;
  void *asPointer;
} Value;
#endif

#if !TAGGED_POINTER
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
#endif

char *type_name(Type type);
bool value_equal(Value val1, Value val2);
int value_compare(Value val1, Value val2, Result *result);
void value_print(Value val);

#endif // VALUE_H
