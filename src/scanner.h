//
// scanner.h
//

#ifndef SCANNER_H
#define SCANNER_H

#include "result.h"

typedef enum
{
  TOKEN_KIND_EOF,
  TOKEN_KIND_COMMA,
  TOKEN_KIND_COLON,
  TOKEN_KIND_LBRACKET,
  TOKEN_KIND_RBRACKET,
  TOKEN_KIND_LBRACE,
  TOKEN_KIND_RBRACE,
  TOKEN_KIND_INT,
  TOKEN_KIND_STRING,
  TOKEN_KIND_FALSE_KW,
  TOKEN_KIND_TRUE_KW
} TokenKind;

typedef struct
{
  TokenKind kind;
  int line;
  int col;
  int length;
  char *start;
} Token;

typedef struct
{
  char *source;
  char *pos;
  int line;
  int col;
  Token token;
} Scanner;

void scanner_init(Scanner *scan, char *source, Result *result);
void scanner_next_token(Scanner *scan, Result *result);

#endif // SCANNER_H
