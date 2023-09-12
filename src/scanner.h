//
// scanner.h
//

#ifndef SCANNER_H
#define SCANNER_H

#include "result.h"

typedef enum
{
  TOKEN_EOF,
  TOKEN_COMMA,
  TOKEN_COLON,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_INT,
  TOKEN_STRING,
  TOKEN_FALSE_KW,
  TOKEN_TRUE_KW
} TokenType;

typedef struct
{
  TokenType type;
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
