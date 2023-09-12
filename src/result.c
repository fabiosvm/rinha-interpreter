//
// result.c
//

#include "result.h"
#include <stdarg.h>
#include <stdio.h>

void result_error(Result *result, const char *fmt, ...)
{
  result->isOk = false;
  va_list args;
  va_start(args, fmt);
  vsnprintf(result->error, RESULT_ERROR_MAX_LENGTH, fmt, args);
  va_end(args);
}
