//
// hash.h
//

#ifndef HASH_H
#define HASH_H

#include <stdint.h>

void hash_init(uint32_t *hash);
void hash_update(uint32_t *hash, uint32_t data);
void hash_update_int(uint32_t *hash, int data);
void hash_update_chars(uint32_t *hash, int length, char *chars);
void hash_update_pointer(uint32_t *hash, void *ptr);

#endif // HASH_H
