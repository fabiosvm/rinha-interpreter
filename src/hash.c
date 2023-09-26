//
// hash.c
//

#include "hash.h"

#define PRIME 16777619

void hash_init(uint32_t *hash)
{
  *hash = 2166136261;
}

void hash_update(uint32_t *hash, uint32_t data)
{
  *hash ^= data;
  *hash *= PRIME;
}

void hash_update_int(uint32_t *hash, int data)
{
  union
  {
    int data;
    uint8_t bytes[4];
  } u;
  u.data = data;
  *hash ^= u.bytes[0];
  *hash *= PRIME;
  *hash ^= u.bytes[1];
  *hash *= PRIME;
  *hash ^= u.bytes[2];
  *hash *= PRIME;
  *hash ^= u.bytes[3];
  *hash *= PRIME;
}

void hash_update_chars(uint32_t *hash, int length, char *chars)
{
  for (int i = 0; i < length; ++i)
  {
    *hash ^= chars[i];
    *hash *= PRIME;
  }
}

void hash_update_pointer(uint32_t *hash, void *ptr)
{
  union
  {
    void *ptr;
    uint8_t bytes[8];
  } u;
  u.ptr = ptr;
  *hash ^= u.bytes[0];
  *hash *= PRIME;
  *hash ^= u.bytes[1];
  *hash *= PRIME;
  *hash ^= u.bytes[2];
  *hash *= PRIME;
  *hash ^= u.bytes[3];
  *hash *= PRIME;
  *hash ^= u.bytes[4];
  *hash *= PRIME;
  *hash ^= u.bytes[5];
  *hash *= PRIME;
  *hash ^= u.bytes[6];
  *hash *= PRIME;
  *hash ^= u.bytes[7];
  *hash *= PRIME;
}
