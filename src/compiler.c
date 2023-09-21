//
// compiler.c
//

#include "compiler.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "str.h"

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
  int length;
  char *start;
  bool isLocal;
  uint8_t index;
} Variable;

typedef struct Compiler
{
  struct Compiler *enclosing;
  Scanner *scan;
  bool emit;
  int numVariables;
  uint8_t nextIndex;
  Variable variables[UINT8_MAX];
  Function *fn;
} Compiler;

static inline void result_unexpected_token_error(Result *result, Scanner *scan);
static inline uint8_t add_constant(Compiler *comp, Value val, Result *result);
static inline void add_local(Compiler *comp, Token *token, Result *result);
static inline void add_variable(Compiler *comp, Token *token, bool isLocal, uint8_t index,
  Variable *out, Result *result);
static inline void resolve_variable(Compiler *comp, Token *token, Variable *out, Result *result);
static inline bool lookup_variable(Compiler *comp, Token *token, Variable *out);
static inline int emit_jump(Compiler *comp, OpCode op, Result *result);
static inline void patch_jump(Compiler *comp, int offset, Result *result);
static inline void compiler_init(Compiler *comp, Compiler *enclosing, Scanner *scan, bool emit,
  Result *result);
static inline void compile_file(Compiler *comp, Result *result);
static inline void compile_term(Compiler *comp, Result *result);
static inline void compile_location(Compiler *comp, Result *result);
static inline void compile_int(Compiler *comp, Result *result);
static inline void compile_str(Compiler *comp, Result *result);
static inline void compile_call(Compiler *comp, Result *result);
static inline void compile_binary(Compiler *comp, Result *result);
static inline void compile_function(Compiler *comp, Result *result);
static inline void compile_parameter(Compiler *comp, Result *result);
static inline void compile_let(Compiler *comp, Result *result);
static inline void compile_if(Compiler *comp, Result *result);
static inline void compile_print(Compiler *comp, Result *result);
static inline void compile_first(Compiler *comp, Result *result);
static inline void compile_second(Compiler *comp, Result *result);
static inline void compile_bool(Compiler *comp, Result *result);
static inline void compile_tuple(Compiler *comp, Result *result);
static inline void compile_var(Compiler *comp, Result *result);

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

static inline uint8_t add_constant(Compiler *comp, Value val, Result *result)
{
  Array *constants = &comp->fn->constants;
  int index = constants->count;
  if (index > UINT8_MAX)
  {
    result_error(result, "too many constants");
    return 0;
  }
  array_add(constants, val, result);
  if (!result_is_ok(result))
    return 0;
  return (uint8_t) index;
}

static inline void add_local(Compiler *comp, Token *token, Result *result)
{
  add_variable(comp, token, true, comp->nextIndex, NULL, result);
  if (!result_is_ok(result))
    return;
  ++comp->nextIndex;
}

static inline void add_variable(Compiler *comp, Token *token, bool isLocal, uint8_t index,
  Variable *out, Result *result)
{
  if (comp->numVariables > UINT8_MAX)
  {
    result_error(result, "too many variables");
    return;
  }
  Variable *var = &comp->variables[comp->numVariables];
  var->length = token->length;
  var->start = token->start;
  var->isLocal = isLocal;
  var->index = index;
  ++comp->numVariables;
  if (out)
    *out = *var;
}

static inline void resolve_variable(Compiler *comp, Token *token, Variable *out, Result *result)
{
  Variable var = { 0 };
  if (lookup_variable(comp, token, &var))
  {
    *out = var;
    return;
  }
  Compiler *enclosing = comp->enclosing;
  if (!enclosing)
  {
    result_error(result, "variable `%->*s` is used but not defined",
      token->length, token->start);
    return;
  }
  resolve_variable(enclosing, token, &var, result);
  if (!result_is_ok(result))
    return;
  uint8_t index = function_add_nonlocal(comp->fn, var.isLocal, var.index, result);
  if (!result_is_ok(result))
    return;
  add_variable(comp, token, false, index, &var, result);
  if (!result_is_ok(result))
    return;
  *out = var;
}

static inline bool lookup_variable(Compiler *comp, Token *token, Variable *out)
{
  for (int i = comp->numVariables - 1; i > -1; --i)
  {
    Variable *var = &comp->variables[i];
    if (token->length != var->length || strncmp(token->start, var->start, token->length))
      continue;
    *out = *var;
    return true;
  }
  return false;
}

static inline int emit_jump(Compiler *comp, OpCode op, Result *result)
{
  Chunk *chunk = &comp->fn->chunk;
  chunk_emit_byte(chunk, op, result);
  if (!result_is_ok(result))
    return 0;
  int offset = chunk->count;
  chunk_emit_word(chunk, 0, result);
  if (!result_is_ok(result))
    return 0;
  return offset;
}

static inline void patch_jump(Compiler *comp, int offset, Result *result)
{
  Chunk *chunk = &comp->fn->chunk;
  int jump = chunk->count;
  if (jump > UINT16_MAX)
  {
    result_error(result, "too much code to jump over");
    return;
  }
  *((uint16_t *) &chunk->code[offset]) = (uint16_t) jump;
}

static inline void compiler_init(Compiler *comp, Compiler *enclosing, Scanner *scan, bool emit,
  Result *result)
{
  Function *fn = function_new(0, result);
  if (!result_is_ok(result))
    return;
  comp->enclosing = enclosing;
  comp->scan = scan;
  comp->emit = emit;
  comp->numVariables = 0;
  comp->nextIndex = 1;
  comp->fn = fn;
}

static inline void compile_file(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
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
  if (comp->emit)
    chunk_emit_byte(&comp->fn->chunk, OP_HALT, result);
}

static inline void compile_term(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // {
  consume(scan, TOKEN_KIND_LBRACE, result);
  // "kind" : String
  consume_string(scan, "kind", result);
  consume(scan, TOKEN_KIND_COLON, result);
  char *kind = scan->token.start;
  consume(scan, TOKEN_KIND_STRING, result);
  // "Int"
  if (!strncmp(kind, "Int", 3))
  {
    compile_int(comp, result);
    return;
  }
  // "Str"
  if (!strncmp(kind, "Str", 3))
  {
    compile_str(comp, result);
    return;
  }
  // "Call"
  if (!strncmp(kind, "Call", 4))
  {
    compile_call(comp, result);
    return;
  }
  // "Binary" 
  if (!strncmp(kind, "Binary", 6))
  {
    compile_binary(comp, result);
    return;
  }
  // "Function"
  if (!strncmp(kind, "Function", 8))
  {
    compile_function(comp, result);
    return;
  }
  // "Let"
  if (!strncmp(kind, "Let", 3))
  {
    compile_let(comp, result);
    return;
  }
  // "If"
  if (!strncmp(kind, "If", 2))
  {
    compile_if(comp, result);
    return;
  }
  // "Print"
  if (!strncmp(kind, "Print", 5))
  {
    compile_print(comp, result);
    return;
  }
  // "First"
  if (!strncmp(kind, "First", 5))
  {
    compile_first(comp, result);
    return;
  }
  // "Second"
  if (!strncmp(kind, "Second", 6))
  {
    compile_second(comp, result);
    return;
  }
  // "Bool"
  if (!strncmp(kind, "Bool", 4))
  {
    compile_bool(comp, result);
    return;
  }
  // "Tuple"
  if (!strncmp(kind, "Tuple", 5))
  {
    compile_tuple(comp, result);
    return;
  }
  // "Var"
  if (!strncmp(kind, "Var", 3))
  {
    compile_var(comp, result);
    return;
  }
  result_unexpected_token_error(result, scan);
}

static inline void compile_location(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
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

static inline void compile_int(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // ,
  // "value" : Int ,
  // "location" : Location
  // }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "value", result);
  consume(scan, TOKEN_KIND_COLON, result);
  Token value = scan->token;
  consume(scan, TOKEN_KIND_INT, result);
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
  if (comp->emit)
  {
    Chunk *chunk = &comp->fn->chunk;
    int data = atoi(value.start);
    if (data <= UINT16_MAX)
    {
      chunk_emit_byte(chunk, OP_INT, result);
      if (!result_is_ok(result))
        return;
      chunk_emit_word(chunk, (uint16_t) data, result);
      return;
    }
    Value val = int_value(data);
    uint8_t index = add_constant(comp, val, result);
    if (!result_is_ok(result))
      return;
    chunk_emit_byte(chunk, OP_CONSTANT, result);
    if (!result_is_ok(result))
      return;
    chunk_emit_byte(chunk, index, result);
  }
}

static inline void compile_str(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // ,
  // "value" : String ,
  // "location" : Location
  // }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "value", result);
  consume(scan, TOKEN_KIND_COLON, result);
  Token value = scan->token;
  consume(scan, TOKEN_KIND_STRING, result);
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
  if (comp->emit)
  {
    Chunk *chunk = &comp->fn->chunk;
    Str *str = str_new(value.length, value.start, result);
    if (!result_is_ok(result))
      return;
    Value val = str_value(str);
    uint8_t index = add_constant(comp, val, result);
    if (!result_is_ok(result))
      return;
    chunk_emit_byte(chunk, OP_CONSTANT, result);
    if (!result_is_ok(result))
      return;
    chunk_emit_byte(chunk, index, result);
  }
}

static inline void compile_call(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // , "callee" : Term ,
  // "arguments" : [ Term , ... ] ,
  // "location" : Location
  // }
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
  uint8_t nargs = 0;
  if (match(scan, TOKEN_KIND_RBRACKET))
  {
    next_token(scan, result);
    goto end;
  }
  compile_term(comp, result);
  if (!result_is_ok(result))
    return;
  ++nargs;
  while (!match(scan, TOKEN_KIND_RBRACKET))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    compile_term(comp, result);
    if (!result_is_ok(result))
      return;
    ++nargs;
  }
  consume(scan, TOKEN_KIND_RBRACKET, result);
end:
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
  if (comp->emit)
  {
    Chunk *chunk = &comp->fn->chunk;
    chunk_emit_byte(chunk, OP_CALL, result);
    if (!result_is_ok(result))
      return;
    chunk_emit_byte(chunk, nargs, result);
  }
}

static inline void compile_binary(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // ,
  // "lhs" : Term ,
  // "op" : ( "Add" | "Sub" | "Mul" | "Div" | "Rem" | "Eq" | "Neq" | "Lt" | "Gt" | "Lte" | "Gte" | "And" | "Or" ) ,
  // "rhs" : Term ,
  // "location" : Location
  // }
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
  OpCode opcode = OP_NOP;
  consume(scan, TOKEN_KIND_STRING, result);
  if (!strncmp(op, "Add", 3))
    opcode = OP_ADD;
  else if (!strncmp(op, "Sub", 3))
    opcode = OP_SUB;
  else if (!strncmp(op, "Mul", 3))
    opcode = OP_MUL;
  else if (!strncmp(op, "Div", 3))
    opcode = OP_DIV;
  else if (!strncmp(op, "Rem", 3))
    opcode = OP_REM;
  else if (!strncmp(op, "Eq", 2))
    opcode = OP_EQ;
  else if (!strncmp(op, "Neq", 3))
    opcode = OP_NEQ;
  else if (!strncmp(op, "Lt", 2))
    opcode = OP_LT;
  else if (!strncmp(op, "Gt", 2))
    opcode = OP_GT;
  else if (!strncmp(op, "Lte", 3))
    opcode = OP_LTE;
  else if (!strncmp(op, "Gte", 3))
    opcode = OP_GTE;
  else if (!strncmp(op, "And", 3))
    opcode = OP_AND;
  else if (!strncmp(op, "Or", 2))
    opcode = OP_OR;
  if (opcode == OP_NOP)
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
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
  if (comp->emit)
    chunk_emit_byte(&comp->fn->chunk, opcode, result);
}

static inline void compile_function(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // ,
  // "parameters" : [ Parameter , ... ] ,
  // "value" : Term ,
  // "location" : Location
  // }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "parameters", result);
  consume(scan, TOKEN_KIND_COLON, result);
  consume(scan, TOKEN_KIND_LBRACKET, result);
  Compiler childComp;
  bool emit = comp->emit;
  compiler_init(&childComp, comp, scan, emit, result);
  if (!result_is_ok(result))
    return;
  if (match(scan, TOKEN_KIND_RBRACKET))
  {
    next_token(scan, result);
    goto end;
  }
  compile_parameter(&childComp, result);
  if (!result_is_ok(result))
    return;
  uint8_t arity = 1;
  while (!match(scan, TOKEN_KIND_RBRACKET))
  {
    consume(scan, TOKEN_KIND_COMMA, result);
    compile_parameter(&childComp, result);
    if (!result_is_ok(result))
      return;
    ++arity;
  }
  consume(scan, TOKEN_KIND_RBRACKET, result);
  childComp.fn->arity = arity;
end:
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "value", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_term(&childComp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
  if (emit)
  {
    chunk_emit_byte(&childComp.fn->chunk, OP_RETURN, result);
    if (!result_is_ok(result))
      return;
    uint8_t index = (uint8_t) comp->fn->functions.count;
    function_add_child(comp->fn, childComp.fn, result);
    if (!result_is_ok(result))
      return;
    Chunk *chunk = &comp->fn->chunk;
    chunk_emit_byte(chunk, OP_CLOSURE, result);
    if (!result_is_ok(result))
      return;
    chunk_emit_byte(chunk, index, result);
  }
}

static inline void compile_parameter(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // {
  consume(scan, TOKEN_KIND_LBRACE, result);
  // "text" : String ,
  consume_string(scan, "text", result);
  consume(scan, TOKEN_KIND_COLON, result);
  Token text = scan->token;
  consume(scan, TOKEN_KIND_STRING, result);
  consume(scan, TOKEN_KIND_COMMA, result);
  // "location" : Location
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  // }
  consume(scan, TOKEN_KIND_RBRACE, result);
  add_local(comp, &text, result);
}

static inline void compile_let(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // ,
  // "name" : Parameter ,
  // "value" : Term ,
  // "next" : Term ,
  // "location" : Location
  // }
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
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
}

static inline void compile_if(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  //  ,
  // "condition" : Term ,
  // "then" : Term ,
  // "otherwise" : Term ,
  // "location" : Location
  // }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "condition", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_term(comp, result);
  if (!result_is_ok(result))
    return;
  bool emit = comp->emit;
  int offset1 = -1;
  if (emit)
  {
    offset1 = emit_jump(comp, OP_JUMP_IF_FALSE, result);
    if (!result_is_ok(result))
      return;
  }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "then", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_term(comp, result);
  if (!result_is_ok(result))
    return;
  int offset2 = -1;
  if (emit)
  {
    offset2 = emit_jump(comp, OP_JUMP, result);
    if (!result_is_ok(result))
      return;
    assert(offset1 != -1);
    patch_jump(comp, offset1, result);
    if (!result_is_ok(result))
      return;
  }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "otherwise", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_term(comp, result);
  if (!result_is_ok(result))
    return;
  if (emit)
  {
    assert(offset2 != -1);
    patch_jump(comp, offset2, result);
    if (!result_is_ok(result))
      return;
  }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
}

static inline void compile_print(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // ,
  // "value" : Term ,
  // "location" : Location
  // }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "value", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_term(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
  if (comp->emit)
    chunk_emit_byte(&comp->fn->chunk, OP_PRINT, result);
}

static inline void compile_first(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // ,
  // "value" : Term ,
  // "location" : Location
  // }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "value", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_term(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
  if (comp->emit)
    chunk_emit_byte(&comp->fn->chunk, OP_FIRST, result);
}

static inline void compile_second(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // ,
  // "value" : Term ,
  // "location" : Location
  // }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "value", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_term(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
  if (comp->emit)
    chunk_emit_byte(&comp->fn->chunk, OP_SECOND, result);
}

static inline void compile_bool(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // ,
  // "value" : Bool ,
  // "location" : Location
  // }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "value", result);
  consume(scan, TOKEN_KIND_COLON, result);
  OpCode opcode = OP_NOP;
  if (match(scan, TOKEN_KIND_FALSE_KW))
  {
    next_token(scan, result);
    opcode = OP_FALSE;
  }
  else if (match(scan, TOKEN_KIND_TRUE_KW))
  {
    next_token(scan, result);
    opcode = OP_TRUE;
  }
  if (opcode == OP_NOP)
  {
    result_unexpected_token_error(result, scan);
    return;
  }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
  if (comp->emit)
    chunk_emit_byte(&comp->fn->chunk, opcode, result);
}

static inline void compile_tuple(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // ,
  // "first" : Term ,
  // "second" : Term ,
  // "location" : Location
  // }
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
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
  if (comp->emit)
    chunk_emit_byte(&comp->fn->chunk, OP_TUPLE, result);
}

static inline void compile_var(Compiler *comp, Result *result)
{
  Scanner *scan = comp->scan;
  // ,
  // "text" : String ,
  // "location" : Location
  // }
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "text", result);
  consume(scan, TOKEN_KIND_COLON, result);
  Token text = scan->token;
  consume(scan, TOKEN_KIND_STRING, result);
  consume(scan, TOKEN_KIND_COMMA, result);
  consume_string(scan, "location", result);
  consume(scan, TOKEN_KIND_COLON, result);
  compile_location(comp, result);
  if (!result_is_ok(result))
    return;
  consume(scan, TOKEN_KIND_RBRACE, result);
  Variable var;
  resolve_variable(comp, &text, &var, result);
  if (!result_is_ok(result))
    return;
  if (!comp->emit)
    return;
  Chunk *chunk = &comp->fn->chunk;
  OpCode op = var.isLocal ? OP_LOCAL : OP_NONLOCAL;
  chunk_emit_byte(chunk, op, result);
  if (!result_is_ok(result))
    return;
  chunk_emit_byte(chunk, var.index, result);
}

Closure *compile(char *source, bool emit, Result *result)
{
  Scanner scan;
  scanner_init(&scan, source, result);
  if (!result_is_ok(result))
    return NULL;
  Compiler comp;
  compiler_init(&comp, NULL, &scan, emit, result);
  if (!result_is_ok(result))
    return NULL;
  compile_file(&comp, result);
  Closure *cl = closure_new(comp.fn, result);
  if (!result_is_ok(result))
    return NULL;
  return cl;
}
