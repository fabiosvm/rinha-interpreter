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
  length = (length < 0) ? (int) strlen(chars) : length;
  Str *str = str_alloc(length, result);
  if (!result_is_ok(result))
    return NULL;
  memcpy(str->chars, chars, length);
  return str;
}

Str *str_from_bool(bool value, Result *result)
{
  return str_new(-1, value ? "true" : "false", result);
}

Str *str_from_int(int value, Result *result)
{
  char chars[16] = { 0 };
  snprintf(chars, sizeof(chars), "%d", value);
  return str_new(-1, chars, result);
}

Str *str_concat(Str *str1, Str *str2, Result *result)
{
  int length = str1->length + str2->length;
  Str *str = str_alloc(length, result);
  if (!result_is_ok(result))
    return NULL;
  memcpy(str->chars, str1->chars, str1->length);
  memcpy(&str->chars[str1->length], str2->chars, str2->length);
  return str;
}

bool str_equal(Str *str1, Str *str2)
{
  if (str1->length != str2->length)
    return false;
  return !strncmp(str1->chars, str2->chars, str1->length);
}

int str_compare(Str *str1, Str *str2)
{
  int minLength = (str1->length < str2->length) ? str1->length : str2->length;
  int result = strncmp(str1->chars, str2->chars, minLength);
  if (result)
    return result;
  return str1->length - str2->length;
}

void str_print(Str *str)
{
  printf("%.*s", str->length, str->chars);
}
