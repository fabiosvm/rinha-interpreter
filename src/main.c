//
// main.c
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "str.h"

static inline void print_usage(const char *name);
static inline Str *read_file(const char *filename);
static inline FILE *open_file(const char *filename);

static inline void print_usage(const char *name)
{
  printf("Usage: %s <filename>\n", name);
}

static inline Str *read_file(const char *filename)
{
  FILE *fp = open_file(filename);
  assert(!fseek(fp, 0, SEEK_END));
  long length = ftell(fp);
  Result result;
  result_ok(&result);
  Str *str = str_alloc(length, &result);
  if (!result_is_ok(&result))
  {
    fprintf(stderr, "fatal error: %s\n", result.error);
    exit(EXIT_FAILURE);
  }
  rewind(fp);
  assert((int) fread(str->chars, 1, length, fp) == length);
  assert(!fclose(fp));
  return str;
}

static inline FILE *open_file(const char *filename)
{
  FILE *fp = NULL;
#ifdef _WIN32
  int rc = fopen_s(&fp, filename, "r");
#else
  fp = fopen(filename, "r");
#endif
#ifdef _WIN32
  if (rc)
#else
  if (!fp)
#endif
  {
    fprintf(stderr, "fatal error: could not open file '%s'\n", filename);
    exit(EXIT_FAILURE);
  }
  return fp;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }
  Str *str = read_file(argv[1]);
  Result result;
  result_ok(&result);
  Closure *cl = compile(str->chars, &result);
  if (!result_is_ok(&result))
  {
    fprintf(stderr, "fatal error: %s\n", result.error);
    exit(EXIT_FAILURE);
  }
  (void) cl;
  printf("Syntax OK\n");
  return EXIT_SUCCESS;
}
