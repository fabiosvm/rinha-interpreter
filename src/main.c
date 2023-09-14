//
// main.c
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "compiler.h"
#include "str.h"

#ifdef _WIN32
  #define stat _stat
#endif

static inline void print_usage(const char *name);
static inline Str *read_file(const char *filename);
static inline int file_size(const char *filename);
static inline FILE *open_file(const char *filename);

static inline void print_usage(const char *name)
{
  printf("Usage: %s <filename>\n", name);
}

static inline Str *read_file(const char *filename)
{
  int length = file_size(filename);
  Result result;
  result_ok(&result);
  Str *str = str_alloc(length, &result);
  if (!result_is_ok(&result))
  {
    fprintf(stderr, "fatal error: %s\n", result.error);
    exit(EXIT_FAILURE);
  }
  FILE *fp = open_file(filename);
  if (fread(str->chars, length, 1, fp) != 1)
  {
    fprintf(stderr, "fatal error: could not read file '%s'\n", filename);
    exit(EXIT_FAILURE);
  }
  (void) fclose(fp);
  return str;
}

static inline int file_size(const char *filename)
{
  struct stat st;
  if (!stat(filename, &st))
    return st.st_size;
  return -1;
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
  printf("syntax ok\n");
  return EXIT_SUCCESS;
}
