//
// main.c
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "compiler.h"
#include "dump.h"
#include "str.h"

#ifdef _WIN32
  #define stat _stat
#endif

static inline void print_usage(const char *name);
static inline void version(void);
static inline void check(const char *filename);
static inline void dump(const char *filename);
static inline void run(const char *filename);
static inline Str *read_file(const char *filename);
static inline int file_size(const char *filename);
static inline FILE *open_file(const char *filename);

static inline void print_usage(const char *name)
{
  printf("Usage: %s <comand> [filename]\n", name);
  printf("\n");
  printf("Commands:\n");
  printf("\n");
  printf("  version  Print version information\n"); 
  printf("  check    Check syntax of file\n");
  printf("  dump     Dump bytecode of file\n");
  printf("  run      Run file\n");
  printf("\n");
}

static inline void version(void)
{
  printf("version 1.0.0\n");
}

static inline void check(const char *filename)
{
  Str *str = read_file(filename);
  Result result;
  result_ok(&result);
  (void) compile(str->chars, false, &result);
  if (!result_is_ok(&result))
  {
    fprintf(stderr, "fatal error: %s\n", result.error);
    exit(EXIT_FAILURE);
  }
  printf("syntax ok\n");
}

static inline void dump(const char *filename)
{
  Str *str = read_file(filename);
  Result result;
  result_ok(&result);
  Closure *cl = compile(str->chars, true, &result);
  if (!result_is_ok(&result))
  {
    fprintf(stderr, "fatal error: %s\n", result.error);
    exit(EXIT_FAILURE);
  }
  dump_function(cl->fn);
}

static inline void run(const char *filename)
{
  // TODO: Implement this function.
  (void) filename;
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
  if (argc < 2 || argc > 3)
  {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }
  char *command = argv[1];
  if (!strcmp(command, "version"))
    version();
  else if (!strcmp(command, "check"))
    check(argv[2]);
  else if (!strcmp(command, "dump"))
    dump(argv[2]);
  else if (!strcmp(command, "run"))
    run(argv[2]);
  else
  {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
