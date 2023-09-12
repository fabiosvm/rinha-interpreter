//
// scanner.c
//

#include "scanner.h"
#include <ctype.h>
#include <string.h>

#define KEYWORD_MAX_LENGTH 5

#define char_at(t, i)   ((t)->pos[(i)])
#define current_char(t) char_at(t, 0)

static inline void skip_spaces(Scanner *scan);
static inline void next_char(Scanner *scan);
static inline void next_chars(Scanner *scan, int n);
static inline bool match_char(Scanner *scan, const char c);
static inline bool match_int(Scanner *scan);
static inline bool match_string(Scanner *scan, Result *result);
static inline bool match_keyword(Scanner *scan, const char *kw);

static inline void skip_spaces(Scanner *scan)
{
  while (isspace(current_char(scan)))
    next_char(scan);
}

static inline void next_char(Scanner *scan)
{
  if (current_char(scan) == '\n')
  {
    ++scan->line;
    scan->col = 1;
    ++scan->pos;
    return;
  }
  ++scan->col;
  ++scan->pos;
}

static inline void next_chars(Scanner *scan, int n)
{
  for (int i = 0; i < n; ++i)
    next_char(scan);
}

static inline bool match_char(Scanner *scan, const char c)
{
  if (current_char(scan) != c)
    return false;
  scan->token.line = scan->line;
  scan->token.col = scan->col;
  scan->token.length = 1;
  scan->token.start = scan->pos;
  next_char(scan);
  return true;
}

static inline bool match_int(Scanner *scan)
{
  if (!isdigit(current_char(scan)))
    return false;
  int n = 1;
  while (isdigit(char_at(scan, n)))
    ++n;
  if (isalnum(char_at(scan, n)) || char_at(scan, n) == '_')
    return false;
  scan->token.type = TOKEN_INT;
  scan->token.line = scan->line;
  scan->token.col = scan->col;
  scan->token.length = n;
  scan->token.start = scan->pos;
  next_chars(scan, n);
  return true;
}

static inline bool match_string(Scanner *scan, Result *result)
{
  if (current_char(scan) != '\"')
    return false;
  int n = 1;
  for (;;)
  {
    if (char_at(scan, n) == '\"')
    {
      ++n;
      break;
    }
    if (char_at(scan, n) == '\0')
    {
      result_error(result, "unterminated string at %d, %d", scan->line, scan->col);
      return false;
    }
    ++n;
  }
  scan->token.type = TOKEN_STRING;
  scan->token.line = scan->line;
  scan->token.col = scan->col;
  scan->token.length = n - 2;
  scan->token.start = &scan->pos[1];
  next_chars(scan, n);
  return true;
}

static inline bool match_keyword(Scanner *scan, const char *kw)
{
  int n = (int) strnlen(kw, KEYWORD_MAX_LENGTH);
  if (strncmp(scan->pos, kw, n)
   || (isalnum(char_at(scan, n)))
   || (char_at(scan, n) == '_'))
    return false;
  scan->token.line = scan->line;
  scan->token.col = scan->col;
  scan->token.length = n;
  scan->token.start = scan->pos;
  next_chars(scan, n);
  return true;
}

void scanner_init(Scanner *scan, char *source, Result *result)
{
  scan->source = source;
  scan->pos = scan->source;
  scan->line = 1;
  scan->col = 1;
  scanner_next_token(scan, result);
}

void scanner_next_token(Scanner *scan, Result *result)
{
  skip_spaces(scan);
  if (match_char(scan, 0))
  {
    scan->token.type = TOKEN_EOF;
    return;
  }
  if (match_char(scan, ','))
  {
    scan->token.type = TOKEN_COMMA;
    return;
  }
  if (match_char(scan, ':'))
  {
    scan->token.type = TOKEN_COLON;
    return;
  }
  if (match_char(scan, '['))
  {
    scan->token.type = TOKEN_LBRACKET;
    return;
  }
  if (match_char(scan, ']'))
  {
    scan->token.type = TOKEN_RBRACKET;
    return;
  }
  if (match_char(scan, '{'))
  {
    scan->token.type = TOKEN_LBRACE;
    return;
  }
  if (match_char(scan, '}'))
  {
    scan->token.type = TOKEN_RBRACE;
    return;
  }
  if (match_int(scan))
    return;
  if (match_string(scan, result) || !result_is_ok(result))
    return;
  if (match_keyword(scan, "false"))
  {
    scan->token.type = TOKEN_FALSE_KW;
    return;
  }
  if (match_keyword(scan, "true"))
  {
    scan->token.type = TOKEN_TRUE_KW;
    return;
  }
  char c = isprint(current_char(scan)) ? current_char(scan) : '?';
  result_error(result, "unexpected character `%c` at %d, %d", c, scan->line, scan->col);
}
