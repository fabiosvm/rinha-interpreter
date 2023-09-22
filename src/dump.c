//
// dump.c
//

#include "dump.h"
#include <stdio.h>

void dump_function(Function *fn)
{
  int numNonlocals = fn->nonlocals.count;
  int numConstants = fn->constants.count;
  FunctionArray *functions = &fn->functions;
  int numFunctions = functions->count;
  printf("; %d parameter(s), %d non-local(s), %d constant(s), %d function(s)\n",
    fn->arity, numNonlocals, numConstants, numFunctions);
  int i = 0;
  int n = 0;
  Chunk *chunk = &fn->chunk;
  uint8_t *code = chunk->code;
  while (i < chunk->count)
  {
    OpCode op = (OpCode) code[i];
    printf("%5d ", i);
    ++i;
    ++n;
    switch (op)
    {
    case OP_NOP:
      printf("NOP\n");
      break;
    case OP_FALSE:
      printf("FALSE\n");
      break;
    case OP_TRUE:
      printf("TRUE\n");
      break;
    case OP_INT:
      {
        int data = *((uint16_t*) &code[i]);
        i += 2;
        printf("INT           %5d\n", data);
      }
      break;
    case OP_CONSTANT:
      {
        int index = code[i];
        ++i;
        printf("CONSTANT      %5d\n", index);
      }
      break;
    case OP_TUPLE:
      printf("TUPLE\n");
      break;
    case OP_CLOSURE:
      {
        int index = code[i];
        ++i;
        printf("CLOSURE       %5d\n", index);
      }
      break;
    case OP_LOCAL:
      {
        int index = code[i];
        ++i;
        printf("LOCAL         %5d\n", index);
      }
      break;
    case OP_NONLOCAL:
      {
        int index = code[i];
        ++i;
        printf("NONLOCAL      %5d\n", index);
      }
      break;
    case OP_ADD:
      printf("ADD\n");
      break;
    case OP_SUB:
      printf("SUB\n");
      break;
    case OP_MUL:
      printf("MUL\n");
      break;
    case OP_DIV:
      printf("DIV\n");
      break;
    case OP_REM:
      printf("REM\n");
      break;
    case OP_EQ:
      printf("EQ\n");
      break;
    case OP_NEQ:
      printf("NEQ\n");
      break;
    case OP_LT:
      printf("LT\n");
      break;
    case OP_GT:
      printf("GT\n");
      break;
    case OP_LTE:
      printf("LTE\n");
      break;
    case OP_GTE:
      printf("GTE\n");
      break;
    case OP_AND:
      printf("AND\n");
      break;
    case OP_OR:
      printf("OR\n");
      break;
    case OP_JUMP:
      {
        int offset = *((uint16_t*) &code[i]);
        i += 2;
        printf("JUMP          %5d\n", offset);
      }
      break;
    case OP_JUMP_IF_FALSE:
      {
        int offset = *((uint16_t*) &code[i]);
        i += 2;
        printf("JUMP_IF_FALSE %5d\n", offset);
      }
      break;
    case OP_FIRST:
      printf("FIRST\n");
      break;
    case OP_SECOND:
      printf("SECOND\n");
      break;
    case OP_PRINT:
      printf("PRINT\n");
      break;
    case OP_CALL:
      {
        int nargs = code[i];
        ++i;
        printf("CALL          %5d\n", nargs);
      }
      break;
    case OP_TAIL_CALL:
      {
        int nargs = code[i];
        ++i;
        printf("TAIL_CALL     %5d\n", nargs);
      }
      break;
    case OP_RETURN:
      printf("RETURN\n");
      break;
    case OP_HALT:
      printf("HALT\n");
      break;
    }
  }
  printf("; %d instruction(s)\n\n", n);
  for (int j = 0; j < numFunctions; ++j)
    dump_function(functions->elements[j]);
}
