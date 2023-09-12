//
// str.c
//

#include "str.h"
#include <stdio.h>
#include <string.h>
#include "memory.h"

Str *str_alloc(int length, Result *result)
{
  Str *str = memory_alloc(sizeof(*str) + length, result);
  if (!result_is_ok(result))
    return NULL;
  str->length = length;
  str->chars[length] = '\0';
  return str;
}

Str *str_new(int length, const char *chars, Result *result)
{
  Str *str = str_alloc(length, result);
  if (!result_is_ok(result))
    return NULL;
  memcpy(str->chars, chars, length);
  return str;
}

void str_print(Str *str)
{
  printf("%.*s", str->length, str->chars);
}
