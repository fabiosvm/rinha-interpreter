//
// result.h
//

#ifndef RESULT_H
#define RESULT_H

#include <stdbool.h>

#define RESULT_ERROR_MAX_LENGTH 511

#define result_is_ok(r) ((r)->isOk)

#define result_ok(r) \
  do { \
    (r)->isOk = true; \
  } while (0)

typedef struct
{
  bool isOk;
  char error[RESULT_ERROR_MAX_LENGTH + 1];
} Result;

void result_error(Result *result, const char *fmt, ...);

#endif // RESULT_H
