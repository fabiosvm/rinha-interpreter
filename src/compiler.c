//
// compiler.c
//

#include "compiler.h"
#include <stddef.h>
#include <string.h>
#include "scanner.h"

#define match(s, t) ((s)->token.kind == (t))

#define next_token(s, r) \
  do { \
    scanner_next_token(s, r); \
    if (!result_is_ok(r)) \
      return; \
  } while (0)

#define consume(s, t, r) \
  do { \
    if (!match(s, t)) { \
      result_unexpected_token_error(r, s); \
      return; \
    } \
    next_token(s, r); \
  } while (0)

#define consume_string(s, str, r) \
  do { \
    if (!match(s, TOKEN_KIND_STRING) || strncmp((s)->token.start, (str), (s)->token.length)) { \
      result_unexpected_token_error(r, s); \
      return; \
    } \
    next_token(s, r); \
  } while (0)

typedef struct
{
  Scanner scan;
  Function *fn;
} Compiler;

static inline void result_unexpected_token_error(Result *result, Scanner *scan);
static inline void compiler_init(Compiler *comp, char *source, Result *result);
static inline void compile_file(Compiler *comp, Result *result);
static inline void compile_term(Compiler *comp, Result *result);
static inline void compile_location(Compiler *comp, Result *result);
static inline void compile_parameter(Compiler *comp, Result *result);

static inline void result_unexpected_token_error(Result *result, Scanner *scan)
{
  Token *token = &scan->token;
  if (token->kind == TOKEN_KIND_EOF)
  {
    result_error(result, "unexpected end of file at %d, %d", token->line, token->col);
    return;
  }
  result_error(result, "unexpected token `%.*s` at %d, %d", token->length, token->start,
    token->line, token->col);
}

static inline void compiler_init(Compiler *comp, char *source, Result *result)
{
  scanner_init(&comp->scan, source, result);
  if (!result_is_ok(result))
    return;
  comp->fn = function_new(0, 0, result);
}

static inline void compile_file(Compiler *comp, Result *result)
{
  Scanner *scan = &comp->scan;
  // {
  consume(scan, TOKEN_KIND_LBRACE, result);
  // "name" : String ,
  consume_string(scan, "name", result);
  consume(scan, TOKEN_KIND_COLON, result);
  Token name = scan->token;
  consume(scan, TOKEN_KIND_STRING, result);
  (void) name;
  consume(scan, TOKEN_KIND_COMMA, result);
  // "expression" : Term ,
  consume_string(scan, "expression", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_term(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_COMMA, result);
  // "location" : Location
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  // }
  consume(scan, TOKEN_KIND_RBRACE, result);
}

static inline void compile_term(Compiler *comp, Result *result)
{
  Scanner *scan = &comp->scan;
  // {
  consume(scan, TOKEN_KIND_LBRACE, result);
  // "kind" : String
  consume_string(scan, "kind", result);
  consume(scan, TOKEN_KIND_COLON, result);
  Token kind = scan->token;
  consume(scan, TOKEN_KIND_STRING, result);
  // "Int" ,
  // "value" : Int
  if (!strncmp(kind.start, "Int", 3))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "value", result);
    consume(scan, TOKEN_KIND_COLON, result);
    Token value = scan->token;
    consume(scan, TOKEN_KIND_INT, result);
    (void) value;
    goto end;
  }
  // "Str" ,
  // "value" : String
  if (!strncmp(kind.start, "Str", 3))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "value", result);
    consume(scan, TOKEN_KIND_COLON, result);
    Token value = scan->token;
    consume(scan, TOKEN_KIND_STRING, result);
    (void) value;
    goto end;
  }
  // "Call" ,
  // "callee" : Term ,
  // "arguments" : [ Term , ... ]
  if (!strncmp(kind.start, "Call", 4))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "callee", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "arguments", result);
    consume(scan, TOKEN_KIND_COLON, result);
    consume(scan, TOKEN_KIND_LBRACKET, result);
    if (match(scan, TOKEN_KIND_RBRACKET))
    {
      next_token(scan, result);
      goto end;
    }
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    while (!match(scan, TOKEN_KIND_RBRACKET))
    {
      consume(scan, TOKEN_KIND_COMMA, result);
      compile_term(comp, result);
      if (!result_is_ok(result))
        return;
    }
    consume(scan, TOKEN_KIND_RBRACKET, result);
    goto end;
  }
  // "Binary" ,
  // "lhs" : Term ,
  // "op" : String ("Add"|"Sub"|"Mul"|"Div"|"Rem"|"Eq"|"Neq"|"Lt"|"Gt"|"Lte"|"Gte"|"And"|"Or") ,
  // "rhs" : Term
  if (!strncmp(kind.start, "Binary", 6))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "lhs", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "op", result);
    consume(scan, TOKEN_KIND_COLON, result);
    char *op = scan->token.start;
    consume(scan, TOKEN_KIND_STRING, result);
    if (strncmp(op, "Add", 3) && strncmp(op, "Sub", 3) && strncmp(op, "Mul", 3) &&
      strncmp(op, "Div", 3) && strncmp(op, "Rem", 3) && strncmp(op, "Eq", 2) &&
      strncmp(op, "Neq", 3) && strncmp(op, "Lt", 2) && strncmp(op, "Gt", 2) &&
      strncmp(op, "Lte", 3) && strncmp(op, "Gte", 3) && strncmp(op, "And", 3) &&
      strncmp(op, "Or", 2))
    {
      result_error(result, "unexpected operator `%s` at %d, %d", op, scan->token.line,
        scan->token.col);
      return;
    }
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "rhs", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    goto end;
  }
  // "Function" ,
  // "parameters" : [ Parameter , ... ] ,
  // "value" : Term
  if (!strncmp(kind.start, "Function", 8))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "parameters", result);
    consume(scan, TOKEN_KIND_COLON, result);
    consume(scan, TOKEN_KIND_LBRACKET, result);
    if (match(scan, TOKEN_KIND_RBRACKET))
    {
      next_token(scan, result);
      goto functionEnd;
    }
    compile_parameter(comp, result);
    if (!result_is_ok(result))
      return;
    while (!match(scan, TOKEN_KIND_RBRACKET))
    {
      consume(scan, TOKEN_KIND_COMMA, result);
      compile_parameter(comp, result);
      if (!result_is_ok(result))
        return;
    }
    consume(scan, TOKEN_KIND_RBRACKET, result);
  functionEnd:
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "value", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    goto end;
  }
  // "Let" ,
  // "name" : Parameter ,
  // "value" : Term ,
  // "next" : Term
  if (!strncmp(kind.start, "Let", 3))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "name", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_parameter(comp, result);
    if (!result_is_ok(result))
      return;
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "value", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "next", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    goto end;
  }
  // "If" ,
  // "condition" : Term ,
  // "then" : Term ,
  // "otherwise" : Term
  if (!strncmp(kind.start, "If", 2))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "condition", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "then", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "otherwise", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    goto end;
  }
  // "Print" ,
  // "value" : Term
  if (!strncmp(kind.start, "Print", 5))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "value", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    goto end;
  }
  // "First" ,
  // "value" : Term
  if (!strncmp(kind.start, "First", 5))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "value", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    goto end;
  }
  // "Second" ,
  // "value" : Term
  if (!strncmp(kind.start, "Second", 6))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "value", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    goto end;
  }
  // "Bool" ,
  // "value" Bool
  if (!strncmp(kind.start, "Bool", 4))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "value", result);
    consume(scan, TOKEN_KIND_COLON, result);
    Token value = scan->token;
    if (!match(scan, TOKEN_KIND_FALSE_KW) && !match(scan, TOKEN_KIND_TRUE_KW))
    {
      result_unexpected_token_error(result, scan);
      return;
    }
    next_token(scan, result);
    (void) value;
    goto end;
  }
  // "Tuple" ,
  // "first" : Term ,
  // "second" : Term
  if (!strncmp(kind.start, "Tuple", 5))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "first", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "second", result);
    consume(scan, TOKEN_KIND_COLON, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    goto end;
  }
  // "Var" ,
  // "text" : String
  if (!strncmp(kind.start, "Var", 3))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    consume_string(scan, "text", result);
    consume(scan, TOKEN_KIND_COLON, result);
    Token text = scan->token;
    consume(scan, TOKEN_KIND_STRING, result);
    (void) text;
    goto end;
  }
  result_unexpected_token_error(result, scan);
  return;
end:
  // , "location" : Location
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  // }
  consume(scan, TOKEN_KIND_RBRACE, result);
}

static inline void compile_location(Compiler *comp, Result *result)
{
  Scanner *scan = &comp->scan;
  // {
  consume(scan, TOKEN_KIND_LBRACE, result);
  // "start" : 	Int ,
  consume_string(scan, "start", result);
  consume(scan, TOKEN_KIND_COLON, result);
  Token start = scan->token;
  consume(scan, TOKEN_KIND_INT, result);
  (void) start;
  consume(scan, TOKEN_KIND_COMMA, result);
  // "end" : Int ,
  consume_string(scan, "end", result);
  consume(scan, TOKEN_KIND_COLON, result);
  Token end = scan->token;
  consume(scan, TOKEN_KIND_INT, result);
  (void) end;
  consume(scan, TOKEN_KIND_COMMA, result);
  // "filename" : String
  consume_string(scan, "filename", result);
  consume(scan, TOKEN_KIND_COLON, result);
  Token filename = scan->token;
  consume(scan, TOKEN_KIND_STRING, result);
  (void) filename;
  // }
  consume(scan, TOKEN_KIND_RBRACE, result);
}

static inline void compile_parameter(Compiler *comp, Result *result)
{
  Scanner *scan = &comp->scan;
  // {
  consume(scan, TOKEN_KIND_LBRACE, result);
  // "text" : String ,
  consume_string(scan, "text", result);
  consume(scan, TOKEN_KIND_COLON, result);
  Token text = scan->token;
  consume(scan, TOKEN_KIND_STRING, result);
  (void) text;
  consume(scan, TOKEN_KIND_COMMA, result);
  // "location" : Location
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  // }
  consume(scan, TOKEN_KIND_RBRACE, result);
}

Closure *compile(char *source, Result *result)
{
  Compiler comp;
  compiler_init(&comp, source, result);
  if (!result_is_ok(result))
    return NULL;
  compile_file(&comp, result);
  return NULL;
}
