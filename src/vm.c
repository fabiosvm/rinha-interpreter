//
// vm.c
//

#include "vm.h"
#include <stdio.h>
#include "tuple.h"

#ifndef TAIL_CALL_SWITCH
#define TAIL_CALL_SWITCH 1
#endif

#if TAIL_CALL_SWITCH
#define dispatch(vm, cl, ip, s, r) \
  do { \
    OpCode op = (OpCode) *(ip); \
    switch (op) \
    { \
    case OP_NOP:           do_nop((vm), (cl), (ip), (s), (r));           break; \
    case OP_FALSE:         do_false((vm), (cl), (ip), (s), (r));         break; \
    case OP_TRUE:          do_true((vm), (cl), (ip), (s), (r));          break; \
    case OP_CONSTANT:      do_constant((vm), (cl), (ip), (s), (r));      break; \
    case OP_TUPLE:         do_tuple((vm), (cl), (ip), (s), (r));         break; \
    case OP_CLOSURE:       do_closure((vm), (cl), (ip), (s), (r));       break; \
    case OP_LOCAL:         do_local((vm), (cl), (ip), (s), (r));         break; \
    case OP_NONLOCAL:      do_nonlocal((vm), (cl), (ip), (s), (r));      break; \
    case OP_ADD:           do_add((vm), (cl), (ip), (s), (r));           break; \
    case OP_SUB:           do_sub((vm), (cl), (ip), (s), (r));           break; \
    case OP_MUL:           do_mul((vm), (cl), (ip), (s), (r));           break; \
    case OP_DIV:           do_div((vm), (cl), (ip), (s), (r));           break; \
    case OP_REM:           do_rem((vm), (cl), (ip), (s), (r));           break; \
    case OP_EQ:            do_eq((vm), (cl), (ip), (s), (r));            break; \
    case OP_NEQ:           do_neq((vm), (cl), (ip), (s), (r));           break; \
    case OP_LT:            do_lt((vm), (cl), (ip), (s), (r));            break; \
    case OP_GT:            do_gt((vm), (cl), (ip), (s), (r));            break; \
    case OP_LTE:           do_lte((vm), (cl), (ip), (s), (r));           break; \
    case OP_GTE:           do_gte((vm), (cl), (ip), (s), (r));           break; \
    case OP_AND:           do_and((vm), (cl), (ip), (s), (r));           break; \
    case OP_OR:            do_or((vm), (cl), (ip), (s), (r));            break; \
    case OP_JUMP:          do_jump((vm), (cl), (ip), (s), (r));          break; \
    case OP_JUMP_IF_FALSE: do_jump_if_false((vm), (cl), (ip), (s), (r)); break; \
    case OP_FIRST:         do_first((vm), (cl), (ip), (s), (r));         break; \
    case OP_SECOND:        do_second((vm), (cl), (ip), (s), (r));        break; \
    case OP_PRINT:         do_print((vm), (cl), (ip), (s), (r));         break; \
    case OP_CALL:          do_call((vm), (cl), (ip), (s), (r));          break; \
    case OP_RETURN:        do_return((vm), (cl), (ip), (s), (r));        break; \
    case OP_HALT:          do_halt((vm), (cl), (ip), (s), (r));          break; \
    } \
  } while (0)
#endif

#if !TAIL_CALL_SWITCH
#define dispatch(vm, cl, ip, s, r) \
  do { \
    OpCode op = (OpCode) *(ip); \
    dispatchTable[op]((vm), (cl), (ip), (s), (r)); \
  } while (0)
#endif

static inline uint8_t read_byte(uint8_t **ip);
static inline uint16_t read_word(uint8_t **ip);
static inline void push(VM *vm, Value val, Result *result);
static inline void push_frame(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_nop(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_false(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_true(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_constant(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_tuple(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_closure(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_local(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_nonlocal(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_add(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_sub(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_mul(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_div(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_rem(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_eq(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_neq(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_lt(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_gt(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_lte(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_gte(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_and(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_or(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_jump(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_jump_if_false(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_first(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_second(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_print(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_call(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_return(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);
static void do_halt(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result);

#if !TAIL_CALL_SWITCH
typedef void (*InstructionFn)(VM *, Closure *, uint8_t *, Value *, Result *);

static const InstructionFn dispatchTable[] = {
  [OP_NOP]      = do_nop,      [OP_FALSE]         = do_false,         [OP_TRUE]    = do_true,
  [OP_CONSTANT] = do_constant, [OP_TUPLE]         = do_tuple,         [OP_CLOSURE] = do_closure,
  [OP_LOCAL]    = do_local,    [OP_NONLOCAL]      = do_nonlocal,      [OP_ADD]     = do_add,
  [OP_SUB]      = do_sub,      [OP_MUL]           = do_mul,           [OP_DIV]     = do_div,
  [OP_REM]      = do_rem,      [OP_EQ]            = do_eq,            [OP_NEQ]     = do_neq,
  [OP_LT]       = do_lt,       [OP_GT]            = do_gt,            [OP_LTE]     = do_lte,
  [OP_GTE]      = do_gte,      [OP_AND]           = do_and,           [OP_OR]      = do_or,
  [OP_JUMP]     = do_jump,     [OP_JUMP_IF_FALSE] = do_jump_if_false, [OP_FIRST]   = do_first,
  [OP_SECOND]   = do_second,   [OP_PRINT]         = do_print,         [OP_CALL]    = do_call,
  [OP_RETURN]   = do_return,   [OP_HALT]          = do_halt
};
#endif

static inline uint8_t read_byte(uint8_t **ip)
{
  uint8_t byte = **ip;
  ++(*ip);
  return byte;
}

static inline uint16_t read_word(uint8_t **ip)
{
  uint16_t word = *((uint16_t *) *ip);
  (*ip) += 2;
  return word;
}

static inline void push(VM *vm, Value val, Result *result)
{
  Stack *stack = &vm->stack;
  if (stack_is_full(stack))
  {
    result_error(result, "stack overflow");
    return;
  }
  stack_push(stack, val);
}

static inline void push_frame(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  Callstack *cstack = &vm->cstack;
  if (callstack_is_full(cstack))
  {
    result_error(result, "too many nested calls");
    return;
  }
  callstack_push(cstack, cl, ip, slots);
}

static void do_nop(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  dispatch(vm, cl, ip, slots, result);
}

static void do_false(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  push(vm, FALSE_VALUE, result);
  if (!result_is_ok(result))
    return;
  dispatch(vm, cl, ip, slots, result);
}

static void do_true(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  push(vm, TRUE_VALUE, result);
  if (!result_is_ok(result))
    return;
  dispatch(vm, cl, ip, slots, result);
}

static void do_constant(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  uint8_t index = read_byte(&ip);
  Array *constants = &cl->fn->constants;
  Value val = array_get(constants, index);
  push(vm, val, result);
  if (!result_is_ok(result))
    return;
  dispatch(vm, cl, ip, slots, result);
}

static void do_tuple(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value first = stack_peek(stack, 1);
  Value second = stack_peek(stack, 0);
  Tuple *tuple = tuple_new(first, second, result);
  if (!result_is_ok(result))
    return;
  stack_set(stack, 1, tuple_value(tuple));
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_closure(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  int index = read_byte(&ip);
  Function **functions = cl->fn->functions.elements;
  Function *fn = functions[index];
  Closure *newClosure = closure_new(fn, result);
  if (!result_is_ok(result))
    return;
  int n = fn->numNonlocals;
  Value *stackSlots = &vm->stack.top[1 - n];
  for (int i = 0; i < n; ++i)
    newClosure->nonlocals[i] = stackSlots[i];
  vm->stack.top -= n;
  push(vm, closure_value(newClosure), result);
  if (!result_is_ok(result))
    return;
  dispatch(vm, cl, ip, slots, result);
}

static void do_local(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  int index = read_byte(&ip);
  Value val = slots[index];
  push(vm, val, result);
  if (!result_is_ok(result))
    return;
  dispatch(vm, cl, ip, slots, result);
}

static void do_nonlocal(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  int index = read_byte(&ip);
  Value val = cl->nonlocals[index];
  push(vm, val, result);
  if (!result_is_ok(result))
    return;
  dispatch(vm, cl, ip, slots, result);
}

static void do_add(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  if (!is_int(val1) || !is_int(val2))
  {
    result_error(result, "cannot add %s and %s", type_name(type(val1)),
      type_name(type(val2)));
    return;
  }
  int num = as_int(val1) + as_int(val2);
  stack_set(stack, 1, int_value(num));
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_sub(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  if (!is_int(val1) || !is_int(val2))
  {
    result_error(result, "cannot subtract %s from %s", type_name(type(val2)),
      type_name(type(val1)));
    return;
  }
  int num = as_int(val1) - as_int(val2);
  stack_set(stack, 1, int_value(num));
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_mul(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  if (!is_int(val1) || !is_int(val2))
  {
    result_error(result, "cannot multiply %s by %s", type_name(type(val1)),
      type_name(type(val2)));
    return;
  }
  int num = as_int(val1) * as_int(val2);
  stack_set(stack, 1, int_value(num));
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_div(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  if (!is_int(val1) || !is_int(val2))
  {
    result_error(result, "cannot divide %s by %s", type_name(type(val1)),
      type_name(type(val2)));
    return;
  }
  int num = as_int(val1) / as_int(val2);
  stack_set(stack, 1, int_value(num));
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_rem(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  if (!is_int(val1) || !is_int(val2))
  {
    result_error(result, "cannot calculate remainder of %s and %s",
      type_name(type(val1)), type_name(type(val2)));
    return;
  }
  int num = as_int(val1) % as_int(val2);
  stack_set(stack, 1, int_value(num));
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_eq(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  bool isEqual = value_equal(val1, val2);
  stack_set(stack, 1, isEqual ? TRUE_VALUE : FALSE_VALUE);
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_neq(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  bool isEqual = value_equal(val1, val2);
  stack_set(stack, 1, isEqual ? FALSE_VALUE : TRUE_VALUE);
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_lt(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  int cmp = value_compare(val1, val2, result);
  if (!result_is_ok(result))
    return;
  stack_set(stack, 1, cmp < 0 ? TRUE_VALUE : FALSE_VALUE);
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_gt(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  int cmp = value_compare(val1, val2, result);
  if (!result_is_ok(result))
    return;
  stack_set(stack, 1, cmp > 0 ? TRUE_VALUE : FALSE_VALUE);
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_lte(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  int cmp = value_compare(val1, val2, result);
  if (!result_is_ok(result))
    return;
  stack_set(stack, 1, cmp > 0 ? FALSE_VALUE : TRUE_VALUE);
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_gte(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  int cmp = value_compare(val1, val2, result);
  if (!result_is_ok(result))
    return;
  stack_set(stack, 1, cmp < 0 ? FALSE_VALUE : TRUE_VALUE);
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_and(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  stack_set(stack, 1, is_falsy(val1) || is_falsy(val2) ? FALSE_VALUE : TRUE_VALUE);
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_or(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val1 = stack_peek(stack, 1);
  Value val2 = stack_peek(stack, 0);
  stack_set(stack, 1, is_falsy(val1) && is_falsy(val2) ? FALSE_VALUE : TRUE_VALUE);
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_jump(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  int offset = read_word(&ip);
  uint8_t *code = cl->fn->chunk.code;
  ip = &code[offset];
  dispatch(vm, cl, ip, slots, result);
}

static void do_jump_if_false(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  int offset = read_word(&ip);
  Stack *stack = &vm->stack;
  uint8_t *code = cl->fn->chunk.code;
  Value val = stack_peek(stack, 0);
  if (is_falsy(val))
    ip = &code[offset];
  stack_pop(stack);
  dispatch(vm, cl, ip, slots, result);
}

static void do_first(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val = stack_peek(stack, 0);
  if (!is_tuple(val))
  {
    result_error(result, "cannot get first element of %s", type_name(type(val)));
    return;
  }
  Tuple *tuple = as_tuple(val);
  stack_set(stack, 0, tuple->first);
  dispatch(vm, cl, ip, slots, result);
}

static void do_second(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val = stack_peek(stack, 0);
  if (!is_tuple(val))
  {
    result_error(result, "cannot get second element of %s", type_name(type(val)));
    return;
  }
  Tuple *tuple = as_tuple(val);
  stack_set(stack, 0, tuple->second);
  dispatch(vm, cl, ip, slots, result);
}

static void do_print(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  Stack *stack = &vm->stack;
  Value val = stack_peek(stack, 0);
  value_print(val);
  printf("\n");
  dispatch(vm, cl, ip, slots, result);
}

static void do_call(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  ++ip;
  int nargs = read_byte(&ip);
  Value *stackSlots = &vm->stack.top[-nargs];
  Value val = stackSlots[0];
  if (!is_closure(val))
  {
    result_error(result, "cannot call %s", type_name(type(val)));
    return;
  }
  Closure *callee = as_closure(val);
  if (callee->fn->arity != nargs)
  {
    result_error(result, "expected %d arguments, got %d", callee->fn->arity, nargs);
    return;
  }
  push_frame(vm, cl, ip, slots, result);
  if (!result_is_ok(result))
    return;
  ip = callee->fn->chunk.code;
  dispatch(vm, callee, ip, stackSlots, result);
}

static void do_return(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  CallFrame frame = callstack_pop(&vm->cstack);
  cl = frame.cl;
  ip = frame.ip;
  slots = frame.slots;
  dispatch(vm, cl, ip, slots, result);
}

static void do_halt(VM *vm, Closure *cl, uint8_t *ip, Value *slots, Result *result)
{
  (void) vm;
  (void) cl;
  (void) ip;
  (void) slots;
  (void) result;
}

void vm_init(VM *vm, int stackSize, int cstackSize, Result *result)
{
  stack_init(&vm->stack, stackSize, result);
  if (!result_is_ok(result))
    return;
  callstack_init(&vm->cstack, cstackSize, result);   
}

void vm_run(VM *vm, Closure *cl, Result *result)
{
  push(vm, closure_value(cl), result);
  if (!result_is_ok(result))
    return;
  uint8_t *ip = cl->fn->chunk.code;
  Value *slots = vm->stack.slots;
  dispatch(vm, cl, ip, slots, result);
}
