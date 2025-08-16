#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <openssl/md5.h>
#include <openssl/md4.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include "hash_utils.h"
#include "utils.h"

// Helper functions for algorithms that need special handling
unsigned char *md5_sha1_combined(const unsigned char *input, size_t input_length, unsigned char *digest)
{
    unsigned char md5_digest[MD5_DIGEST_LENGTH];
    MD5(input, input_length, md5_digest);
    SHA1(md5_digest, MD5_DIGEST_LENGTH, digest);
    return digest;
}

unsigned char *sha256_192_truncated(const unsigned char *input, size_t input_length, unsigned char *digest)
{
    unsigned char full_digest[SHA256_DIGEST_LENGTH];
    SHA256(input, input_length, full_digest);
    memcpy(digest, full_digest, 24); // Copy only first 24 bytes (192 bits)
    return digest;
}

// Define supported hash algorithms

const Hash_t MD5_alg = {
    "MD5",
    "MD5 Hash Algorithm",
    0,
    MD5_DIGEST_LENGTH,
    MD5
};

const Hash_t MD4_alg = {
    "MD4",
    "MD4 Hash Algorithm",
    1,
    MD4_DIGEST_LENGTH,
    MD4
};

const Hash_t RIPEMD160_alg = {
    "RIPEMD160",
    "RIPEMD160 Hash Algorithm",
    2,
    RIPEMD160_DIGEST_LENGTH,
    RIPEMD160
};

const Hash_t SHA1_alg = {
    "SHA1",
    "SHA1 Hash Algorithm",
    3,
    SHA_DIGEST_LENGTH,
    SHA1
};

const Hash_t SHA224_alg = {
    "SHA224",
    "SHA224 Hash Algorithm",
    4,
    SHA224_DIGEST_LENGTH,
    SHA224
};

const Hash_t SHA256_alg = {
    "SHA256",
    "SHA256 Hash Algorithm",
    5,
    SHA256_DIGEST_LENGTH,
    SHA256
};

const Hash_t SHA384_alg = {
    "SHA384",
    "SHA384 Hash Algorithm",
    6,
    SHA384_DIGEST_LENGTH,
    SHA384
};

const Hash_t SHA512_alg = {
    "SHA512",
    "SHA512 Hash Algorithm",
    7,
    SHA512_DIGEST_LENGTH,
    SHA512
};

const Hash_t MD5_SHA1_alg = {
    "MD5-SHA1",
    "MD5 followed by SHA1 Hash Algorithm",
    8,
    SHA_DIGEST_LENGTH,
    md5_sha1_combined
};

const Hash_t SHA256_192_alg = {
    "SHA256-192",
    "SHA256-192 (truncated) Hash Algorithm",
    9,
    24, // 192 bits = 24 bytes
    sha256_192_truncated
};

// Array of supported algorithms
static Hash_t algs[] = {
    MD4_alg,
    MD5_alg,
    RIPEMD160_alg,
    SHA1_alg,
    SHA224_alg,
    SHA256_alg,
    SHA384_alg,
    SHA512_alg,
    MD5_SHA1_alg,
    SHA256_192_alg
};

static const size_t num_algorithms = sizeof(algs) / sizeof(algs[0]);

// Convert a hash digest (byte array) to a hexadecimal string
void hash_to_hex(const unsigned char digest[], size_t digest_length, char *output)
{
    for (size_t i = 0; i < digest_length; i++)
    {
        // Format each byte as two hexadecimal characters
        snprintf(&output[i * 2], 3, "%02x", (unsigned int)digest[i]);
    }
    // Null-terminate the output string
    output[digest_length * 2] = '\0';
}

// Compute the hash of an input string using the specified algorithm
char *hash_generic(const char *input, HashFunc_t hash_alg, size_t digest_length, char *output)
{
    // Allocate memory for the raw digest
    unsigned char *digest = malloc(digest_length);
    if (!digest)
    {
        fprintf(stderr, "\nDigest memory allocation failed in hash_generic.\n");
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    size_t input_length = strlen(input);

    // Apply the hash algorithm
    hash_alg((const unsigned char *)input, input_length, digest);

    // Convert the binary digest to a hexadecimal string
    hash_to_hex(digest, digest_length, output);
    free(digest);

    return output;
}

// List all supported hash algorithms
void list_algorithms()
{
    printf("\n=== List of supported hash algorithms ===\n");

    for (size_t i = 0; i < num_algorithms; i++)
    {
        printf(" [%zu] %s - ID : %zu\n", i, (&algs[i]) -> name, (&algs[i]) -> id);
    }

}

// Get the algorithm ID from user input (either by name or index)
size_t get_alg_id(const char *input, Hash_t *args)
{
    // Check if input is a number (index)
    char *end;
    size_t id = (size_t)strtoul(input, &end, 10);

    if (*end == '\0') // Conversion successful
    {
        if (id < num_algorithms)
        {
            set_args(&algs[id], args);
            return id;
        }

        // Unknown entry
        return SIZE_MAX;
    }

    // Otherwise, compare with algorithm names (case-insensitive)
    for (size_t i = 0; i < num_algorithms; i++)
    {
        if (strcasecmp(algs[i].name, input) == 0)
        {
            set_args(&algs[i], args);
            return algs[i].id;
        }
    }
    
    // Unknown entry
    return SIZE_MAX;
}

// Set the algorithm parameters in the args structure
int set_args(Hash_t *alg, Hash_t *args)
{
    if (args == NULL)
    {
        return -1;
    }
    
    args -> name = alg -> name;
    args -> description = alg -> description;
    args -> id = alg -> id;
    args -> digest_length = alg -> digest_length;
    args -> alg = alg -> alg;
    
    return 0;
}

// Attempt to crack the hash using the provided wordlist
int crack_hash(Hash_t *args, const char *hash, const char *wordlist)
{
    if (args == NULL)
    {
        fprintf(stderr, "\nHash structure is NULL, can't crack hash.\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(wordlist, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file '%s': %s\n", wordlist, strerror(errno));
        return EXIT_FAILURE;
    }

    int size = MAX_LENGTH;
    char buffer[size];
    char output[(args -> digest_length) * 2 + 1];
    char *line;
    
    // Read each line from the wordlist and hash it
    while ((line = read_line(file, buffer, size)))
    {
        hash_generic(line, args -> alg, args -> digest_length, output);

        // Compare the computed hash with the target hash (case-insensitive)
        if (strcasecmp(hash, output) == 0)
        {
            printf("\nFound pass : %s\n", line);
            fclose(file);
            return EXIT_SUCCESS;
        }
    }

    printf("\nNo pass was found.\n");
    return -1;
}