#ifndef CLI_H
#define CLI_H

#include <stdbool.h>
#include "hash_utils.h"

// Print usage information
void print_usage(const char *program_name);
// Show help message
void show_help(const char *program_name);
// Parse command-line arguments
int parse_args(int argc, char *argv[], const char *program_name);

// Validation functions
bool validate_algorithm(const char *alg_name, Hash_t *args);
bool validate_hash_format(const char *hash);
bool validate_wordlist_file(const char *wordlist_path);
bool validate_hash_length(const char *hash, const Hash_t *args);
void print_validation_error(const char *message, const char *program_name);

// Hash file processing functions
bool validate_hash_file(const char *hash_file_path);
int process_hash_file(const char *hash_file_path, const char *wordlist_path, Hash_t *args);

#endif // CLI_H
