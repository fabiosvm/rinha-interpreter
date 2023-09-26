//
// closure.c
//

#include "closure.h"
#include "hash.h"
#include "memory.h"

#if MEMORIZATION
static inline uint32_t key_hash(int arity, Value *args);
static inline bool key_equal(int arity, Value *args, Value *key);
#endif

#if MEMORIZATION
static inline uint32_t key_hash(int arity, Value *args)
{
  uint32_t hash;
  hash_init(&hash);
  for (int i = 0; i < arity; ++i)
    hash_update(&hash, value_hash(args[i]));
  return hash;
}
#endif

#if MEMORIZATION
static inline bool key_equal(int arity, Value *args, Value *key)
{
  for (int i = 0; i < arity; ++i)
    if (!value_equal(args[i], key[i]))
      return false;
  return true;
}
#endif

Closure *closure_new(Function *fn, Result *result)
{
#if MEMORIZATION
  int entrySize = fn->arity + 1;
  Value *memo = memory_alloc(sizeof(*memo) * entrySize * CLOSURE_MEMO_CAPACITY, result);
  if (!result_is_ok(result))
    return NULL;
  for (int i = 0; i < CLOSURE_MEMO_CAPACITY; ++i)
    memo[i * entrySize] = UNDEFINED_VALUE;
#endif
  int numNonlocals = fn->nonlocals.count;
  int size = sizeof(Closure) + sizeof(Value *) * (numNonlocals - 1);
  Closure *cl = memory_alloc(size, result);
  if (!result_is_ok(result))
    return NULL;
  cl->fn = fn;
#if MEMORIZATION
  cl->memo = memo;
#endif
  return cl;
}

#if MEMORIZATION
void closure_memorize(Closure *cl, Value *args, Value resultVal)
{
  int arity = cl->fn->arity;
  Value *memo = cl->memo;
  int index = key_hash(arity, args) & CLOSURE_MEMO_MASK;
  Value *entry = &memo[index * arity + 1];
  for (int i = 0; i < arity; ++i)
    entry[i] = args[i];
  entry[arity] = resultVal;
}
#endif

#if MEMORIZATION
Value closure_remember(Closure *cl, Value *args)
{
  int arity = cl->fn->arity;
  Value *memo = cl->memo;
  int index = key_hash(arity, args) & CLOSURE_MEMO_MASK;
  Value *entry = &memo[index * arity + 1];
  if (is_undefined(entry[0]))
    return UNDEFINED_VALUE;
  if (key_equal(arity, args, entry))
    return entry[arity];
  return UNDEFINED_VALUE;
}
#endif
