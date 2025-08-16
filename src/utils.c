#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <openssl/sha.h>

#include "utils.h"
#include "hash_utils.h"

// Function to read a line from a file
char *read_line(FILE *file, char *buffer, int size)
{
    if (file == NULL)
    {
        return NULL;
    }

    if (buffer == NULL)
    {
        printf("In read_line function : Error, given buffer is NULL\n");
        return NULL;
    }

    if (size > INT_MAX)
    {
        printf("Buffer size too large for fgets\n");
        return NULL;
    }
    
    // Read a line from the file
    if (fgets(buffer, size, file))
    {
        // Remove the newline character if present
        buffer[strcspn(buffer, "\n")] = 0;
        return buffer;
    }

    return NULL;
}