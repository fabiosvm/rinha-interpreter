//
// str.h
//

#ifndef STR_H
#define STR_H

#include "result.h"

typedef struct
{
  int length;
  char chars[1];
} Str;

Str *str_alloc(int length, Result *result);
Str *str_new(int length, const char *chars, Result *result);
void str_print(Str *str);

#endif // STR_H
