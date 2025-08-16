#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <sys/stat.h>
#include "cli.h"
#include "hash_utils.h"

// Print usage information for the program
void print_usage(const char *program_name)
{
    if (program_name == NULL)
    {
        return;
    }

    printf("\nUsage : %s <ALG_NAME> <HASH> <WORDLIST_PATH>\n", program_name);
    printf("   or : %s <ALG_NAME> -f <HASH_FILE> <WORDLIST_PATH>\n", program_name);
    printf("Try '%s -h' for more information.\n", program_name);
}

// Show help message with usage and examples
void show_help(const char *program_name)
{
    printf("%s - Simple password hash cracking tool\n\n", program_name);

    printf("Usage:\n");
    printf("  %s <ALG_NAME> <HASH> <WORDLIST_PATH>\n", program_name);
    printf("  %s <ALG_NAME> -f <HASH_FILE> <WORDLIST_PATH>\n\n", program_name);
    printf("Arguments:\n");
    printf("  ALG_NAME      Hash algorithm to use (e.g., md5, sha1, sha256)\n");
    printf("  HASH          Target hash to crack (hexadecimal format)\n");
    printf("  HASH_FILE     File containing multiple hashes (one per line)\n");
    printf("  WORDLIST_PATH Path to the wordlist file\n\n");
    printf("Options:\n");
    printf("  -h, --help    Show this help message\n");
    printf("  -l, --list    List all supported algorithms\n");
    printf("  -f, --file    Read hashes from a file\n\n");
    printf("Examples:\n");
    printf("  %s md5 '5f4dcc3b5aa765d61d8327deb882cf99' rockyou.txt\n", program_name);
    printf("  %s sha1 '5baa61e4c9b93f3f0682250b6cf8331b7ee68fd8' rockyou.txt\n", program_name);
    printf("  %s sha256 '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8' rockyou.txt\n", program_name);
    printf("  %s md5 -f hashes.txt rockyou.txt\n", program_name);

    list_algorithms();
}

// Print validation error with usage information
void print_validation_error(const char *message, const char *program_name)
{
    fprintf(stderr, "\nError: %s\n", message);
    print_usage(program_name);
}

// Validate that the algorithm name is supported
bool validate_algorithm(const char *alg_name, Hash_t *args)
{
    if (alg_name == NULL || args == NULL)
    {
        return false;
    }

    size_t result_id = get_alg_id(alg_name, args);
    return (result_id != SIZE_MAX);
}

// Validate that the hash is in proper hexadecimal format
bool validate_hash_format(const char *hash)
{
    if (hash == NULL)
    {
        return false;
    }

    size_t hash_len = strlen(hash);

    // Check if hash is empty
    if (hash_len == 0)
    {
        return false;
    }

    // Check if all characters are hexadecimal
    for (size_t i = 0; i < hash_len; i++)
    {
        if (!isxdigit((unsigned char)hash[i]))
        {
            return false;
        }
    }

    return true;
}

// Validate that the hash length matches the expected length for the algorithm
bool validate_hash_length(const char *hash, const Hash_t *args)
{
    if (hash == NULL || args == NULL)
    {
        return false;
    }

    size_t hash_len = strlen(hash);
    size_t expected_len = args->digest_length * 2; // Each byte = 2 hex characters

    return (hash_len == expected_len);
}

// Validate that the wordlist file exists and is accessible
bool validate_wordlist_file(const char *wordlist_path)
{
    if (wordlist_path == NULL)
    {
        return false;
    }

    FILE *file = fopen(wordlist_path, "r");
    if (file == NULL)
    {
        return false;
    }

    fclose(file);
    return true;
}

// Validate that the hash file exists and is accessible
bool validate_hash_file(const char *hash_file_path)
{
    if (hash_file_path == NULL)
    {
        return false;
    }

    FILE *file = fopen(hash_file_path, "r");
    if (file == NULL)
    {
        return false;
    }

    fclose(file);
    return true;
}

// Process hash file and crack each hash
int process_hash_file(const char *hash_file_path, const char *wordlist_path, Hash_t *args)
{
    FILE *hash_file = fopen(hash_file_path, "r");
    if (hash_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open hash file '%s'\n", hash_file_path);
        return EXIT_FAILURE;
    }

    char line[1024];
    int line_number = 0;
    int total_hashes = 0;
    int cracked_hashes = 0;
    int failed_hashes = 0;

    printf("=== Processing hash file: %s ===\n", hash_file_path);
    printf("Algorithm: %s\n", args->name);
    printf("Wordlist: %s\n", wordlist_path);
    printf("=====================================\n\n");

    while (fgets(line, sizeof(line), hash_file))
    {
        line_number++;

        // Remove newline and carriage return
        line[strcspn(line, "\r\n")] = 0;

        // Skip empty lines and comments
        if (strlen(line) == 0 || line[0] == '#')
        {
            continue;
        }

        // Remove leading/trailing whitespace
        char *hash = line;
        while (isspace((unsigned char)*hash))
            hash++;

        char *end = hash + strlen(hash) - 1;
        while (end > hash && isspace((unsigned char)*end))
            end--;
        end[1] = '\0';

        // Skip if empty after trimming
        if (strlen(hash) == 0)
        {
            continue;
        }

        total_hashes++;

        // Validate hash format
        if (!validate_hash_format(hash))
        {
            fprintf(stderr, "Line %d: Invalid hash format '%s'\n", line_number, hash);
            failed_hashes++;
            continue;
        }

        // Validate hash length
        if (!validate_hash_length(hash, args))
        {
            size_t expected_len = args->digest_length * 2;
            size_t actual_len = strlen(hash);
            fprintf(stderr, "Line %d: Hash length mismatch for algorithm '%s'. Expected %zu characters, got %zu\n",
                    line_number, args->name, expected_len, actual_len);
            failed_hashes++;
            continue;
        }

        printf("Processing hash %d: %s\n", total_hashes, hash);

        // Attempt to crack the hash
        int crack_result = crack_hash(args, hash, wordlist_path);

        if (crack_result == EXIT_SUCCESS)
        {
            cracked_hashes++;
        }
        else
        {
            printf("Hash not found in wordlist.\n");
        }

        printf("\n");
    }

    fclose(hash_file);

    // Print summary
    printf("=== Summary ===\n");
    printf("Total hashes processed: %d\n", total_hashes);
    printf("Successfully cracked: %d\n", cracked_hashes);
    printf("Failed to crack: %d\n", total_hashes - cracked_hashes);
    printf("Invalid hashes: %d\n", failed_hashes);
    printf("================\n");

    return (cracked_hashes > 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

// Parse command-line arguments and handle options with robust validation
int parse_args(int argc, char *argv[], const char *program_name)
{
    // Check for help or list options first
    if (argc >= 2)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                show_help(program_name);
                return -2;
            }

            if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--list") == 0)
            {
                list_algorithms();
                return -2;
            }
        }
    }

    // Check for file mode (-f option)
    bool file_mode = false;
    int file_option_index = -1;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0)
        {
            file_mode = true;
            file_option_index = i;
            break;
        }
    }

    // Validate argument count based on mode
    if (file_mode)
    {
        // File mode: ./hashcrack <ALG_NAME> -f <HASH_FILE> <WORDLIST_PATH>
        if (argc != 5)
        {
            if (argc < 5)
            {
                fprintf(stderr, "\nError: Not enough arguments for file mode.\n");
                fprintf(stderr, "Expected 4 arguments, got %d.\n", argc - 1);
            }
            else
            {
                fprintf(stderr, "\nError: Too many arguments for file mode.\n");
                fprintf(stderr, "Expected 4 arguments, got %d.\n", argc - 1);
            }
            print_usage(program_name);
            return -1;
        }

        // Extract arguments for file mode
        const char *alg_name = argv[1];
        const char *hash_file_path = argv[file_option_index + 1];
        const char *wordlist_path = argv[file_option_index + 2];

        // Initialize hash algorithm structure for validation
        Hash_t args;
        args.name = NULL;
        args.description = NULL;
        args.id = 0;
        args.digest_length = 0;
        args.alg = NULL;

        // Validate algorithm
        if (!validate_algorithm(alg_name, &args))
        {
            fprintf(stderr, "\nError: Invalid or unsupported algorithm '%s'.\n", alg_name);
            fprintf(stderr, "Use '%s -l' to see all supported algorithms.\n", program_name);
            return -1;
        }

        // Validate hash file
        if (!validate_hash_file(hash_file_path))
        {
            fprintf(stderr, "\nError: Cannot access hash file '%s'.\n", hash_file_path);
            fprintf(stderr, "Please check that the file exists and is readable.\n");
            return -1;
        }

        // Validate wordlist file
        if (!validate_wordlist_file(wordlist_path))
        {
            fprintf(stderr, "\nError: Cannot access wordlist file '%s'.\n", wordlist_path);
            fprintf(stderr, "Please check that the file exists and is readable.\n");
            return -1;
        }

        // Process hash file
        return process_hash_file(hash_file_path, wordlist_path, &args);
    }
    else
    {
        // Standard mode: ./hashcrack <ALG_NAME> <HASH> <WORDLIST_PATH>
        if (argc != 4)
        {
            if (argc < 4)
            {
                fprintf(stderr, "\nError: Not enough arguments provided.\n");
                fprintf(stderr, "Expected 3 arguments, got %d.\n", argc - 1);
            }
            else
            {
                fprintf(stderr, "\nError: Too many arguments provided.\n");
                fprintf(stderr, "Expected 3 arguments, got %d.\n", argc - 1);
            }
            print_usage(program_name);
            return -1;
        }

        // Extract arguments
        const char *alg_name = argv[1];
        const char *hash = argv[2];
        const char *wordlist_path = argv[3];

        // Initialize hash algorithm structure for validation
        Hash_t args;
        args.name = NULL;
        args.description = NULL;
        args.id = 0;
        args.digest_length = 0;
        args.alg = NULL;

        // Validate algorithm
        if (!validate_algorithm(alg_name, &args))
        {
            fprintf(stderr, "\nError: Invalid or unsupported algorithm '%s'.\n", alg_name);
            fprintf(stderr, "Use '%s -l' to see all supported algorithms.\n", program_name);
            return -1;
        }

        // Validate hash format
        if (!validate_hash_format(hash))
        {
            fprintf(stderr, "\nError: Invalid hash format '%s'.\n", hash);
            fprintf(stderr, "Hash must be in hexadecimal format (e.g., '5f4dcc3b5aa765d61d8327deb882cf99').\n");
            return -1;
        }

        // Validate hash length
        if (!validate_hash_length(hash, &args))
        {
            size_t expected_len = args.digest_length * 2;
            size_t actual_len = strlen(hash);
            fprintf(stderr, "\nError: Hash length mismatch for algorithm '%s'.\n", args.name);
            fprintf(stderr, "Expected %zu characters, got %zu.\n", expected_len, actual_len);
            return -1;
        }

        // Validate wordlist file
        if (!validate_wordlist_file(wordlist_path))
        {
            fprintf(stderr, "\nError: Cannot access wordlist file '%s'.\n", wordlist_path);
            fprintf(stderr, "Please check that the file exists and is readable.\n");
            return -1;
        }

        // All validations passed for standard mode
        return 0;
    }
}