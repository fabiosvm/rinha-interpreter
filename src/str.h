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
Str *str_from_bool(bool value, Result *result);
Str *str_from_int(int value, Result *result);
Str *str_concat(Str *str1, Str *str2, Result *result);
bool str_equal(Str *str1, Str *str2);
int str_compare(Str *str1, Str *str2);
void str_print(Str *str);

#endif // STR_H
