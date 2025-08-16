#ifndef HASH_UTILS_H
#define HASH_UTILS_H

#include <stdbool.h>

// Hash function pointer type
typedef unsigned char *(*HashFunc_t) (const unsigned char *, size_t, unsigned char *);

// Structure representing a hash algorithm and its properties
typedef struct
{
    const char *name;           // Algorithm name
    const char *description;    // Description
    size_t id;                  // Algorithm ID
    size_t digest_length;       // Digest length in bytes
    HashFunc_t alg;             // Hash function pointer
} Hash_t;

void hash_to_hex(const unsigned char digest[], size_t digest_length, char *output);
char *hash_generic(const char *input, HashFunc_t hash_alg, size_t digest_length, char *output);

size_t get_alg_id(const char *input, Hash_t *args);

void list_algorithms();

int set_args(Hash_t *alg, Hash_t *args);

int crack_hash(Hash_t *args, const char *hash, const char *wordlist);

#endif // HASH_UTILS_H