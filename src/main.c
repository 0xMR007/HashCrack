#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "cli.h"
#include "utils.h"

// Banner to display at program start
const char *banner =
    "\n"
    "       ██╗  ██╗ █████╗ ███████╗██╗  ██╗ ██████╗██████╗  █████╗  ██████╗██╗  ██╗\n"
    "       ██║  ██║██╔══██╗██╔════╝██║  ██║██╔════╝██╔══██╗██╔══██╗██╔════╝██║ ██╔╝\n"
    "       ███████║███████║███████╗███████║██║     ██████╔╝███████║██║     █████╔╝ \n"
    "       ██╔══██║██╔══██║╚════██║██╔══██║██║     ██╔══██╗██╔══██║██║     ██╔═██╗ \n"
    "       ██║  ██║██║  ██║███████║██║  ██║╚██████╗██║  ██║██║  ██║╚██████╗██║  ██╗\n"
    "       ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝ \n"
    "                                         https://github.com/0xMR007";

int main(int argc, char *argv[])
{
    // Print the ASCII banner
    printf("%s\n\n", banner);
    const char *name = argv[0];

    // Parse and validate command-line arguments
    int result = parse_args(argc, argv, name);

    if (result == -2)
    {
        // Help or list displayed, exit successfully
        return EXIT_SUCCESS;
    }
    else if (result == EXIT_SUCCESS || result == EXIT_FAILURE)
    {
        // File mode was processed directly by parse_args
        // Return the result from file processing
        return result;
    }
    else if (result != 0)
    {
        // Validation failed, exit with error
        return EXIT_FAILURE;
    }

    // At this point, we're in standard mode with all arguments validated
    // Extract arguments (we know they exist and are valid)
    const char *alg_name = argv[1];
    const char *hash = argv[2];
    const char *wordlist = argv[3];

    // Initialize hash algorithm structure
    Hash_t args;
    args.name = NULL;
    args.description = NULL;
    args.id = 0;
    args.digest_length = 0;
    args.alg = NULL;

    // Get algorithm details (this should succeed since we validated it)
    size_t result_id = get_alg_id(alg_name, &args);
    if (result_id == SIZE_MAX)
    {
        fprintf(stderr, "Internal error: Algorithm validation failed for '%s'\n", alg_name);
        return EXIT_FAILURE;
    }

    printf("=== HashCrack CLI ===\n");
    printf("Algorithm   : %s (id: %zu)\n", args.name, args.id);
    printf("Target hash : %s\n", hash);
    printf("Wordlist    : %s\n", wordlist);
    printf("=======================\n");
    printf("Starting cracking process...\n");

    // Attempt to crack the hash
    int crack_result = crack_hash(&args, hash, wordlist);

    if (crack_result == EXIT_SUCCESS)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        printf("Check : hash format, wordlist, etc...\n");
        return EXIT_FAILURE;
    }
}